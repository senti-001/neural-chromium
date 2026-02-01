#include "components/agent_interface/agent_shared_memory.h"

#include <cstring>

#include "base/logging.h"
#include "base/no_destructor.h"
#include "build/build_config.h"

#if BUILDFLAG(IS_WIN)
#include <windows.h>
#endif

namespace agent_interface {

AgentSharedMemory::AgentSharedMemory() = default;
AgentSharedMemory::~AgentSharedMemory() = default;

// static
AgentSharedMemory* AgentSharedMemory::GetInstance() {
  static base::NoDestructor<AgentSharedMemory> instance;
  return instance.get();
}

bool AgentSharedMemory::Initialize(size_t size) {
#if BUILDFLAG(IS_WIN)
  if (buffer_) return true; // Already initialized

  // Create named shared memory for Agent IPC
  // "NeuralChromium_Agent_SharedMem" matches nexus_agent.py
  HANDLE hMapFile = CreateFileMappingW(
      INVALID_HANDLE_VALUE,    // use paging file
      NULL,                    // default security
      PAGE_READWRITE,          // read/write access
      0,                       // maximum object size (high-order DWORD)
      (DWORD)size,             // maximum object size (low-order DWORD)
      L"NeuralChromium_Agent_SharedMem"); // name of mapping object

  if (hMapFile == NULL) {
    LOG(ERROR) << "Could not create file mapping object (" << GetLastError() << ").";
    return false;
  }

  void* pBuf = MapViewOfFile(
      hMapFile,            // handle to map object
      FILE_MAP_ALL_ACCESS, // read/write permission
      0,
      0,
      size);

  if (pBuf == NULL) {
    LOG(ERROR) << "Could not map view of file (" << GetLastError() << ").";
    CloseHandle(hMapFile);
    return false;
  }

  map_handle_ = hMapFile;
  buffer_ = pBuf;
  size_ = size;

  // Initialize Audio Buffer pointer (offset 16MB)
  // Assumes size is at least 32MB
  if (size >= 16 * 1024 * 1024) {
      audio_buffer_ = static_cast<uint8_t*>(buffer_) + 16 * 1024 * 1024;
  }

  LOG(INFO) << "Agent Named Shared Memory Created: NeuralChromium_Agent_SharedMem";
  return true;
#else
  LOG(ERROR) << "Named shared memory only supported on Windows for this demo.";
  return false;
#endif
}

void AgentSharedMemory::WriteFrame(const FrameHeader& header, const uint8_t* data, size_t data_size) {
  if (!buffer_) return;

  // Header is at offset 0
  memcpy(buffer_, &header, sizeof(FrameHeader));

  if (data && data_size > 0) {
      // Data follows header
      uint8_t* dest = static_cast<uint8_t*>(buffer_) + sizeof(FrameHeader);
      memcpy(dest, data, data_size);
  }
}

bool AgentSharedMemory::InitializeAudio(size_t size) {
    // Already handled in Initialize (single monolithic buffer)
    return buffer_ != nullptr;
}

void AgentSharedMemory::WriteAudio(const AudioHeader& header, const uint8_t* data, size_t data_size) {
  if (!audio_buffer_) return;

  // Audio Header is at offset 16MB
  memcpy(audio_buffer_, &header, sizeof(AudioHeader));

  if (data && data_size > 0) {
      // Audio Data follows audio header
      uint8_t* dest = static_cast<uint8_t*>(audio_buffer_) + sizeof(AudioHeader);
      memcpy(dest, data, data_size);
  }
}

}  // namespace agent_interface
