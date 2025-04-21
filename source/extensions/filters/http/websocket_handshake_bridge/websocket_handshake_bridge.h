#pragma once

#include "envoy/http/filter.h"
#include "envoy/server/filter_config.h"
#include "envoy/stream_info/filter_state.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketHandshakeBridge {

class WebSocketHandshakeBridgeFilter : public Http::StreamDecoderFilter, public Http::StreamEncoderFilter {
public:
  WebSocketHandshakeBridgeFilter();

  // Decoder
  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers, bool end_stream) override;
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) override;

  // Encoder
  Http::FilterHeadersStatus encodeHeaders(Http::ResponseHeaderMap& headers, bool end_stream) override;
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) override;

private:
  std::string generateSecWebSocketKey() const;
  std::string computeAcceptKey(const std::string& key) const;

  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{};
  Http::StreamEncoderFilterCallbacks* encoder_callbacks_{};
  static constexpr absl::string_view kFilterStateKey = "websocket_handshake_key";
};

} // namespace WebSocketHandshakeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
