#include "websocket_upgrade_bridge.h"
#include "source/common/common/base64.h"
#include "openssl/sha.h"
#include "openssl/rand.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketUpgradeBridge {

WebSocketUpgradeBridgeFilter::WebSocketUpgradeBridgeFilter() = default;

void WebSocketUpgradeBridgeFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

void WebSocketUpgradeBridgeFilter::setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
}

Http::FilterHeadersStatus WebSocketUpgradeBridgeFilter::decodeHeaders(Http::RequestHeaderMap& headers, bool) {
  const auto method = headers.getMethodValue();
  const auto protocol = headers.get(Http::LowerCaseString(":protocol"));

  if (method == "CONNECT" && !protocol.empty() && protocol[0]->value() == "websocket") {
    std::string key = generateWebSocketKey();

    headers.setUpgrade("websocket");
    headers.setConnection("Upgrade");
    headers.setCopy(Http::LowerCaseString("sec-websocket-version"), "13");
    headers.setCopy(Http::LowerCaseString("sec-websocket-key"), key);

    decoder_callbacks_->streamInfo().filterState()->setData(
        std::string(kFilterStateKey),
        std::make_unique<StreamInfo::StringAccessorImpl>(key),
        StreamInfo::FilterState::StateType::ReadOnly,
        StreamInfo::FilterState::LifeSpan::FilterChain
    );
  }

  return Http::FilterHeadersStatus::Continue;
}

Http::FilterHeadersStatus WebSocketUpgradeBridgeFilter::encodeHeaders(Http::ResponseHeaderMap& headers, bool) {
  const auto& fs = encoder_callbacks_->streamInfo().filterState();
  if (!fs->hasData<std::string>(std::string(kFilterStateKey))) {
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
    ENVOY_LOG(error, "WebSocket Accept mismatch. Expected: {}, Got: {}", expected, accept[0]->value().getStringView());
    return Http::FilterHeadersStatus::StopIteration;
  }

  return Http::FilterHeadersStatus::Continue;
}

std::string WebSocketUpgradeBridgeFilter::generateWebSocketKey() const {
  unsigned char buf[16];
  RAND_bytes(buf, sizeof(buf));
  return Base64::encode(std::string(reinterpret_cast<char*>(buf), sizeof(buf)));
}

std::string WebSocketUpgradeBridgeFilter::computeAcceptKey(const std::string& key) const {
  static const std::string guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  std::string input = key + guid;

  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

  return Base64::encode(std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH));
}

} // namespace WebSocketUpgradeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
