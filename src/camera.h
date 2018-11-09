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
    void handleInput(QKeyEvent *event);
private:
    QVector3D position = {-5,0,1};
    QVector3D front = {1,0,0};
    QVector3D up = {0,0,1};
    float yaw = 0;
    float pitch = 0;
    float cameraSpeed = 0.2f;
};

#endif // CAMERA_H
