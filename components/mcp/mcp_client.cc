#include "components/mcp/mcp_client.h"

#include "base/logging.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"

namespace mcp {

McpClient::McpClient(std::unique_ptr<McpTransport> transport)
    : transport_(std::move(transport)) {}

McpClient::~McpClient() {
  if (transport_) {
    transport_->Close();
  }
}

void McpClient::Initialize() {
  DCHECK(transport_);
  // Start reading
  transport_->Start(base::BindRepeating(&McpClient::OnMessageReceived, weak_factory_.GetWeakPtr()));

  // Send "initialize" request
  base::Value::Dict params;
  params.Set("protocolVersion", "0.1.0");
  base::Value::Dict capabilities;
  params.Set("capabilities", std::move(capabilities));
  
  base::Value::Dict client_info;
  client_info.Set("name", "Neural-Chromium");
  client_info.Set("version", "1.0.0");
  params.Set("clientInfo", std::move(client_info));

  SendRequest("initialize", std::move(params));
}

void McpClient::SetInitCallback(InitCallback callback) {
  init_callback_ = std::move(callback);
}

void McpClient::CallTool(const std::string& tool_name, const base::Value::Dict& args, ToolCallback callback) {
  base::Value::Dict params;
  params.Set("name", tool_name);
  params.Set("arguments", args.Clone());
  SendRequest("tools/call", std::move(params));
  // TODO: register callback with request ID
}

void McpClient::OnMessageReceived(const std::string& message) {
  DVLOG(1) << "MCP Msg: " << message;
  // TODO: Parse JSON and handle response
}

void McpClient::SendRequest(const std::string& method, base::Value::Dict params) {
  base::Value::Dict request;
  request.Set("jsonrpc", "2.0");
  request.Set("method", method);
  request.Set("params", std::move(params));
  request.Set("id", 1); // TODO: Generate ID

  std::string json_string;
  base::JSONWriter::Write(request, &json_string);
  
  if (transport_->IsConnected()) {
    transport_->Send(json_string);
  }
}

}  // namespace mcp
