#include "source/extensions/filters/http/websocket_handshake_bridge/websocket_handshake_bridge.h"

#include "test/mocks/http/mocks.h"
#include "test/mocks/stream_info/mocks.h"
#include "test/test_common/utility.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace WebSocketHandshakeBridge {
namespace {

class WebSocketHandshakeBridgeTest : public ::testing::Test {
public:
  WebSocketHandshakeBridgeFilter filter_;
  NiceMock<Http::MockStreamDecoderFilterCallbacks> decoder_callbacks_;
  NiceMock<Http::MockStreamEncoderFilterCallbacks> encoder_callbacks_;
  StreamInfo::StreamInfoImpl stream_info_;

  void SetUp() override {
    ON_CALL(decoder_callbacks_, streamInfo()).WillByDefault(testing::ReturnRef(stream_info_));
    ON_CALL(encoder_callbacks_, streamInfo()).WillByDefault(testing::ReturnRef(stream_info_));

    filter_.setDecoderFilterCallbacks(decoder_callbacks_);
    filter_.setEncoderFilterCallbacks(encoder_callbacks_);
  }
};

TEST_F(WebSocketHandshakeBridgeTest, InjectsKeyOnH2WebSocketRequest) {
  Http::TestRequestHeaderMapImpl headers{
      {":method", "CONNECT"},
      {":protocol", "websocket"},
      {":authority", "test.com"},
      {":path", "/chat"},
  };

  EXPECT_EQ(filter_.decodeHeaders(headers, false), Http::FilterHeadersStatus::Continue);

  EXPECT_EQ(headers.getUpgradeValue(), "websocket");
  EXPECT_EQ(headers.getConnectionValue(), "Upgrade");
  EXPECT_EQ(headers.get(Http::LowerCaseString("sec-websocket-version"))[0]->value().getStringView(), "13");
  EXPECT_FALSE(headers.get(Http::LowerCaseString("sec-websocket-key")).empty());

  // Verify FilterState has key
  auto* state = stream_info_.filterState()->getDataReadOnly<std::string>("websocket_handshake_key");
  EXPECT_NE(state, nullptr);
}

TEST_F(WebSocketHandshakeBridgeTest, AcceptsValidSecWebSocketAccept) {
  const std::string key = "dGhlIHNhbXBsZSBub25jZQ=="; // known value
  stream_info_.filterState()->setData(
      "websocket_handshake_key",
      std::make_unique<StreamInfo::StringAccessorImpl>(key),
      StreamInfo::FilterState::StateType::ReadOnly,
      StreamInfo::FilterState::LifeSpan::FilterChain
  );

  // This is computed from RFC example (key + GUID → SHA1 → base64)
  const std::string expected_accept = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";

  Http::TestResponseHeaderMapImpl headers{
      {":status", "101"},
      {"sec-websocket-accept", expected_accept},
  };

  EXPECT_EQ(filter_.encodeHeaders(headers, false), Http::FilterHeadersStatus::Continue);
}

TEST_F(WebSocketHandshakeBridgeTest, RejectsInvalidSecWebSocketAccept) {
  const std::string key = "badkey==";
  stream_info_.filterState()->setData(
      "websocket_handshake_key",
      std::make_unique<StreamInfo::StringAccessorImpl>(key),
      StreamInfo::FilterState::StateType::ReadOnly,
      StreamInfo::FilterState::LifeSpan::FilterChain
  );

  Http::TestResponseHeaderMapImpl headers{
      {":status", "101"},
      {"sec-websocket-accept", "wrong_value"},
  };

  EXPECT_EQ(filter_.encodeHeaders(headers, false), Http::FilterHeadersStatus::StopIteration);
}

} // namespace
} // namespace WebSocketHandshakeBridge
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
