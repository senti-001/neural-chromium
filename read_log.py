import os

def tail_file(filename, n=50):
    if not os.path.exists(filename):
        return f"{filename} not found."
    try:
        with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
            return ''.join(lines[-n:])
    except Exception as e:
        return f"Error reading {filename}: {e}"

output = "--- agent_benchmark.log ---\n"
output += tail_file("agent_benchmark.log")
output += "\n\n--- benchmark_debug_log.txt ---\n"
output += tail_file("benchmark_debug_log.txt")

with open("tail_log.txt", "w", encoding='utf-8') as f:
    f.write(output)
