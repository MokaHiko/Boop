#include "mesh.h"

#include <json.hpp>
#include <lz4.h>

namespace boop
{

  VertexFormat parase_format(const char *f)
  {
    if (strcmp(f, "F32_PNCV") == 0)
      return VertexFormat::F32_PNCV;
    else
      return VertexFormat::Unknown;
  }

  MeshInfo read_mesh_info(AssetFile *file)
  {
    MeshInfo info = {};

    nlohmann::json metadata = nlohmann::json::parse(file->json);

    info.vertex_buffer_size = metadata["vertex_buffer_size"];
    info.index_buffer_size = metadata["index_buffer_size"];
    info.original_file_path = metadata["original_file_path"];

    std::string vertex_format_string = metadata["vertex_format"];
    info.vertex_format = parase_format(vertex_format_string.c_str());

    std::string compression_string = metadata["compression_mode"];
    info.compression_mode = parse_compression(compression_string.c_str());

    return info;
  }

  void unpack_mesh(MeshInfo *info, const char *src_buffer,
                   size_t src_size, char *vertex_buffer,
                   char *index_buffer)
  {
    // Create buffer to hold merged buffer
    std::vector<char> decompressed_buffer;
    decompressed_buffer.resize(info->vertex_buffer_size + info->index_buffer_size);

    LZ4_decompress_safe(src_buffer, decompressed_buffer.data(), static_cast<int>(src_size), static_cast<int>(decompressed_buffer.size()));

    // Copy vertex buffer
    memcpy(vertex_buffer, decompressed_buffer.data(), info->vertex_buffer_size);

    // Copy index buffer
    memcpy(index_buffer,  decompressed_buffer.data() + info->vertex_buffer_size, info->index_buffer_size);
  }

  AssetFile pack_mesh(MeshInfo *info, char *vertex_buffer,
                      char *index_buffer)
  {
    AssetFile file = {};

    file.type[0] = 'M';
    file.type[1] = 'E';
    file.type[2] = 'S';
    file.type[3] = 'H';

    file.version = 1;

    // ~ Meta data
    nlohmann::json metadata;

    metadata["vertex_buffer_size"] = info->vertex_buffer_size;
    metadata["index_buffer_size"] = info->index_buffer_size;
    metadata["original_file_path"] = info->original_file_path;

    if (info->vertex_format == VertexFormat::F32_PNCV)
    {
      metadata["vertex_format"] = "F32_PNCV";
    }
    else
    {
      metadata["vertex_format"] = "Unknown";
    }

    metadata["compression_mode"] = "LZ4";

    file.json = metadata.dump();

    // ~ Binary data

    // Create buffer to copy and store both vertex and index buffer
    std::vector<char> merged_buffer;
    merged_buffer.resize(info->vertex_buffer_size +
                         info->index_buffer_size);

    memcpy(merged_buffer.data(), vertex_buffer, info->vertex_buffer_size);
    memcpy(merged_buffer.data() + info->vertex_buffer_size, index_buffer,
           info->index_buffer_size);

    // Resize file to compression bound
    size_t compression_staging =
        LZ4_compressBound(static_cast<uint32_t>(merged_buffer.size()));
    file.raw_data.resize(compression_staging);

    int compressed_size =
        LZ4_compress_default(merged_buffer.data(), file.raw_data.data(),
                             static_cast<int>(merged_buffer.size()),
                             static_cast<int>(compression_staging));
    file.raw_data.resize(compressed_size);

    return file;
  }
}