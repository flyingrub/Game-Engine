#include "camera.h"
#include "QDebug"
#include "QtMath"
#include <iostream>

Camera::Camera()
{
}

QMatrix4x4 Camera::getMatrix()
{
    QMatrix4x4 m;
    m.lookAt(position, position + front, up);
    return m;
}

void Camera::handleMouseMove(QMouseEvent *event, QVector2D center)
{
    QVector2D diff = center - QVector2D(event->localPos());
    yaw += diff.x()/10;
    pitch += diff.y()/10;
    if(pitch > 89.0f)
      pitch =  89.0f;
    if(pitch < -89.0f)
      pitch = -89.0f;
    front.setX(qCos(qDegreesToRadians(pitch)) * qCos(qDegreesToRadians(yaw)));
    front.setZ(qSin(qDegreesToRadians(pitch)));
    front.setY(qCos(qDegreesToRadians(pitch)) * qSin(qDegreesToRadians(yaw)));
    front.normalize();
}

void Camera::handleInput(QKeyEvent *event)
{
    if (event->type() == QEvent::Type::KeyPress) {
        keysPressed.insert(event->key());
    } else {
        keysPressed.remove(event->key());
    }
}

void Camera::updateVelocity() {
    if (keysPressed.contains(Qt::Key_E)) {
        velocity -= cameraSpeed * up;
    }
    if (keysPressed.contains(Qt::Key_A)) {
        velocity += cameraSpeed * up;
    }
    if (keysPressed.contains(Qt::Key_D)) {
        QVector3D right = QVector3D::crossProduct(front,up).normalized();
        velocity += cameraSpeed * right;
    }
    if (keysPressed.contains(Qt::Key_Q)) {
        QVector3D right = QVector3D::crossProduct(front,up).normalized();
        velocity -= cameraSpeed * right;
    }
    if (keysPressed.contains(Qt::Key_Z)) {
        QVector3D f (front.x(), front.y(), 0);
        velocity += cameraSpeed * f.normalized();
    }
    if (keysPressed.contains(Qt::Key_S)) {
        QVector3D f (front.x(), front.y(), 0);
        velocity -= cameraSpeed * f.normalized();
    }
}

void Camera::update(float time)
{
    updateVelocity();
    if (velocity.length() < 0.001) {
        velocity = {0,0,0};
    }
    position += velocity * time / 1000;
    velocity *= 0.90;
}

QVector3D Camera::getPosition() const
{
    return position;
}

BoundingBox Camera::getBoundingBox()
{
    return BoundingBox::fromPoint(position, {0.5,0.5,1});
}
