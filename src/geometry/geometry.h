#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <QVector2D>
#include <QVector3D>
#include <QtDebug>
#include <vector>
using namespace std;

struct BoundingBox {
    QVector3D min;
    QVector3D max;
};

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

class Geometry
{
public:
    Geometry();
    virtual ~Geometry();
    virtual createGeometry();
    void calcBoundingBox();
protected:
    BoundingBox boundingBox;
    vector<VertexData> vertexBuffer;
    vector<int> indexBuffer;
    virtual void createGeometry() = 0;
};

#endif // RENDEROBJECT_H
