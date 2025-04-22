#pragma once

#include "envoy/http/filter.h"
#include "envoy/server/filter_config.h"
#include "envoy/stream_info/filter_state.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketUpgradeBridge {

class WebSocketUpgradeBridgeFilter : public Http::StreamDecoderFilter, public Http::StreamEncoderFilter {
public:
  WebSocketUpgradeBridgeFilter();

  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap&, bool) override;
  Http::FilterHeadersStatus encodeHeaders(Http::ResponseHeaderMap&, bool) override;

  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override;
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override;

private:
  std::string generateWebSocketKey() const;
  std::string computeAcceptKey(const std::string& key) const;

  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{};
  Http::StreamEncoderFilterCallbacks* encoder_callbacks_{};

  static constexpr absl::string_view kFilterStateKey = "websocket_key";
};

} // namespace WebSocketUpgradeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
