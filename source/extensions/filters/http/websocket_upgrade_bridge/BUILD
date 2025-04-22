load("@envoy//bazel:envoy_build_system.bzl", "envoy_cc_library")

envoy_cc_library(
    name = "config",
    srcs = ["websocket_upgrade_bridge.cc", "config.cc"],
    hdrs = ["websocket_upgrade_bridge.h", "config.h"],
    deps = [
        "@envoy//source/common/common:base64_lib",
        "@envoy//include/envoy/http:filter_interface",
        "@envoy//include/envoy/server:filter_config_interface",
        "@envoy//include/envoy/stream_info:filter_state_interface",
        "@openssl//:openssl",
    ],
    visibility = ["//visibility:public"],
)

alias(
    name = "config_envoy_extension",
    actual = ":config",
    visibility = ["//visibility:public"],
)
