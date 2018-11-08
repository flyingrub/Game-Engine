#include "camera.h"
#include "QDebug"
Camera::Camera()
{
    cameraMatrix.lookAt(position,lookAt,up);
}



QMatrix4x4 Camera::getMatrix()
{
    return cameraMatrix;
}

void Camera::handleMouseMove(QMouseEvent *event, QVector2D center)
{
    QVector2D diff = QVector2D(event->localPos()) - center;

    QVector3D left = {1,0,0};
    left = (cameraMatrix * left);
    qDebug() << left;
    cameraMatrix.rotate(diff.x()/10.0, {0,0,1});

    cameraMatrix.rotate(diff.y()/10.0, left.normalized());

//    cameraMatrix.rotate(acc, cross)
}

void Camera::translate(QVector3D t)
{

}
