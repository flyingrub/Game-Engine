#include "sphere.h"
#include <iostream>
#include "cmath"
const double PI = 3.14159265;


Sphere::Sphere(double radius, int lat, int mer) : radius(radius), lat(lat), mer(mer)
{
    createGeometry();
    bind();
    calcBoundingBox();
}

void Sphere::createGeometry()
{
    for (int j = 0; j <= mer; j++) {
        double theta = 2.0 * PI / mer * j;
        for (int i = 0; i <= lat; i++) {
            double phi = PI / lat * i;
            double x = sin(phi) * radius * cos(theta);
            double y = sin(phi) * radius * sin(theta);
            double z = cos(phi) * radius;
            vertices.push_back({QVector3D(x, y, z), QVector2D(0,0)});
        }
    }

    for (int i = 0; i < mer; i++) {
        for (int j = 0; j < lat; j++) {
            int k = i + 1;
            int l = j + 1;
            int p1 = (lat + 1 ) * i + j;
            int p2 = (lat + 1 ) * k + j;
            int p3 = (lat + 1 ) * i + l;
            int p4 = (lat + 1 ) * k + l;
            int toAdd[] = {p4,p2,p1,p4,p3,p1};
            indices.insert(indices.end(), std::begin(toAdd), std::end(toAdd));
        }
    }
}
