#include "components/agent_interface/agent_shared_memory.h"

#include <cstring>

#include "base/logging.h"

#include "base/no_destructor.h"

namespace agent_interface {

AgentSharedMemory::AgentSharedMemory() = default;
AgentSharedMemory::~AgentSharedMemory() = default;

// static
AgentSharedMemory* AgentSharedMemory::GetInstance() {
  static base::NoDestructor<AgentSharedMemory> instance;
  return instance.get();
}

bool AgentSharedMemory::Initialize(size_t size) {
  base::MappedReadOnlyRegion mapped_region =
      base::ReadOnlySharedMemoryRegion::Create(size);
  if (!mapped_region.IsValid()) {
    LOG(ERROR) << "Failed to create shared memory region of size " << size;
    return false;
  }

  region_ = std::move(mapped_region.region);
  mapping_ = std::move(mapped_region.mapping);
  return true;
}

void AgentSharedMemory::WriteFrame(const FrameHeader& header, const uint8_t* data, size_t data_size) {
  if (!mapping_.IsValid()) {
    return;
  }

  // Calculate total size needed
  size_t val_size = sizeof(FrameHeader) + data_size;
  if (val_size > mapping_.size()) {
      LOG(ERROR) << "Frame too large for shared memory: " << val_size << " > " << mapping_.size();
      return;
  }

  // Write header
  memcpy(mapping_.memory(), &header, sizeof(FrameHeader));

  // Write pixel data immediately after header
  uint8_t* dest_pixels = static_cast<uint8_t*>(mapping_.memory()) + sizeof(FrameHeader);
  memcpy(dest_pixels, data, data_size);
}

bool AgentSharedMemory::InitializeAudio(size_t size) {
  base::MappedReadOnlyRegion mapped_region =
      base::ReadOnlySharedMemoryRegion::Create(size);
  if (!mapped_region.IsValid()) {
    LOG(ERROR) << "Failed to create audio shared memory region of size " << size;
    return false;
  }

  audio_region_ = std::move(mapped_region.region);
  audio_mapping_ = std::move(mapped_region.mapping);
  return true;
}

void AgentSharedMemory::WriteAudio(const AudioHeader& header, const uint8_t* data, size_t data_size) {
  if (!audio_mapping_.IsValid()) {
    return;
  }

  // Calculate total size needed
  size_t val_size = sizeof(AudioHeader) + data_size;
  if (val_size > audio_mapping_.size()) {
      LOG(ERROR) << "Audio buffer too large for shared memory: " << val_size << " > " << audio_mapping_.size();
      return;
  }

  // Write header
  memcpy(audio_mapping_.memory(), &header, sizeof(AudioHeader));

  // Write audio data immediately after header
  if (data && data_size > 0) {
      uint8_t* dest_audio = static_cast<uint8_t*>(audio_mapping_.memory()) + sizeof(AudioHeader);
      memcpy(dest_audio, data, data_size);
  }
}

}  // namespace agent_interface
