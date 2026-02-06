import sys
with open("debug_output.txt", "w") as f:
    f.write("Hello from Python\n")
print("Hello from Stdout")
print("Hello from Stderr", file=sys.stderr)
