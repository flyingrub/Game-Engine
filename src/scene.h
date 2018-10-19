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
    Scene* getParent();
    void setGeometry(std::shared_ptr<Geometry>);
    void updateLocalMatrix(QMatrix4x4 newLocalMatrix);
    void updateGlobalMatrix();
    void draw(QOpenGLShaderProgram* program);
private:
    QMatrix4x4 localMatrix;
    QMatrix4x4 globalMatrix;
    Scene* parent;
    std::vector<Scene*> children;
    std::optional<std::shared_ptr<Geometry>> geometry;
};

#endif // SCENE_H
