#ifndef COMPONENTS_MCP_MCP_SERVICE_H_
#define COMPONENTS_MCP_MCP_SERVICE_H_

#include <memory>
#include <string>
#include <vector>

#include "base/no_destructor.h"
#include "components/mcp/mcp_client.h"

namespace mcp {

// Singleton service that manages all MCP clients.
class McpService {
 public:
  static McpService* GetInstance();

  // Load server configuration and connect to servers.
  void Initialize();

  // Add a client manually (for testing).
  void AddClient(std::unique_ptr<McpClient> client);

  // List all available tools from all connected servers.
  // Returns a list of tool definitions (JSON).
  base::Value::List ListTools();

  // Call a tool on the appropriate server.
  void CallTool(const std::string& server_name, const std::string& tool_name, 
                const base::Value::Dict& args, McpClient::ToolCallback callback);

 private:
  friend class base::NoDestructor<McpService>;
  McpService();
  ~McpService();

  std::vector<std::unique_ptr<McpClient>> clients_;
};

}  // namespace mcp

#endif  // COMPONENTS_MCP_MCP_SERVICE_H_
