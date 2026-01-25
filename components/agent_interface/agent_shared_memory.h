#ifndef COMPONENTS_AGENT_INTERFACE_AGENT_SHARED_MEMORY_H_
#define COMPONENTS_AGENT_INTERFACE_AGENT_SHARED_MEMORY_H_

#include <memory>
#include <string>

#include "base/memory/read_only_shared_memory_region.h"
#include "base/memory/shared_memory_mapping.h"
#include "components/agent_interface/agent_config.h"
#include "components/agent_interface/frame_header.h"
#include "components/agent_interface/audio_header.h"

namespace agent_interface {

class AgentSharedMemory {
 public:
  AgentSharedMemory();
  ~AgentSharedMemory();

  static AgentSharedMemory* GetInstance();

  // Initialize the shared memory region with the given size.
  bool Initialize(size_t size);

  // Write a frame to the shared memory.
  void WriteFrame(const FrameHeader& header, const uint8_t* data, size_t data_size);

  // Write audio data to the shared memory (separate region or offset).
  // For simplicity V1, we might repurpose or add a separate region.
  // For now, let's assume we might need a separate method to initialize audio memory.
  bool InitializeAudio(size_t size);
  void WriteAudio(const AudioHeader& header, const uint8_t* data, size_t data_size);
  const base::ReadOnlySharedMemoryRegion& GetAudioRegion() const { return audio_region_; }

  // Get the READ-ONLY shared memory region to pass to the agent process.
  const base::ReadOnlySharedMemoryRegion& GetRegion() const { return region_; }

 private:
  base::ReadOnlySharedMemoryRegion region_;
  base::WritableSharedMemoryMapping mapping_;

  base::ReadOnlySharedMemoryRegion audio_region_;
  base::WritableSharedMemoryMapping audio_mapping_;
};

}  // namespace agent_interface

#endif  // COMPONENTS_AGENT_INTERFACE_AGENT_SHARED_MEMORY_H_
