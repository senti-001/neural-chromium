@echo off
set PYTHONPATH=%CD%\src
echo Starting Benchmark Run at %TIME% > benchmark_debug_log.txt
python src/run_benchmarks.py >> benchmark_debug_log.txt 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Benchmark exited with error code %ERRORLEVEL% >> benchmark_debug_log.txt
) else (
    echo Benchmark finished successfully >> benchmark_debug_log.txt
)
