#include "config.h"
#include "websocket_handshake_bridge.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketHandshakeBridge {

Http::FilterFactoryCb WebSocketHandshakeBridgeFilterFactory::createFilterFactoryFromProto(
    const Protobuf::Message&, const std::string&,
    Server::Configuration::FactoryContext&) {
  return [](Http::FilterChainFactoryCallbacks& callbacks) {
    auto filter = std::make_shared<WebSocketHandshakeBridgeFilter>();
    callbacks.addStreamDecoderFilter(filter);
    callbacks.addStreamEncoderFilter(filter);
  };
}

ProtobufTypes::MessagePtr WebSocketHandshakeBridgeFilterFactory::createEmptyConfigProto() {
  return std::make_unique<ProtobufWkt::Struct>(); // currently no config needed
}

REGISTER_FACTORY(WebSocketHandshakeBridgeFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace WebSocketHandshakeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
