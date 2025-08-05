load("//tools/bazel:proto_library.bzl", "envoy_proto_library")
load("//bazel:envoy_build_system.bzl", "envoy_cc_library")

envoy_cc_library(
    name = "websocket_handshake_filter_lib",
    srcs = ["websocket_handshake_filter.cc"],
    hdrs = [],
    deps = [
        "@envoy//source/common/common",
        "@envoy//source/common/http",
        "@envoy//source/common/crypto",
        "@envoy//source/common/stream_info",
        "@envoy//source/extensions/filters/http/common:factory_base_lib",
        "@envoy//source/extensions/filters/http/common:config_lib",
        "@envoy//source/common/config:utility_lib",
        "@envoy//source/common/protobuf:utility_lib",
        "@envoy//api/envoy/config/filter/http",
        "@envoy//api/envoy/extensions/filters/http",
        "@envoy//source/server:filter_config_interface",
        "@com_google_absl//absl/strings",
    ],
)

envoy_cc_library(
    name = "websocket_handshake_filter",
    deps = [":websocket_handshake_filter_lib"],
)
