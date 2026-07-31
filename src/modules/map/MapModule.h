#pragma once
#include "../GameModule.h"
#include "MapDrawingData.h"
#include <cstdint>

class MapModule : public GameModule {
public:
    MapModule();
    ~MapModule() override;
    void Start() override;
    void Update() override;
    void Draw() override;
    void DrawSorted(const std::vector<TileDraw> &externalSortables = {});

    bool IsSolid(int tileX, int tileY) const;
    bool CollidesWithRect(const Rectangle &box) const;
    void DrawDebugCollisions() const;

    bool IsDebugCollisionsEnabled() const {
        return debugCollisions;
    }

    Vector2 FindSpawnPoint() const;

private:
    Tilemap tilemap;
    std::vector<TileDraw> staticDrawList;
    std::vector<TileDraw> sortableDrawList;
    std::vector<TileDraw> ySortedDrawList;
    std::vector<uint8_t> solid;
    std::vector<Rectangle> mapCollisions;
    bool debugCollisions = true;

    Tilemap LoadTilemap(const std::string &tilemapPath);
    TilesetInfo LoadTilesetInfo(const std::string &tsjPath, int firstGid, const std::string &baseDir);
    const TilesetInfo *FindTileset(int gid) const;
    Rectangle SourceRectForGid(const TilesetInfo &ts, int gid) const;
    void BuildDrawList();
    void BuildCollision();
};
