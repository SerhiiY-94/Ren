#pragma once

#include <memory>

namespace Ren {
enum eTex2DFormat;
std::unique_ptr<uint8_t[]> ReadTGAFile(const void *data, int &w, int &h, eTex2DFormat &format);

void ReorderTriangleIndices(const uint32_t *indices, uint32_t indices_count, uint32_t vtx_count, uint32_t *out_indices);
}