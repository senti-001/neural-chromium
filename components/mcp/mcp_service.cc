#include "components/mcp/mcp_service.h"

#include "base/logging.h"
#include "base/no_destructor.h"
#include "components/mcp/stdio_transport.h"

namespace mcp {

// static
McpService* McpService::GetInstance() {
  static base::NoDestructor<McpService> instance;
  return instance.get();
}

McpService::McpService() = default;
McpService::~McpService() = default;

void McpService::Initialize() {
  LOG(INFO) << "Initializing McpService";
  
  // TODO: Load real config. For now, we do nothing or could hardcode a test server
  // std::vector<std::string> args;
  // auto transport = std::make_unique<StdioTransport>("python", args);
  // AddClient(std::make_unique<McpClient>(std::move(transport)));
}

void McpService::AddClient(std::unique_ptr<McpClient> client) {
  clients_.push_back(std::move(client));
  clients_.back()->Initialize();
}

base::Value::List McpService::ListTools() {
  base::Value::List tools;
  // TODO: Aggregate from clients
  return tools;
}

void McpService::CallTool(const std::string& server_name, const std::string& tool_name, 
                          const base::Value::Dict& args, McpClient::ToolCallback callback) {
  // TODO: Router logic
}

}  // namespace mcp
