#include "websocket_handshake_bridge.h"

#include "common/common/base64.h"
#include "openssl/sha.h"
#include "openssl/rand.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketHandshakeBridge {

WebSocketHandshakeBridgeFilter::WebSocketHandshakeBridgeFilter() = default;

void WebSocketHandshakeBridgeFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

void WebSocketHandshakeBridgeFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
}

Http::FilterHeadersStatus WebSocketHandshakeBridgeFilter::decodeHeaders(Http::RequestHeaderMap& headers, bool) {
  const auto protocol = headers.get(Http::LowerCaseString(":protocol"));
  if (!protocol.empty() && protocol[0]->value().getStringView() == "websocket") {
    std::string key = generateSecWebSocketKey();
    headers.setUpgrade("websocket");
    headers.setConnection("Upgrade");
    headers.setReferenceSecWebSocketVersion("13");
    headers.setSecWebSocketKey(key);

    decoder_callbacks_->streamInfo().filterState()->setData(
      std::string(kFilterStateKey),
      std::make_unique<StreamInfo::StringAccessorImpl>(key),
      StreamInfo::FilterState::StateType::ReadOnly,
      StreamInfo::FilterState::LifeSpan::FilterChain
    );
  }
  return Http::FilterHeadersStatus::Continue;
}

Http::FilterHeadersStatus WebSocketHandshakeBridgeFilter::encodeHeaders(Http::ResponseHeaderMap& headers, bool) {
  if (headers.getStatusValue() != "101") {
    return Http::FilterHeadersStatus::Continue;
  }

  const auto& fs = encoder_callbacks_->streamInfo().filterState();
  if (!fs->hasData<std::string>(std::string(kFilterStateKey))) {
    ENVOY_LOG(warn, "WebSocketHandshakeBridge: missing key in FilterState");
    return Http::FilterHeadersStatus::Continue;
  }

  const auto& key = fs->getDataReadOnly<std::string>(std::string(kFilterStateKey));
  const auto accept = headers.get(Http::LowerCaseString("sec-websocket-accept"));
  if (accept.empty()) {
    ENVOY_LOG(error, "Missing Sec-WebSocket-Accept");
    return Http::FilterHeadersStatus::StopIteration;
  }

  const std::string expected = computeAcceptKey(key);
  if (accept[0]->value().getStringView() != expected) {
    ENVOY_LOG(error, "Invalid WebSocket Accept key. Expected: {}, Got: {}", expected, accept[0]->value().getStringView());
    return Http::FilterHeadersStatus::StopIteration;
  }

  return Http::FilterHeadersStatus::Continue;
}

std::string WebSocketHandshakeBridgeFilter::generateSecWebSocketKey() const {
  unsigned char key[16];
  RAND_bytes(key, sizeof(key));
  return Base64::encode(std::string(reinterpret_cast<char*>(key), sizeof(key)));
}

std::string WebSocketHandshakeBridgeFilter::computeAcceptKey(const std::string& key) const {
  static const std::string guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  std::string to_hash = key + guid;
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1(reinterpret_cast<const unsigned char*>(to_hash.c_str()), to_hash.size(), hash);
  return Base64::encode(std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH));
}

} // namespace WebSocketHandshakeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
