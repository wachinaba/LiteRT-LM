// Copyright 2026 The ODML Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_ODML_LITERT_LM_SCHEMA_CAPABILITIES_MAX_CONTEXT_TOKENS_H_
#define THIRD_PARTY_ODML_LITERT_LM_SCHEMA_CAPABILITIES_MAX_CONTEXT_TOKENS_H_

#include <cstdint>
#include <istream>
#include <string>

#include "absl/status/statusor.h"  // from @com_google_absl

namespace litert::lm::schema::capabilities {

// Returns the maximum supported context tokens for the given LiteRT-LM stream.
absl::StatusOr<uint32_t> GetMaxContextTokens(std::istream& litertlm_stream);

// Returns the maximum supported context tokens for the given LiteRT-LM file
// path.
absl::StatusOr<uint32_t> GetMaxContextTokens(const std::string& litertlm_path);

}  // namespace litert::lm::schema::capabilities

#endif  // THIRD_PARTY_ODML_LITERT_LM_SCHEMA_CAPABILITIES_MAX_CONTEXT_TOKENS_H_
