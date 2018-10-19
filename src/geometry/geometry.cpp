#include "geometry.h"

Geometry::Geometry()
{
    createGeometry();
}

Geometry::~Geometry() {

}

void Geometry::calcBoundingBox()
{
    boundingBox.min = vertexBuffer[0].position;
    boundingBox.max = vertexBuffer[0].position;
    for (auto const& p : vertexBuffer) {
        if (p.position.x() < boundingBox.min.x()) {
            boundingBox.min.setX(p.position.x());
        }
        if (p.position.y() < boundingBox.min.y()) {
            boundingBox.min.setY(p.position.y());
        }
        if (p.position.z() < boundingBox.min.z()) {
            boundingBox.min.setZ(p.position.z());
        }
        if (p.position.x() > boundingBox.max.x()) {
            boundingBox.max.setX(p.position.x());
        }
        if (p.position.y() > boundingBox.max.y()) {
            boundingBox.max.setY(p.position.y());
        }
        if (p.position.z() > boundingBox.max.z()) {
            boundingBox.max.setZ(p.position.z());
        }
    }
}
