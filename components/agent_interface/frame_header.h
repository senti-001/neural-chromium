#ifndef COMPONENTS_AGENT_INTERFACE_FRAME_HEADER_H_
#define COMPONENTS_AGENT_INTERFACE_FRAME_HEADER_H_

#include <stdint.h>

namespace agent_interface {

struct FrameHeader {
  uint32_t magic_number; // 0x4E43524D "NCRM" (Neural Chromium)
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint64_t timestamp_us;
  uint32_t format; // 0=RGBA, 1=BGRA
  uint32_t frame_index;
};

}  // namespace agent_interface

#endif  // COMPONENTS_AGENT_INTERFACE_FRAME_HEADER_H_
