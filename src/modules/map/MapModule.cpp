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

const TilesetInfo *MapModule::FindTileset(int gid) const {
    const TilesetInfo *best = nullptr;
    for (const auto &tileset : tilemap.tilesets) {
        if (tileset.firstGid <= gid && (best == nullptr || tileset.firstGid > best->firstGid)) {
            best = &tileset;
        }
    }
    return best;
}

Rectangle MapModule::SourceRectFor(const TilesetInfo &ts, int gid) const {
    const int local = gid - ts.firstGid;
    const int col = local % ts.columns;
    const int row = local / ts.columns;
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
                const int gid = layer.data[y * layer.width + x];
                if (gid == 0) {
                    continue;
                }

                const TilesetInfo *ts = FindTileset(gid);
                if (ts == nullptr) {
                    continue;
                }

                const float widthInUnits = static_cast<float>(ts->tileWidth) / static_cast<float>(tilemap.tileWidth);
                const float heightInUnits = static_cast<float>(ts->tileHeight) / static_cast<float>(tilemap.tileHeight);

                // tall tiles (walls_high is 16x32) hang upward out of their grid cell
                drawList.push_back({
                    ts->texture,
                    SourceRectFor(*ts, gid),
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
 * Flattens every collision layer into one grid
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
