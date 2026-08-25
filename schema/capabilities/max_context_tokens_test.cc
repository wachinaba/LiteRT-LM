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
#include <sstream>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/status/status.h"  // from @com_google_absl
#include "absl/status/status_matchers.h"  // from @com_google_absl
#include "flatbuffers/buffer.h"  // from @flatbuffers
#include "flatbuffers/flatbuffer_builder.h"  // from @flatbuffers
#include "runtime/proto/llm_metadata.pb.h"
#include "schema/core/litertlm_header.h"
#include "schema/core/litertlm_header_schema_generated.h"

namespace litert::lm::schema::capabilities {
namespace {

using ::absl_testing::IsOkAndHolds;
using ::absl_testing::StatusIs;

// Helper to create a complete LiteRT-LM file in memory with LlmMetadata.
std::string CreateLiteRTLMBinaryStringWithMetadata(
    const litert::lm::proto::LlmMetadata& llm_metadata) {
  std::string serialized_proto = llm_metadata.SerializeAsString();
  size_t proto_size = serialized_proto.size();

  flatbuffers::FlatBufferBuilder builder;

  uint64_t begin_offset = 16 * 1024;
  uint64_t end_offset = begin_offset + proto_size;

  auto section_object =
      CreateSectionObject(builder, 0, begin_offset, end_offset,
                          AnySectionDataType_LlmMetadataProto);
  std::vector<flatbuffers::Offset<SectionObject>> section_objects_vector = {
      section_object};
  auto section_metadata = CreateSectionMetadata(
      builder, builder.CreateVector(section_objects_vector));
  auto root = CreateLiteRTLMMetaData(builder, 0, section_metadata);
  builder.Finish(root);

  size_t flatbuffer_size = builder.GetSize();

  std::ostringstream output_stream(std::ios::binary);

  // 0. Write magic number and versions
  output_stream.write("LITERTLM", 8);
  output_stream.write(reinterpret_cast<const char*>(&LITERTLM_MAJOR_VERSION),
                      sizeof(uint32_t));
  output_stream.write(reinterpret_cast<const char*>(&LITERTLM_MINOR_VERSION),
                      sizeof(uint32_t));
  output_stream.write(reinterpret_cast<const char*>(&LITERTLM_PATCH_VERSION),
                      sizeof(uint32_t));

  // 1. Write padding
  uint32_t padding = 0;
  output_stream.write(reinterpret_cast<const char*>(&padding),
                      sizeof(uint32_t));

  // 2. Write header end offset
  uint64_t header_end_offset = 32 + flatbuffer_size;
  output_stream.write(reinterpret_cast<const char*>(&header_end_offset),
                      sizeof(uint64_t));

  // 3. Write flatbuffer
  output_stream.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                      flatbuffer_size);

  // Pad to begin_offset
  size_t current_pos = 32 + flatbuffer_size;
  if (begin_offset > current_pos) {
    std::string padding_str(begin_offset - current_pos, 0);
    output_stream.write(padding_str.data(), padding_str.size());
  }

  // 4. Write Proto Data
  output_stream.write(serialized_proto.data(), proto_size);

  return output_stream.str();
}

// Helper to create a LiteRT-LM file without LlmMetadata section.
std::string CreateLiteRTLMBinaryStringWithoutMetadata() {
  flatbuffers::FlatBufferBuilder builder;

  auto section_object =
      CreateSectionObject(builder, 0, 0, 100, AnySectionDataType_TFLiteModel);
  std::vector<flatbuffers::Offset<SectionObject>> section_objects_vector = {
      section_object};
  auto section_metadata = CreateSectionMetadata(
      builder, builder.CreateVector(section_objects_vector));
  auto root = CreateLiteRTLMMetaData(builder, 0, section_metadata);
  builder.Finish(root);

  size_t flatbuffer_size = builder.GetSize();

  std::ostringstream output_stream(std::ios::binary);

  output_stream.write("LITERTLM", 8);
  output_stream.write(reinterpret_cast<const char*>(&LITERTLM_MAJOR_VERSION),
                      sizeof(uint32_t));
  output_stream.write(reinterpret_cast<const char*>(&LITERTLM_MINOR_VERSION),
                      sizeof(uint32_t));
  output_stream.write(reinterpret_cast<const char*>(&LITERTLM_PATCH_VERSION),
                      sizeof(uint32_t));
  uint32_t padding = 0;
  output_stream.write(reinterpret_cast<const char*>(&padding),
                      sizeof(uint32_t));
  uint64_t header_end_offset = 32 + flatbuffer_size;
  output_stream.write(reinterpret_cast<const char*>(&header_end_offset),
                      sizeof(uint64_t));
  output_stream.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
                      flatbuffer_size);

  return output_stream.str();
}

TEST(MaxContextTokensTest, GetMaxContextTokens_Valid_ReturnsCorrectValue) {
  litert::lm::proto::LlmMetadata metadata;
  metadata.set_max_num_tokens(4096);

  std::string file_data = CreateLiteRTLMBinaryStringWithMetadata(metadata);
  std::istringstream stream(file_data, std::ios::binary);

  EXPECT_THAT(GetMaxContextTokens(stream), IsOkAndHolds(4096));
}

TEST(MaxContextTokensTest,
     GetMaxContextTokens_NoMetadataSection_ReturnsNotFound) {
  std::string file_data = CreateLiteRTLMBinaryStringWithoutMetadata();
  std::istringstream stream(file_data, std::ios::binary);

  EXPECT_THAT(GetMaxContextTokens(stream),
              StatusIs(absl::StatusCode::kNotFound));
}

TEST(MaxContextTokensTest, GetMaxContextTokens_InvalidStream_ReturnsError) {
  std::istringstream stream("invalid_data");
  EXPECT_THAT(GetMaxContextTokens(stream),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

}  // namespace
}  // namespace litert::lm::schema::capabilities
