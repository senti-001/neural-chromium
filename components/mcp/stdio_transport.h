#ifndef COMPONENTS_MCP_STDIO_TRANSPORT_H_
#define COMPONENTS_MCP_STDIO_TRANSPORT_H_

#include <memory>
#include <string>
#include <vector>

#include "base/files/file.h"
#include "base/process/process.h"
#include "base/threading/thread.h"
#include "components/mcp/mcp_transport.h"

namespace mcp {

// Transport that communicates with a local process via Stdin/Stdout.
class StdioTransport : public McpTransport {
 public:
  StdioTransport(const std::string& command, const std::vector<std::string>& args);
  ~StdioTransport() override;

  // McpTransport implementation:
  void Start(ReadCallback read_callback) override;
  void Send(const std::string& message) override;
  bool IsConnected() const override;
  void Close() override;

 private:
  void ReadLoop();

  std::string command_;
  std::vector<std::string> args_;
  
  ReadCallback read_callback_;
  base::Process process_;
  
  // Pipes for communication
  base::File stdin_file_;
  base::File stdout_file_;

  // Thread for reading stdout to avoid blocking the UI/IO thread.
  base::Thread reader_thread_;
  bool running_ = false;

  base::WeakPtrFactory<StdioTransport> weak_factory_{this};
};

}  // namespace mcp

#endif  // COMPONENTS_MCP_STDIO_TRANSPORT_H_
