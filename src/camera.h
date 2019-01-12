#ifndef CAMERA_H
#define CAMERA_H
#include <QVector3D>
#include <QMatrix4x4>
#include <QMouseEvent>
#include "geometry/boundingbox.h"

class Camera
{
public:
    Camera();
    QMatrix4x4 getMatrix();
    void handleMouseMove(QMouseEvent* e, QVector2D center);
    void handleInput(QKeyEvent *event);
    void update(float time);
    QVector3D getPosition() const;
    BoundingBox getBoundingBox();

private:
    void updateVelocity();

    QVector3D position = {-15,-15,2};
    QVector3D front = {1,1,0};
    QVector3D up = {0,0,1};
    float yaw = 0;
    float pitch = 0;
    float cameraSpeed = 0.5f;
    QVector3D velocity = {0,0,0};

    QSet<int> keysPressed;
};

#endif // CAMERA_H
