#ifndef SPHERE_H
#define SPHERE_H
#include "geometry.h"


class Sphere : public Geometry
{
public:
    Sphere() : Sphere(1) {}
    Sphere(int radius) : Sphere(radius, 20,20) {}
    Sphere(int radius, int lat, int mer);
private:
    int radius;
    int lat;
    int mer;
    virtual void createGeometry();
};

#endif // SPHERE_H
