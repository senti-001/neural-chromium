#ifndef COMPONENTS_AGENT_INTERFACE_AUDIO_HEADER_H_
#define COMPONENTS_AGENT_INTERFACE_AUDIO_HEADER_H_

#include <cstdint>

namespace agent_interface {

struct AudioHeader {
  uint32_t magic_number; // 0x41554449 "AUDI"
  uint32_t sample_rate;
  uint32_t channels;
  uint32_t samples_per_frame;
  uint64_t timestamp_us;
  uint32_t format; // 0=PCM_16BIT, 1=FLOAT_32BIT
  uint32_t frame_index;
};

}  // namespace agent_interface

#endif  // COMPONENTS_AGENT_INTERFACE_AUDIO_HEADER_H_
