@echo off
setlocal
echo [Neural-Launcher] Checking for Chrome Binary...

:: 1. Check AgentDebug (Common Dev target)
if exist "src\out\AgentDebug\chrome.exe" (
    set CHROME_BIN=src\out\AgentDebug\chrome.exe
    echo [Neural-Launcher] Found Debug Build (AgentDebug)
    goto Launch
)

:: 2. Check x64.release (Official Release target)
if exist "src\out\x64.release\chrome.exe" (
    set CHROME_BIN=src\out\x64.release\chrome.exe
    echo [Neural-Launcher] Found Release Build (x64.release)
    goto Launch
)

:: 3. Check x64.debug
if exist "src\out\x64.debug\chrome.exe" (
    set CHROME_BIN=src\out\x64.debug\chrome.exe
    echo [Neural-Launcher] Found Debug Build (x64.debug)
    goto Launch
)

echo [Neural-Launcher] ERROR: Could not find chrome.exe in out\AgentDebug, out\x64.release, or out\x64.debug.
echo Please ensure you have built the browser.
exit /b 1

:Launch
echo [Neural-Launcher] Launching %CHROME_BIN% ...
echo   --remote-debugging-port=9222
echo   --disable-gpu (Required for simple headless/remote setups)
echo   --no-first-run
echo   --no-default-browser-check

:: Launch
start "Neural-Chromium" "%CHROME_BIN%" --remote-debugging-port=9222 --start-maximized --no-first-run --no-default-browser-check --disable-gpu --disable-gpu-compositing --enable-logging --v=1 --no-sandbox --user-data-dir=C:\tmp\neural_chrome_profile





echo [Neural-Launcher] Browser Started.
exit /b 0
endlocal
