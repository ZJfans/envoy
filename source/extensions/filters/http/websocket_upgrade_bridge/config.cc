#include "config.h"
#include "websocket_upgrade_bridge.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketUpgradeBridge {

Http::FilterFactoryCb WebSocketUpgradeBridgeFilterFactory::createFilterFactoryFromProto(
    const Protobuf::Message&, const std::string&,
    Server::Configuration::FactoryContext&) {

  return [](Http::FilterChainFactoryCallbacks& callbacks) {
    auto filter = std::make_shared<WebSocketUpgradeBridgeFilter>();
    callbacks.addStreamDecoderFilter(filter);
    callbacks.addStreamEncoderFilter(filter);
  };
}

ProtobufTypes::MessagePtr WebSocketUpgradeBridgeFilterFactory::createEmptyConfigProto() {
  return std::make_unique<ProtobufWkt::Struct>();
}

REGISTER_FACTORY(WebSocketUpgradeBridgeFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace WebSocketUpgradeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
