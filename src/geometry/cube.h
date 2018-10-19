#ifndef CUBE_H
#define CUBE_H

#include "geometry.h"

class Cube : public Geometry
{
public:
    Cube();
private:
    virtual void createGeometry();
};

#endif // CUBE_H
