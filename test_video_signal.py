import sys
sys.path.insert(0, 'src/glazyr')
from nexus_agent import AgentSharedMemory

m = AgentSharedMemory()
for i in range(10):
    f = m.read_video_frame()
    if f:
        print(f"✅ Video Signal: {f['width']}x{f['height']}")
        break
    else:
        print(f"❌ No Signal (attempt {i+1}/10)")
        import time
        time.sleep(1)
