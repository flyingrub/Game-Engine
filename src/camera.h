#ifndef CAMERA_H
#define CAMERA_H
#include <QVector3D>
#include <QMatrix4x4>
#include <QMouseEvent>

class Camera
{
public:
    Camera();
    QMatrix4x4 getMatrix();
    void handleMouseMove(QMouseEvent* e, QVector2D center);
    void translate(QVector3D t);
private:
    QVector3D position = {-5,-5,5};
    QVector3D lookAt = {0,0,5};
    QVector3D up = {0,0,1};
    QMatrix4x4 cameraMatrix;
};

#endif // CAMERA_H
