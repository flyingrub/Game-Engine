#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <QVector3D>
#include <QMatrix4x4>

struct BoundingBox {
    QVector3D min;
    QVector3D max;
    bool initialized;

    BoundingBox()
        : min(0,0,0)
        , max(0,0,0)
        , initialized(false)
    {}

    BoundingBox static fromPoint(QVector3D pos, float size) {
        return BoundingBox::fromPoint(pos, {size,size,size});
    }

    BoundingBox static fromPoint(QVector3D pos, QVector3D size) {
        BoundingBox b;
        b.expand(pos - size);
        b.expand(pos + size);
        return b;
    }

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

    bool collide(BoundingBox box) {
        return (min.x() <= box.max.x() && max.x() >= box.min.x()) &&
               (min.y() <= box.max.y() && max.y() >= box.min.y()) &&
               (min.z() <= box.max.z() && max.z() >= box.min.z());
    }

};

#endif // BOUNDINGBOX_H
