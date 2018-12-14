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
    bool initialized;

    BoundingBox()
        : min(0,0,0)
        , max(0,0,0)
        , initialized(false)
    {}

    void expand(const QVector3D & point)
    {
        if(!initialized)
        {
            initialized = true;
            min = point;
            max = point;
            return;
        }

        min.setX(std::min(point.x(), min.x()));
        min.setY(std::min(point.y(), min.y()));
        min.setZ(std::min(point.z(), min.z()));

        max.setX(std::max(point.x(), max.x()));
        max.setY(std::max(point.y(), max.y()));
        max.setZ(std::max(point.z(), max.z()));
    }

    bool inView() {
        if (min.x() > 1.0f) {
            return false;
        } else if (min.y() > 1.0f) {
            return false;
        } else if (max.x() < -1.0f) {
            return false;
        } else if (max.y() < - 1.0f) {
            return false;
        }
        return true;
    }

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
    BoundingBox getBoundingBox() const;    
    BoundingBox getScreenSpaceBoundingBox(QMatrix4x4 matrix) const;
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
