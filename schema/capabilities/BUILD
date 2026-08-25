# Copyright 2026 The ODML Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# [Google-internal load of `cc_library`]
# [Google-internal load of `cc_test`]

package(
    default_hdrs_check = "strict",
    default_visibility = [
        "//visibility:public",
    ],
)

licenses(["notice"])

cc_library(
    name = "capabilities",
    hdrs = ["capabilities.h"],
    deps = [
        ":max_context_tokens",
        ":speculative_decoding",
    ],
)

cc_library(
    name = "max_context_tokens",
    srcs = ["max_context_tokens.cc"],
    hdrs = ["max_context_tokens.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "//runtime/proto:llm_metadata_cc_proto",
        "//runtime/util:litert_status_util",
        "//schema/core:litertlm_header_schema",
        "//schema/core:litertlm_read",
    ],
)

cc_library(
    name = "speculative_decoding",
    srcs = ["speculative_decoding.cc"],
    hdrs = ["speculative_decoding.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "//runtime/util:litert_status_util",
        "//schema/core:litertlm_header_schema",
        "//schema/core:litertlm_read",
    ],
)

cc_test(
    name = "max_context_tokens_test",
    srcs = ["max_context_tokens_test.cc"],
    deps = [
        ":max_context_tokens",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_matchers",
        "@flatbuffers//:runtime_cc",
        "//runtime/proto:llm_metadata_cc_proto",
        "//schema/core:litertlm_header",
        "//schema/core:litertlm_header_schema",
    ],
)

cc_test(
    name = "speculative_decoding_test",
    srcs = ["speculative_decoding_test.cc"],
    deps = [
        ":speculative_decoding",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:status_matchers",
        "@flatbuffers",
        "//schema/core:litertlm_header",
        "//schema/core:litertlm_header_schema",
    ],
)
