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

#include "schema/capabilities/max_context_tokens.h"

#include <cstddef>
#include <cstdint>
#include <ios>
#include <istream>
#include <memory>
#include <string>

#include "absl/status/status.h"  // from @com_google_absl
#include "absl/status/status_macros.h"  // from @com_google_absl
#include "absl/status/statusor.h"  // from @com_google_absl

#include "runtime/proto/llm_metadata.pb.h"
#include "runtime/util/status_macros.h"
#include "schema/core/litertlm_header_schema_generated.h"
#include "schema/core/litertlm_read.h"

namespace litert::lm::schema::capabilities {

absl::StatusOr<uint32_t> GetMaxContextTokens(std::istream& litertlm_stream) {
  litertlm_stream.seekg(0);
  LitertlmHeader header;
  ABSL_RETURN_IF_ERROR(ReadHeaderFromLiteRTLM(litertlm_stream, &header));

  const LiteRTLMMetaData* litertlm_metadata = header.metadata;
  RET_CHECK_NE(litertlm_metadata, nullptr);
  const litert::lm::schema::SectionMetadata* section_metadata_obj =
      litertlm_metadata->section_metadata();
  RET_CHECK_NE(section_metadata_obj, nullptr);
  auto section_objects = section_metadata_obj->objects();
  RET_CHECK_NE(section_objects, nullptr);

  for (size_t i = 0; i < section_objects->size(); ++i) {
    const auto* section = section_objects->Get(i);
    if (section->data_type() == AnySectionDataType_LlmMetadataProto) {
      if (section->end_offset() < section->begin_offset()) {
        return absl::InternalError("Invalid section offsets.");
      }
      litertlm_stream.seekg(section->begin_offset());
      size_t size = section->end_offset() - section->begin_offset();
      std::unique_ptr<char[]> buffer(new char[size]);
      litertlm_stream.read(buffer.get(), size);
      if (!litertlm_stream) {
        return absl::InternalError("Failed to read LlmMetadataProto section.");
      }
      litert::lm::proto::LlmMetadata llm_metadata;
      if (!llm_metadata.ParseFromArray(buffer.get(), size)) {
        return absl::InternalError("Failed to parse LlmMetadataProto.");
      }
      return llm_metadata.max_num_tokens();
    }
  }

  return absl::NotFoundError("LlmMetadataProto section not found.");
}

absl::StatusOr<uint32_t> GetMaxContextTokens(const std::string& litertlm_path) {
  litert::lm::proto::LlmMetadata llm_metadata;
  ABSL_RETURN_IF_ERROR(ReadAnyLlmMetadata(litertlm_path, &llm_metadata));
  return llm_metadata.max_num_tokens();
}

}  // namespace litert::lm::schema::capabilities
