#include "camera.h"
#include "QDebug"
#include "QtMath"

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
    if (event->key() == Qt::Key_E) {
        position -= cameraSpeed * up;
    } else if (event->key() == Qt::Key_A) {
        position += cameraSpeed * up;
    } else if (event->key() == Qt::Key_D) {
        QVector3D right = QVector3D::crossProduct(front,up).normalized();
        position += cameraSpeed * right;
    } else if (event->key() == Qt::Key_Q) {
        QVector3D right = QVector3D::crossProduct(front,up).normalized();
        position -= cameraSpeed * right;
    } else if (event->key() == Qt::Key_Z) {
        position += cameraSpeed * front;
    } else if (event->key() == Qt::Key_S) {
        position -= cameraSpeed * front;
    }
}
