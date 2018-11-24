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
    QVector3D normal = {1,1,1};

    bool operator<(const VertexData that) const{
            return memcmp((void*)this, (void*)&that, sizeof(VertexData))>0;
    }
};

class Geometry : protected QOpenGLFunctions
{
public:
    Geometry();
    Geometry(QString objFilename);
    virtual ~Geometry();
    void calcBoundingBox();
    void draw(QOpenGLShaderProgram* program);
protected:
    BoundingBox boundingBox;
    vector<VertexData> vertices;
    vector<int> indices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    virtual void createGeometry() {};
    void bind();
private:
    void createGeometryFromObj(QString filename);
    void optimizeIndex();
    bool isFromObj = false;
};

#endif // RENDEROBJECT_H
