#pragma once

#include <memory>

namespace ren {
enum eTex2DFormat;
std::unique_ptr<uint8_t[]> ReadTGAFile(const void *data, int &w, int &h, eTex2DFormat &format);
}