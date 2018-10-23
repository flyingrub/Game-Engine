#ifndef TERRAIN_H
#define TERRAIN_H

#include "geometry.h"
#include <QImage>
#include <optional>

class Terrain : public Geometry
{
public:
    Terrain();
    Terrain(int width, int height);
    Terrain(QString heightmap, int with, int height);
private:
    virtual void createGeometry();
    int width=0;
    int height=0;
    std::optional<QImage> heightmap;
};

#endif // TERRAIN_H
