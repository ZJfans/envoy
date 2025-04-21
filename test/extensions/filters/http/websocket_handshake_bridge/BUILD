load("@envoy//bazel:envoy_build_system.bzl", "envoy_cc_test")

envoy_cc_test(
    name = "websocket_handshake_bridge_test",
    srcs = ["websocket_handshake_bridge_test.cc"],
    deps = [
        "@envoy//source/extensions/filters/http/websocket_handshake_bridge:websocket_handshake_bridge_lib",
        "@envoy//test/mocks/http:mocks",
        "@envoy//test/test_common",
        "@envoy//test/mocks/stream_info:mocks",
        "@envoy//source/common/http:header_map_impl",
        "@envoy//source/common/stream_info:stream_info_impl",
    ],
)
