# Neural-Chromium Overlay

This repository contains the "Neural Layer" modifications for Chromium. It is designed to be pushed on top of a standard Chromium checkout.

## Architecture
This overlay implements:
*   **Zero-Latency IPC**: `components/agent_interface` (Shared Memory)
*   **Audio Injection**: Modifications to `media/renderers`
*   **Embedded MCP**: `components/mcp` (Model Context Protocol client)
*   **Visual Studio 2022 Support**: Patches for `build/vs_toolchain.py`

## Usage

1.  **Checkout Chromium**: Follow standard instructions to get `chromium/src`.
2.  **Apply Overlay**:
    Copy all files from this repository into your `src/` directory, overwriting existing files.
    ```bash
    cp -r * /path/to/chromium/src/
    ```
3.  **Build**:
    ```bash
    autoninja -C out/AgentDebug chrome
    ```
