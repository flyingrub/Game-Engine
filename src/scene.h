#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <optional>
#include "geometry/geometry.h"
#include <memory>
#include "camera.h"

enum Type {Red, Blue, Green, Pink, None};

class Scene
{
public:
    Scene();
    virtual ~Scene();
    void addChild(Scene* s);
    void removeChild(Scene* s);
    std::optional<Scene *> getParent();
    void setGeometry(std::shared_ptr<Geometry>);
    QMatrix4x4 getLocalMatrix();
    void updateGlobalMatrix();
    void rotate(const QVector3D &vector);
    void scale(const QVector3D &vector);
    void translate(const QVector3D &vector);
    void draw(QOpenGLShaderProgram* program);

    bool inView();
    bool shouldDraw();
    QMatrix4x4 getGlobalMatrix() const;
    BoundingBox getGeometryBoundingBox() const;
    bool hasChild(Scene* s) const;
    void setType(Type t);

    bool collide(Camera camera);
    bool getShouldCollide() const;
    void setShouldCollide(bool value);

    bool correctType();
private:
    Type type = Type::None;
    QVector3D m_translation = {0,0,0};
    QVector3D m_rotation = {0,0,0};
    QVector3D m_scale = {1,1,1};
    QMatrix4x4 globalMatrix;
    std::optional<Scene*> parent;
    std::vector<Scene*> children;
    std::optional<std::shared_ptr<Geometry>> geometry;
    bool shouldCollide=true;
};

#endif // SCENE_H
