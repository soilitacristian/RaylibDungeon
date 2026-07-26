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

    bool IsSolid(int tileX, int tileY) const;
    Vector2 FindSpawnPoint() const;

  private:
    Tilemap tilemap;
    std::vector<TileDraw> drawList;
    std::vector<uint8_t> solid;

    Tilemap LoadTilemap(const std::string &tilemapPath);
    TilesetInfo LoadTilesetInfo(const std::string &tsjPath, int firstGid, const std::string &baseDir);
    const TilesetInfo *FindTileset(int gid) const;
    Rectangle SourceRectFor(const TilesetInfo &ts, int gid) const;
    void BuildDrawList();
    void BuildCollision();
};
