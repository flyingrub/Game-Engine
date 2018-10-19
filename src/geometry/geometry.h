#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

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

class Geometry : protected QOpenGLFunctions
{
public:
    Geometry();
    virtual ~Geometry();
    void calcBoundingBox();
    void draw(QOpenGLShaderProgram* program);
protected:
    BoundingBox boundingBox;
    vector<VertexData> vertices;
    vector<int> indices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    virtual void createGeometry() = 0;
    void bind();
};

#endif // RENDEROBJECT_H
