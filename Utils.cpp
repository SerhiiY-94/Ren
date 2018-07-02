#include "Utils.h"

#include <deque>

#include "Texture.h"

std::unique_ptr<uint8_t[]> ren::ReadTGAFile(const void *data, int &w, int &h, eTex2DFormat &format) {
    uint8_t tga_header[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    const uint8_t *tga_compare = (const uint8_t *)data;
    const uint8_t *img_header = (const uint8_t *)data + sizeof(tga_header);
    uint32_t img_size;
    bool compressed = false;

    if (memcmp(tga_header, tga_compare, sizeof(tga_header)) != 0) {
        if (tga_compare[2] == 1) {
            fprintf(stderr, "Image cannot be indexed color.");
        }
        if (tga_compare[2] == 3) {
            fprintf(stderr, "Image cannot be greyscale color.");
        }
        if (tga_compare[2] == 9 || tga_compare[2] == 10) {
            //fprintf(stderr, "Image cannot be compressed.");
            compressed = true;
        }
    }

    w = img_header[1] * 256u + img_header[0];
    h = img_header[3] * 256u + img_header[2];

    if (w <= 0 || h <= 0 ||
            (img_header[4] != 24 && img_header[4] != 32)) {
        if (w <= 0 || h <= 0) {
            fprintf(stderr, "Image must have a width and height greater than 0");
        }
        if (img_header[4] != 24 && img_header[4] != 32) {
            fprintf(stderr, "Image must be 24 or 32 bit");
        }
        return nullptr;
    }

    uint32_t bpp = img_header[4];
    uint32_t bytes_per_pixel = bpp / 8;
    img_size = w * h * bytes_per_pixel;
    const uint8_t *image_data = (const uint8_t *)data + 18;

    std::unique_ptr<uint8_t[]> image_ret(new uint8_t[img_size]);

    if (!compressed) {
        for (unsigned i = 0; i < img_size; i += bytes_per_pixel) {
            image_ret[i] = image_data[i + 2];
            image_ret[i + 1] = image_data[i + 1];
            image_ret[i + 2] = image_data[i];
            if (bytes_per_pixel == 4) {
                image_ret[i + 3] = image_data[i + 3];
            }
        }
    } else {
        for (unsigned num = 0; num < img_size;) {
            uint8_t packet_header = *image_data++;
            if (packet_header & (1 << 7)) {
                uint8_t color[4];
                unsigned size = (packet_header & ~(1 << 7)) + 1;
                size *= bytes_per_pixel;
                for (unsigned i = 0; i < bytes_per_pixel; i++) {
                    color[i] = *image_data++;
                }
                for (unsigned i = 0; i < size; i += bytes_per_pixel, num += bytes_per_pixel) {
                    image_ret[num] = color[2];
                    image_ret[num + 1] = color[1];
                    image_ret[num + 2] = color[0];
                    if (bytes_per_pixel == 4) {
                        image_ret[num + 3] = color[3];
                    }
                }
            } else {
                unsigned size = (packet_header & ~(1 << 7)) + 1;
                size *= bytes_per_pixel;
                for (unsigned i = 0; i < size; i += bytes_per_pixel, num += bytes_per_pixel) {
                    image_ret[num] = image_data[i + 2];
                    image_ret[num + 1] = image_data[i + 1];
                    image_ret[num + 2] = image_data[i];
                    if (bytes_per_pixel == 4) {
                        image_ret[num + 3] = image_data[i + 3];
                    }
                }
                image_data += size;
            }
        }
    }

    if (bpp == 32) {
        format = RawRGBA8888;
    } else if (bpp == 24) {
        format = RawRGB888;
    }

    return image_ret;
}

void ren::ReorderTriangleIndices(const uint32_t *indices, uint32_t indices_count, uint32_t vtx_count, uint32_t *out_indices) {
    // From https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html

    uint32_t prim_count = indices_count / 3;

    struct vtx_data_t {
        int32_t cache_pos = -1;
        float score = 0.0f;
        uint32_t ref_count = 0;
        uint32_t active_tris_count = 0;
        std::unique_ptr<int32_t[]> tris;
    };

    auto get_vertex_score = [](const vtx_data_t &v) -> float {
        const float CacheDecayPower = 1.5f;
        const float LastTriScore = 0.75f;
        const float ValenceBoostScale = 2.0f;
        const float ValenceBoostPower = 0.5f;

        const int MaxSizeVertexCache = 32;

        if (v.active_tris_count == 0) {
            // No tri needs this vertex!
            return -1.0f;
        }

        float score = 0.0f;

        if (v.cache_pos < 0) {
            // Vertex is not in FIFO cache - no score.
        } else if (v.cache_pos < 3) {
            // This vertex was used in the last triangle,
            // so it has a fixed score, whichever of the three
            // it's in. Otherwise, you can get very different
            // answers depending on whether you add
            // the triangle 1,2,3 or 3,1,2 - which is silly.
            score = LastTriScore;
        } else {
            assert(v.cache_pos < MaxSizeVertexCache);
            // Points for being high in the cache.
            const float scaler = 1.0f / (MaxSizeVertexCache - 3);
            score = 1.0f - (v.cache_pos - 3) * scaler;
            score = std::pow(score, CacheDecayPower);
        }

        // Bonus points for having a low number of tris still to
        // use the vert, so we get rid of lone verts quickly.

        float ValenceBoost = std::pow((float)v.active_tris_count, -ValenceBoostPower);
        score += ValenceBoostScale * ValenceBoost;
        return score;
    };

    struct tri_data_t {
        bool is_in_list = false;
        float score = 0.0f;
        uint32_t indices[3] = {};
    };

    std::vector<vtx_data_t> vertices(vtx_count);
    std::vector<tri_data_t> triangles(prim_count);

    for (uint32_t i = 0; i < indices_count; i += 3) {
        tri_data_t &tri = triangles[i/3];

        tri.indices[0] = indices[i + 0];
        tri.indices[1] = indices[i + 1];
        tri.indices[2] = indices[i + 2];

        vertices[indices[i + 0]].active_tris_count++;
        vertices[indices[i + 1]].active_tris_count++;
        vertices[indices[i + 2]].active_tris_count++;
    }

    for (auto &v : vertices) {
        v.tris.reset(new int32_t[v.active_tris_count]);
        v.score = get_vertex_score(v);
    }

    int32_t next_best_index = -1, next_next_best_index = -1;
    float next_best_score = -1.0f, next_next_best_score = -1.0f;

    for (uint32_t i = 0; i < indices_count; i += 3) {
        tri_data_t &tri = triangles[i / 3];

        auto &v0 = vertices[indices[i + 0]];
        auto &v1 = vertices[indices[i + 1]];
        auto &v2 = vertices[indices[i + 2]];

        v0.tris[v0.ref_count++] = i / 3;
        v1.tris[v1.ref_count++] = i / 3;
        v2.tris[v2.ref_count++] = i / 3;

        tri.score = v0.score + v1.score + v2.score;

        if (tri.score > next_best_score) {
            if (tri.score > next_next_best_score) {
                next_next_best_index = i / 3;
                next_next_best_score = tri.score;
            }
            next_best_index = i / 3;
            next_best_score = tri.score;
        }
    }

    std::deque<uint32_t> lru_cache;

    auto use_vertex = [](std::deque<uint32_t> &lru_cache, uint32_t vtx_index) {
        auto it = std::find(std::begin(lru_cache), std::end(lru_cache), vtx_index);

        if (it == std::end(lru_cache)) {
            lru_cache.push_back(vtx_index);
        }

        if (it != std::begin(lru_cache)) {
            lru_cache.erase(it);
            lru_cache.push_front(vtx_index);
        }
    };

    for (int32_t out_index = 0; out_index < indices_count; ) {
        if (next_best_index < 0) {
            next_best_score = next_next_best_score = -1.0f;
            next_best_index = next_next_best_index = -1;

            for (int32_t i = 0; i < prim_count; i++) {
                auto &tri = triangles[i];

                if (!tri.is_in_list) {
                    if (tri.score > next_best_score) {
                        if (tri.score > next_next_best_score) {
                            next_next_best_index = i / 3;
                            next_next_best_score = tri.score;
                        }
                        next_best_index = i / 3;
                        next_best_score = tri.score;
                    }
                }

                auto &next_best_tri = triangles[next_best_index];

                for (int32_t j = 0; j < 3; j++) {
                    out_indices[out_index++] = next_best_tri.indices[j];

                    auto &v = vertices[next_best_tri.indices[j]];
                    v.active_tris_count--;
                    for (uint32_t k = 0; k < v.ref_count; k++) {
                        if (v.tris[k] == next_best_index) {
                            v.tris[k] = -1;
                            break;
                        }
                    }

                    use_vertex(lru_cache, next_best_tri.indices[j]);
                }

                next_best_tri.is_in_list = true;
            }
        }
    }
}