#ifndef COMPONENTS_AGENT_INTERFACE_AGENT_CONFIG_H_
#define COMPONENTS_AGENT_INTERFACE_AGENT_CONFIG_H_

#include <string>

namespace agent_interface {

enum class AIProvider {
  GEMINI,
  ANTHROPIC,
  OPENAI,
};

struct AgentConfig {
  AIProvider provider = AIProvider::GEMINI;
  std::string api_key;
  std::string model_name;
};

}  // namespace agent_interface

#endif  // COMPONENTS_AGENT_INTERFACE_AGENT_CONFIG_H_
