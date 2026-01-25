import http.server
import socketserver
import os
import json
import re
import time

PORT = 8000
LOG_FILE = os.path.join("out", "AgentDebug", "build.log")
# Adjust path if running from src root
if not os.path.exists(LOG_FILE):
     # Try loading from subfolder if script is run from project root
    candidate = os.path.join("neural-chromium", "src", "out", "AgentDebug", "build.log")
    if os.path.exists(candidate):
        LOG_FILE = candidate

HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Neural-Chromium Build Command</title>
    <style>
        :root {
            --bg-color: #0d0d0d;
            --text-color: #00ff41;
            --dim-text: #008f11;
            --error-color: #ff003c;
            --panel-bg: #1a1a1a;
            --border-color: #333;
        }
        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            font-family: 'Consolas', 'Monaco', 'Courier New', monospace;
            margin: 0;
            padding: 20px;
            height: 100vh;
            box-sizing: border-box;
            display: flex;
            flex-direction: column;
        }
        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 2px solid var(--dim-text);
            padding-bottom: 15px;
            margin-bottom: 20px;
        }
        h1 { margin: 0; text-transform: uppercase; letter-spacing: 2px; text-shadow: 0 0 5px var(--text-color); }
        .status { font-size: 1.5em; font-weight: bold; }
        .status.running { color: #00e5ff; animation: pulse 2s infinite; }
        .status.failed { color: var(--error-color); }
        .status.success { color: #00ff41; }

        .progress-container {
            background-color: var(--panel-bg);
            border: 1px solid var(--border-color);
            height: 30px;
            width: 100%;
            margin-bottom: 20px;
            position: relative;
        }
        .progress-bar {
            background-color: var(--dim-text);
            height: 100%;
            width: 0%;
            transition: width 0.5s ease;
            box-shadow: 0 0 10px var(--dim-text);
        }
        .progress-text {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            color: #fff;
            mix-blend-mode: difference;
        }

        #log-window {
            flex-grow: 1;
            background-color: var(--panel-bg);
            border: 1px solid var(--border-color);
            padding: 15px;
            overflow-y: auto;
            white-space: pre-wrap;
            font-size: 0.9em;
            color: #ccc;
        }
        .log-line { margin-bottom: 2px; }
        .log-line.error { color: var(--error-color); }
        .log-line.ninja { color: #888; }
        
        @keyframes pulse {
            0% { opacity: 1; text-shadow: 0 0 5px #00e5ff; }
            50% { opacity: 0.7; text-shadow: 0 0 2px #00e5ff; }
            100% { opacity: 1; text-shadow: 0 0 5px #00e5ff; }
        }
    </style>
</head>
<body>
    <header>
        <h1>Neural-Chromium <span style="font-size:0.6em; color:#888;">// SYSTEM.BUILD</span></h1>
        <div id="status-display" class="status">INITIALIZING...</div>
    </header>

    <div class="progress-container">
        <div id="progress-bar" class="progress-bar"></div>
        <div id="progress-text" class="progress-text">0%</div>
    </div>

    <div id="log-window"></div>

    <script>
        const statusDisplay = document.getElementById('status-display');
        const progressBar = document.getElementById('progress-bar');
        const progressText = document.getElementById('progress-text');
        const logWindow = document.getElementById('log-window');

        function fetchStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    // Update Status
                    statusDisplay.textContent = data.status;
                    statusDisplay.className = 'status ' + data.status.toLowerCase();

                    // Update Progress
                    const pct = data.percentage + '%';
                    progressBar.style.width = pct;
                    progressText.textContent = `${data.current} / ${data.total} (${pct})`;

                    // Update Logs
                    logWindow.innerHTML = data.logs.join('\\n');
                    logWindow.scrollTop = logWindow.scrollHeight;
                })
                .catch(err => console.error(err));
        }

        setInterval(fetchStatus, 2000);
        fetchStatus();
    </script>
</body>
</html>
"""

class BuildHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(HTML_TEMPLATE.encode('utf-8'))
        elif self.path == '/status':
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(self.get_build_status()).encode('utf-8'))
        else:
            self.send_error(404)

    def get_build_status(self):
        status = "UNKNOWN"
        current = 0
        total = 0
        percentage = 0
        logs = []

        if not os.path.exists(LOG_FILE):
             return {
                "status": "NO LOG FOUND",
                "current": 0,
                "total": 0,
                "percentage": 0,
                "logs": ["Waiting for build.log..."]
            }

        try:
            with open(LOG_FILE, 'r') as f:
                # Read specific chunk relevant for tail
                # A robust implementation would be more efficient, but for <50MB logs this is ok
                # We'll seek to end - 10KB
                f.seek(0, 2)
                fsize = f.tell()
                f.seek(max(fsize - 20000, 0), 0)
                lines = f.read().splitlines()
                
                # Parse last lines for progress
                # Ninja format: [123/456] ...
                prog_re = re.compile(r'\[(\d+)/(\d+)\]')
                
                for line in reversed(lines):
                    match = prog_re.search(line)
                    if match:
                        current = int(match.group(1))
                        total = int(match.group(2))
                        if total > 0:
                            percentage = round((current / total) * 100, 1)
                        break
                
                # Determine status
                last_lines = lines[-20:]
                logs = []
                for l in last_lines:
                    # Simple color coding for HTML
                    cls = "log-line"
                    if "FAILED" in l or "error:" in l or "steps failed" in l:
                        status = "FAILED"
                        cls += " error"
                    elif "Building" in l or "[" in l:
                        status = "RUNNING"
                        cls += " ninja"
                    elif "Build Succeeded" in l:
                        status = "SUCCESS"
                        percentage = 100
                        current = total
                    
                    logs.append(f'<div class="{cls}">{l}</div>')
                
                if status == "UNKNOWN":
                    # Fallback check
                    if any("steps failed" in l for l in lines):
                         status = "FAILED"
                    elif any("Build Succeeded" in l for l in lines):
                         status = "SUCCESS"
                    else:
                         status = "IDLE/RUNNING"

        except Exception as e:
            logs.append(f"Error reading log: {str(e)}")

        return {
            "status": status,
            "current": current,
            "total": total,
            "percentage": percentage,
            "logs": logs
        }

if __name__ == "__main__":
    print(f"Serving Build Dashboard at http://localhost:{PORT}")
    print(f"Monitoring: {LOG_FILE}")
    http.server.test(HandlerClass=BuildHandler, port=PORT)
