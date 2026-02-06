"""
System Specification Detection Script
Detects CPU, RAM, and GPU specs for VLM model selection
"""

import platform
import subprocess
import psutil

def get_cpu_info():
    """Get CPU information"""
    try:
        cpu_name = platform.processor()
        cpu_cores = psutil.cpu_count(logical=False)
        cpu_threads = psutil.cpu_count(logical=True)
        return {
            "name": cpu_name,
            "physical_cores": cpu_cores,
            "logical_cores": cpu_threads
        }
    except Exception as e:
        return {"error": str(e)}

def get_ram_info():
    """Get RAM information in GB"""
    try:
        ram = psutil.virtual_memory()
        return {
            "total_gb": round(ram.total / (1024**3), 2),
            "available_gb": round(ram.available / (1024**3), 2)
        }
    except Exception as e:
        return {"error": str(e)}

def get_gpu_info():
    """Get GPU information using wmic on Windows"""
    try:
        # Try to get GPU info via wmic
        result = subprocess.run(
            ["wmic", "path", "win32_VideoController", "get", "name,AdapterRAM"],
            capture_output=True,
            text=True,
            timeout=5
        )
        
        gpus = []
        lines = result.stdout.strip().split('\n')[1:]  # Skip header
        for line in lines:
            line = line.strip()
            if line:
                parts = line.rsplit(None, 1)
                if len(parts) == 2:
                    name, vram_bytes = parts
                    try:
                        vram_gb = round(int(vram_bytes) / (1024**3), 2)
                    except:
                        vram_gb = "Unknown"
                    gpus.append({"name": name.strip(), "vram_gb": vram_gb})
                else:
                    gpus.append({"name": line, "vram_gb": "Unknown"})
        
        return gpus if gpus else [{"name": "No GPU detected", "vram_gb": 0}]
    except Exception as e:
        return [{"error": str(e)}]

def print_specs():
    """Print system specifications"""
    print("=" * 60)
    print("SYSTEM SPECIFICATIONS FOR VLM MODEL SELECTION")
    print("=" * 60)
    
    # CPU
    cpu = get_cpu_info()
    print("\n📊 CPU:")
    print(f"  Name: {cpu.get('name', 'Unknown')}")
    print(f"  Physical Cores: {cpu.get('physical_cores', 'Unknown')}")
    print(f"  Logical Cores: {cpu.get('logical_cores', 'Unknown')}")
    
    # RAM
    ram = get_ram_info()
    print("\n💾 RAM:")
    print(f"  Total: {ram.get('total_gb', 'Unknown')} GB")
    print(f"  Available: {ram.get('available_gb', 'Unknown')} GB")
    
    # GPU
    gpus = get_gpu_info()
    print("\n🎮 GPU(s):")
    for i, gpu in enumerate(gpus, 1):
        if "error" in gpu:
            print(f"  Error detecting GPU: {gpu['error']}")
        else:
            print(f"  GPU {i}: {gpu.get('name', 'Unknown')}")
            print(f"    VRAM: {gpu.get('vram_gb', 'Unknown')} GB")
    
    print("\n" + "=" * 60)
    print("\nVLM MODEL RECOMMENDATIONS:")
    print("=" * 60)
    
    # Provide recommendations based on specs
    total_ram = ram.get('total_gb', 0)
    gpu_vram = max([g.get('vram_gb', 0) for g in gpus if isinstance(g.get('vram_gb'), (int, float))], default=0)
    
    print("\nBased on your hardware:")
    print(f"  Total RAM: {total_ram} GB")
    print(f"  GPU VRAM: {gpu_vram} GB")
    print()
    
    if gpu_vram >= 8:
        print("✅ You can run:")
        print("  - Llama 3.2 Vision 11B (recommended for better accuracy)")
        print("  - Moondream (current, lightweight)")
        print("  - Qwen2-VL 7B")
    elif gpu_vram >= 4:
        print("✅ You can run:")
        print("  - Moondream (current)")
        print("  - Qwen2-VL 2B")
        print("  - MiniCPM-V 2.6")
    elif total_ram >= 16:
        print("⚠️ Limited GPU VRAM, but sufficient RAM for CPU inference:")
        print("  - Moondream (current, CPU mode)")
        print("  - Qwen2-VL 2B (CPU mode, slow)")
    else:
        print("⚠️ Limited resources. Stick with:")
        print("  - Moondream (current, most lightweight)")
    
    print("\n" + "=" * 60)

if __name__ == "__main__":
    print_specs()
