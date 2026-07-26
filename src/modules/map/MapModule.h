#pragma once
#include "../GameModule.h"
#include "MapDrawingData.h"

class MapModule : public GameModule {
  public:
    MapModule();
    ~MapModule() override;
    void Start() override;
    void Update() override;
    void Draw() override;

  private:
    MapDrawingData drawingData;
    Rectangle TileSource(const MapDrawingData &data, int i, int j);
    void DrawMap(const MapDrawingData &data);
};
