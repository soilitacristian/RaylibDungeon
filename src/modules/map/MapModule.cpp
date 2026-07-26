#include "MapModule.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

MapModule::MapModule() = default;

MapModule::~MapModule() {
    for (auto &tileset : tilemap.tilesets) {
        UnloadTexture(tileset.texture);
    }
}

TilesetInfo MapModule::LoadTilesetInfo(const std::string &tsjPath, int firstGid, const std::string &baseDir) {
    const std::string path = baseDir + "/" + tsjPath;
    std::ifstream file(path);
    if (!file) {
        TraceLog(LOG_FATAL, "MapModule::LoadTilesetInfo: cannot open tileset %s", path.c_str());
    }

    nlohmann::json json;
    file >> json;

    TilesetInfo tsInfo;                     // example for atlas_floor-16x16
    tsInfo.firstGid = firstGid;             // "firstgid":97,
    tsInfo.columns = json["columns"];       // "columns":7,
    tsInfo.tileWidth = json["tilewidth"];   // "tilewidth":16,
    tsInfo.tileHeight = json["tileheight"]; // "tileheight":16,

    const std::string imagePath = json["image"];
    tsInfo.texture = LoadTexture((baseDir + "/" + imagePath).c_str());

    return tsInfo;
}

Tilemap MapModule::LoadTilemap(const std::string &tilemapPath) {
    std::ifstream file(tilemapPath);
    if (!file) {
        TraceLog(LOG_FATAL, "MapModule::LoadTilemap: cannot open map %s", tilemapPath.c_str());
    }

    nlohmann::json json;
    file >> json;

    Tilemap map;                         // example from dungeon.json
    map.width = json["width"];           // "width":30,
    map.height = json["height"];         // "height":20,
    map.tileWidth = json["tilewidth"];   // "tilewidth":16,
    map.tileHeight = json["tileheight"]; // "tileheight":16,

    const std::string baseDir = std::filesystem::path(tilemapPath).parent_path().string();
    for (auto &tilesetRef : json["tilesets"]) {
        map.tilesets.push_back(LoadTilesetInfo(tilesetRef["source"], tilesetRef["firstgid"], baseDir));
    }

    for (auto &layerJson : json["layers"]) {
        TileLayer layer;                                        // example from dungeon.json
        layer.name = layerJson["name"];                         // "name":"walls",
        layer.width = layerJson["width"];                       // "width":30,
        layer.height = layerJson["height"];                     // "height":20,
        layer.data = layerJson["data"].get<std::vector<int>>(); // "data":[2, 75, 75 ... ],
        layer.isCollision = (layer.name == "walls");
        map.layers.push_back(std::move(layer));
    }

    return map;
}

/*
 * Each tileset starts from a specific gid, for example our tileset containing the floors (atlas_floor-16x16)
 * starts at gid = 97, we check what tileset contains that gid in it's range and return that tileset as a match
 *
 * Example:
 * Tracing gid = 184
 *
 * 1. walls_high tileset -> firstGid 1 -> 1 <= 184? Yes. Best is null -> best = walls_high
 * 2. floor tileset -> firstGid 97 -> 97 <= 184? Yes. Is 97 > 1? Yes, so floor starts later than our
 *    current best -> best = floor.
 * 3. walls_low tileset -> firstGid 146. 146 <= 184? Yes, and 146 > 97? Yes -> best = walls_low
 * 4. return walls_low
 */
const TilesetInfo *MapModule::FindTileset(int gid) const {
    const TilesetInfo *best = nullptr;
    for (const auto &tileset : tilemap.tilesets) {
        if (tileset.firstGid <= gid && (best == nullptr || tileset.firstGid > best->firstGid)) {
            best = &tileset;
        }
    }
    return best;
}

/*
 * Creates the source rectangle for a specific tile gid from its' tileset.
 *
 * Example:
 * We want gid = 75 from the atlas_walls_high-16x32 tileset
 * ts.firstGid = 1 up until 96, 97 is the firstGid of another tileset
 *
 * 1. localId of that tile: 75 - 1 = 74
 * 2. col: 74 % 24 = 2
 * 3. row: 74 / 24 = 3.03 to int = 3
 * 4. return a rectangle with the size
 *  {
 *      x = col * tileWidth  = 2 * 16 = 32
 *      y = row * tileHeight = 3 * 32 = 96
 *      width = 16
 *      height = 32
 *  }
 */
Rectangle MapModule::SourceRectForGid(const TilesetInfo &ts, int gid) const {
    const int localId = gid - ts.firstGid;
    const int col = localId % ts.columns;
    const int row = localId / ts.columns;
    return {
        static_cast<float>(col * ts.tileWidth),
        static_cast<float>(row * ts.tileHeight),
        static_cast<float>(ts.tileWidth),
        static_cast<float>(ts.tileHeight),
    };
}

// Resolves every tile once. Layer order is preserved, so draw order is insertion order
void MapModule::BuildDrawList() {
    for (const auto &layer : tilemap.layers) {
        for (int y = 0; y < layer.height; y++) {
            for (int x = 0; x < layer.width; x++) {
                /*
                 * Our layer.data is not a 2d array, it's just a long list of gids for tiles,
                 * so to find which tile gid we need to draw on a 2d grid we can use the formula below.
                 * y -> column
                 * x -> row
                 * layer.width -> how wide / long is "1 row"
                 *
                 * Example:
                 * We want the tile from row 2 col 2, but layer.data is a just a long list
                 *
                 * We calculate: 2 x 30 + 2 = 62, that's the index we have to use in layer.data
                 * to get that specific tile
                 */
                auto dataIndex = y * layer.width + x;
                const int gid = layer.data[dataIndex];

                // Ignore empty tiles
                if (gid == 0) {
                    continue;
                }

                // Using the gid find which tileset contains the tile we got from layer.data
                const TilesetInfo *ts = FindTileset(gid);
                if (ts == nullptr) {
                    continue;
                }

                /*
                 * atlas_floor-16x16 and atlas_walls_low-16x16 use 16x16 so 16 / 16 = 1 world unit
                 * resources/tiled/atlas_walls_high-16x32 uses 16x32 so 32 / 16 = 2 world units
                 */
                const float widthInUnits = static_cast<float>(ts->tileWidth) / static_cast<float>(tilemap.tileWidth);
                const float heightInUnits = static_cast<float>(ts->tileHeight) / static_cast<float>(tilemap.tileHeight);

                // tall tiles (e.g. walls_high) hang upward out of their grid cell
                drawList.push_back({
                    ts->texture,
                    SourceRectForGid(*ts, gid),
                    {
                        static_cast<float>(x),
                        static_cast<float>(y) - (heightInUnits - 1.0f),
                        widthInUnits,
                        heightInUnits,
                    },
                });
            }
        }
    }
}

/*
 * solid is resized and filled with 30 x 20 = 600 bytes, all zero
 *
 * Example:
 * We have the top wall in our json map -> "data":[2, 75, 75, 75, 75, 75 ...]
 * All of these indexes from solid[0 to 29] = 1, meaning that they're solid.
 * Which we're going to use later for collision checking
 */
void MapModule::BuildCollision() {
    solid.assign(static_cast<size_t>(tilemap.width) * tilemap.height, 0);
    for (const auto &layer : tilemap.layers) {
        if (!layer.isCollision) {
            continue;
        }
        for (size_t i = 0; i < layer.data.size(); i++) {
            if (layer.data[i] != 0) {
                solid[i] = 1;
            }
        }
    }
}

void MapModule::Start() {
    tilemap = LoadTilemap("resources/tiled/dungeon.json");
    BuildDrawList();
    BuildCollision();
}

void MapModule::Update() {}

void MapModule::Draw() {
    for (const auto &tile : drawList) {
        DrawTexturePro(tile.texture, tile.source, tile.dest, {0, 0}, 0.0f, WHITE);
    }
}

bool MapModule::IsSolid(int tileX, int tileY) const {
    if (tileX < 0 || tileY < 0 || tileX >= tilemap.width || tileY >= tilemap.height) {
        return true;
    }
    return solid[static_cast<size_t>(tileY) * tilemap.width + tileX] != 0;
}

/*
 * Find the nearest valid tile in the map's center to spawn the player in
 */
Vector2 MapModule::FindSpawnPoint() const {
    const float centerX = tilemap.width * 0.5f;
    const float centerY = tilemap.height * 0.5f;

    Vector2 best = {centerX, centerY};
    float bestDistance = -1.0f;

    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (IsSolid(x, y)) {
                continue;
            }

            const float dx = (x + 0.5f) - centerX;
            const float dy = (y + 0.5f) - centerY;
            const float distance = dx * dx + dy * dy;

            if (bestDistance < 0.0f || distance < bestDistance) {
                bestDistance = distance;
                best = {x + 0.5f, y + 1.0f};
            }
        }
    }

    return best;
}
