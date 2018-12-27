#ifndef SPHERE_H
#define SPHERE_H
#include "geometry.h"


class Sphere : public Geometry
{
public:
    Sphere() : Sphere(1.0f) {}
    Sphere(double radius) : Sphere(radius, 20,20) {}
    Sphere(double radius, int lat, int mer);
private:
    double radius;
    int lat;
    int mer;
    virtual void createGeometry();
};

#endif // SPHERE_H
