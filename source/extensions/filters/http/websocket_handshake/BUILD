load("//bazel:envoy_cc_binary.bzl", "envoy_cc_library")

envoy_cc_library(
    name = "websocket_handshake_filter_lib",
    srcs = [
        "websocket_handshake_filter.cc",
    ],
    hdrs = [
        "websocket_handshake_filter.h",
    ],
    deps = [
        "//source/common/common",
        "//source/common/http",
        "//source/common/crypto",
        "//source/extensions/filters/http/common",
    ],
)

envoy_cc_library(
    name = "config",
    deps = [":websocket_handshake_filter_lib"],
)
