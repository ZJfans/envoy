load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_extension",
    "envoy_cc_library",
    "envoy_extension_package",
)

licenses(["notice"])

envoy_extension_package()

envoy_cc_library(
    name = "websocket_upgrade_bridge_lib",
    srcs = ["websocket_upgrade_bridge.cc"],
    hdrs = ["websocket_upgrade_bridge.h"],
    deps = [
        "//source/common/common:assert_lib",
        "//source/common/common:base64_lib",
        "//source/common/http:header_utility_lib",
        "//source/common/stream_info:stream_info_impl",
        "@envoy//include/envoy/http:filter_interface",
        "@envoy//include/envoy/stream_info:filter_state_interface",
    ],
)

envoy_cc_extension(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":websocket_upgrade_bridge_lib",
        "@envoy//envoy/server:filter_config_interface",
        "//envoy/registry",
        "//source/common/protobuf:utility_lib",
        "//source/extensions/filters/http/common:factory_base_lib",
    ],
)
