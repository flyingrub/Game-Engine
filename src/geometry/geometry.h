#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "boundingbox.h"
#include <vector>
using namespace std;


struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal = {0,0,0};

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
    BoundingBox getBoundingBox() const;
    BoundingBox getBoundingBox(QMatrix4x4 matrix) const;
protected:
    BoundingBox boundingBox;
    vector<VertexData> vertices;
    vector<int> indices;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    void bind();
    virtual void createGeometry() {}
private:
    void createGeometryFromObj(QString filename);
    void optimizeIndex();
    bool isFromObj = false;
};

#endif // RENDEROBJECT_H
