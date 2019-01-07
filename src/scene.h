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
    void updateGlobalMatrix();
    void rotate(const QVector3D &vector);
    void scale(const QVector3D &vector);
    void translate(const QVector3D &vector);
    void draw(QOpenGLShaderProgram* program);

    bool inView();
    QMatrix4x4 getGlobalMatrix() const;

private:
    QVector3D m_translation = {0,0,0};
    QVector3D m_rotation = {0,0,0};
    QVector3D m_scale = {1,1,1};
    QMatrix4x4 globalMatrix;
    std::optional<Scene*> parent;
    std::vector<Scene*> children;
    std::optional<std::shared_ptr<Geometry>> geometry;
};

#endif // SCENE_H
