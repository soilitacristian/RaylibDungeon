#pragma once
#include "raylib.h"
#include <cstdint>
#include <string>
#include <vector>

/*
 * https://doc.mapeditor.org/en/stable/reference/global-tile-ids/
 *
 * A gid is 32 bits. Tiled uses the top 4 for orientation and leaves the bottom 28 for the actual tile id.
 *
 * Example:
 * raw  10100000000000000000000000111101    = 2684354621
 *      \__/
 *      this
 *
 * we only care about the first 4 bits '1010'
 * bit 32   FLIP_HORIZONTAL         0x80000000 = 1
 * bit 31   FLIP_VERTICAL           0x40000000 = 0
 * bit 30   FLIP_DIAGONAL           0x20000000 = 1
 * bit 29   ROTATED_HEXAGONAL_120   0x10000000 = 0
 *
 * TILE_FLIP_HORIZONTAL         0x80000000    1000 0000 0000 ... 0000
 * TILE_FLIP_VERTICAL           0x40000000    0100 0000 0000 ... 0000
 * TILE_FLIP_DIAGONAL           0x20000000    0010 0000 0000 ... 0000
 * ROTATED_HEXAGONAL_120_FLAG   0x10000000    0001 0000 0000 ... 0000
 */
constexpr std::uint32_t TILE_FLIP_HORIZONTAL = 0x80000000;
constexpr std::uint32_t TILE_FLIP_VERTICAL = 0x40000000;
constexpr std::uint32_t TILE_FLIP_DIAGONAL = 0x20000000;
constexpr std::uint32_t ROTATED_HEXAGONAL_120_FLAG = 0x10000000;

/*
 * This mask just means "keep the bits from 1 -> 28, throw away 29 -> 32"
 * Since those contain flags and we only care about the gid, we use this
 * mask to remove those 4 bits.
 *
 * 0xFFFFFFFF   unsigned int    all bits set to 1
 * 0x0FFFFFFF   unsigned int    all bits set to 1 except for 29 -> 32
 *
 * TILE_GID_MASK    0x0FFFFFFF = 0000 1111 1111 ... 1111
 */
constexpr std::uint32_t TILE_GID_MASK = 0x0FFFFFFF;

struct TilesetInfo {
    std::uint32_t firstGid;
    int columns;
    int tileWidth;
    int tileHeight;
    Texture2D texture;
};

/*
 * Flags are still attahed to the data
 */
struct TileLayer {
    std::string name;
    int width, height;
    std::vector<std::uint32_t> data;
    bool isCollision;
};

struct Tilemap {
    int width, height;
    int tileWidth, tileHeight;
    std::vector<TilesetInfo> tilesets;
    std::vector<TileLayer> layers;
};

/*
 * Tile ready to be drawn
 */
struct TileDraw {
    Texture2D texture;
    Rectangle source;
    Rectangle dest;
    Vector2 origin;
    float rotation;
};

struct TileTransform {
    float rotation;
    bool flipX, flipY;
};
