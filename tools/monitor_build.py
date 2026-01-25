
import sys
import time
import os
import re

LOG_FILE = r"out\AgentDebug\build.log"

def main():
    if not os.path.exists(LOG_FILE):
        print(f"Waiting for log file {LOG_FILE} to appear...")
        while not os.path.exists(LOG_FILE):
            time.sleep(1)
        print("Log file found. Monitoring build progress...")

    total_tasks = 0
    tasks_regex = re.compile(r"\[(\d+)/(\d+)\]")

    with open(LOG_FILE, "r") as f:
        # Seek to end initially? No, build starts fresh.
        while True:
            line = f.readline()
            if not line:
                time.sleep(0.5)
                continue
            
            match = tasks_regex.search(line)
            if match:
                current = int(match.group(1))
                total = int(match.group(2))
                percent = (current / total) * 100
                bar_length = 50
                filled_length = int(bar_length * current // total)
                bar = '█' * filled_length + '-' * (bar_length - filled_length)
                
                # Overwrite line
                sys.stdout.write(f"\rBuild Progress: |{bar}| {percent:.1f}% [{current}/{total}]")
                sys.stdout.flush()
                
            if "STAMP" in line and "chrome.exe" in line: # Rough check for completion
                print("\nBuild might be complete!")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nMonitoring stopped.")
