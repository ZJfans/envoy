#include "envoy/http/filter.h"
#include "envoy/server/filter_config.h"
#include "common/common/base64.h"
#include "common/common/utility.h"
#include "common/crypto/utility.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebsocketHandshake {

class WebsocketHandshakeFilter : public Http::StreamDecoderFilter, public Http::StreamEncoderFilter {
public:
  WebsocketHandshakeFilter() = default;

  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers, bool) override {
    const auto method = headers.getMethodValue();
    const auto upgrade = headers.getUpgradeValue();
    const bool is_websocket = (method == Http::Headers::get().MethodValues.Connect &&
                               headers.getProtocolValue() == "websocket") ||
                              StringUtil::caseInsensitiveCompare(upgrade, "websocket");
    if (is_websocket) {
      auto key_header = headers.get(Http::CustomHeaders::get().SecWebSocketKey);
      if (key_header == nullptr) {
        const std::string generated_key = Base64::encode(Random::String::generate(16), 16);
        headers.addCopy(Http::CustomHeaders::get().SecWebSocketKey, generated_key);
        sec_websocket_key_ = generated_key;
      } else {
        sec_websocket_key_ = std::string(key_header->value().getStringView());
      }
      headers.setCopy(Http::CustomHeaders::get().SecWebSocketVersion, "13");
    }
    return Http::FilterHeadersStatus::Continue;
  }

  Http::FilterHeadersStatus encodeHeaders(Http::ResponseHeaderMap& headers, bool) override {
    if (headers.getStatusValue() == Http::Headers::get().StatusValues.SwitchingProtocols &&
        !sec_websocket_key_.empty()) {
      auto accept_header = headers.get(Http::CustomHeaders::get().SecWebSocketAccept);
      if (!accept_header) {
        sendLocalReject("Missing Sec-WebSocket-Accept header");
        return Http::FilterHeadersStatus::StopIteration;
      }
      const std::string magic = sec_websocket_key_ + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
      const std::string sha1_result = Envoy::Common::Crypto::Utility::getSha1Digest(magic);
      const std::string expected = Base64::encode(sha1_result); // standard base64

      if (!StringUtil::caseInsensitiveCompare(accept_header->value().getStringView(), expected)) {
        sendLocalReject("Invalid Sec-WebSocket-Accept value");
        return Http::FilterHeadersStatus::StopIteration;
      }
    }
    return Http::FilterHeadersStatus::Continue;
  }

  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& cb) override {
    decoder_callbacks_ = &cb;
  }
  void setEncoderFilterCallbacks(Http::StreamEncoderFilterCallbacks& cb) override {
    encoder_callbacks_ = &cb;
  }

  Http::FilterDataStatus decodeData(Buffer::Instance&, bool) override {
    return Http::FilterDataStatus::Continue;
  }
  Http::FilterDataStatus encodeData(Buffer::Instance&, bool) override {
    return Http::FilterDataStatus::Continue;
  }
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap&) override {
    return Http::FilterTrailersStatus::Continue;
  }
  Http::FilterTrailersStatus encodeTrailers(Http::ResponseTrailerMap&) override {
    return Http::FilterTrailersStatus::Continue;
  }

private:
  void sendLocalReject(absl::string_view message) {
    if (decoder_callbacks_) {
      decoder_callbacks_->sendLocalReply(Http::Code::BadRequest, std::string(message), nullptr, absl::nullopt, "websocket_handshake_filter");
    }
  }

  std::string sec_websocket_key_;
  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{nullptr};
  Http::StreamEncoderFilterCallbacks* encoder_callbacks_{nullptr};
};

class WebsocketHandshakeFilterFactory : public Server::Configuration::NamedHttpFilterConfigFactory {
public:
  Http::FilterFactoryCb createFilterFactoryFromProto(const Protobuf::Message&, const std::string&,
                                                     Server::Configuration::FactoryContext&) override {
    return [](Http::FilterChainFactoryCallbacks& callbacks) {
      callbacks.addStreamFilter(std::make_shared<WebsocketHandshakeFilter>());
    };
  }

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return std::make_unique<ProtobufWkt::Struct>();
  }

  std::string name() const override {
    return "envoy.filters.http.websocket_handshake";
  }
};

REGISTER_FACTORY(WebsocketHandshakeFilterFactory, Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace WebsocketHandshake
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
