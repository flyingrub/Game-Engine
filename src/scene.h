#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <optional>
#include "geometry/geometry.h"
#include <memory>

class Scene
{
public:
    Scene();
    virtual ~Scene();
    void addChild(Scene* s);
    std::optional<Scene *> getParent();
    void setGeometry(std::shared_ptr<Geometry>);
    QMatrix4x4 getLocalMatrix();
    void rotate(float angle, const QVector3D &vector);
    void scale(const QVector3D &vector);
    void translate(const QVector3D &vector);
    void updateMatrix(QMatrix4x4 newLocalMatrix);
    void draw(QOpenGLShaderProgram* program);
private:
    void updateGlobalMatrix();
    QMatrix4x4 localMatrix;
    QMatrix4x4 globalMatrix;
    std::optional<Scene*> parent;
    std::vector<Scene*> children;
    std::optional<std::shared_ptr<Geometry>> geometry;
};

#endif // SCENE_H
