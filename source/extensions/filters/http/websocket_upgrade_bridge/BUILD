load("@envoy//bazel:envoy_build_system.bzl", "envoy_cc_library")

envoy_cc_library(
    name = "config",
    srcs = ["websocket_upgrade_bridge.cc", "config.cc"],
    hdrs = ["websocket_upgrade_bridge.h", "config.h"],
    deps = [
        "@envoy//source/common/common:base64",
        "@envoy//include/envoy/http:filter_interface",
        "@envoy//include/envoy/server:filter_config_interface",
        "@envoy//include/envoy/stream_info:filter_state_interface",
        "@openssl//:openssl",
    ],
    visibility = ["//visibility:public"],
)
