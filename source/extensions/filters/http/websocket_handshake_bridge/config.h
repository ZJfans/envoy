#pragma once

#include "envoy/server/filter_config.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketHandshakeBridge {

class WebSocketHandshakeBridgeFilterFactory : public Server::Configuration::NamedHttpFilterConfigFactory {
public:
  Http::FilterFactoryCb createFilterFactoryFromProto(const Protobuf::Message&, const std::string&,
                                                     Server::Configuration::FactoryContext&) override;

  ProtobufTypes::MessagePtr createEmptyConfigProto() override;

  std::string name() const override {
    return "envoy.filters.http.websocket_handshake_bridge";
  }
};

} // namespace WebSocketHandshakeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
