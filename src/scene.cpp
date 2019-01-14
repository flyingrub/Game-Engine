#include "scene.h"
#include "mainwidget.h"

Scene::Scene(){

}

Scene::~Scene()
{
    for (auto& c : children) {
        delete c;
    }
}

void Scene::addChild(Scene* s)
{
    children.push_back(s);
    s->parent = this;
}

void Scene::removeChild(Scene *s)
{
    for (size_t i = 0; i<children.size(); i++) {
        if (children[i] == s) {
            children.erase(children.begin() + i);
        }
    }
}

std::optional<Scene*> Scene::getParent()
{
    return parent;
}

void Scene::setGeometry(std::shared_ptr<Geometry> g)
{
    geometry = g;
}

void Scene::rotate(const QVector3D &vector)
{
    m_rotation += vector;
}

void Scene::scale(const QVector3D &vector)
{
   m_scale += vector;
}

void Scene::translate(const QVector3D &vector)
{
    m_translation += vector;
}


QMatrix4x4 Scene::getLocalMatrix()
{
    QMatrix4x4 localMatrix;
    localMatrix.translate(m_translation);
    localMatrix.rotate(m_rotation.x(), {1,0,0});
    localMatrix.rotate(m_rotation.y(), {0,1,0});
    localMatrix.rotate(m_rotation.z(), {0,0,1});
    localMatrix.scale(m_scale);
    return localMatrix;
}

void Scene::updateGlobalMatrix()
{
    QMatrix4x4 localMatrix = getLocalMatrix();
    if (parent) {
        globalMatrix = parent.value()->globalMatrix * localMatrix;
    } else {
        globalMatrix = localMatrix;
    }
    for (auto const& c : children) {
        c->updateGlobalMatrix();
    }
}

bool Scene::inView() {
    MainWidget* main = MainWidget::singleton;
    QMatrix4x4 mvp =
            main->getProjection() *
            main->getCamera().getMatrix() *
            globalMatrix;
    BoundingBox b = geometry.value().get()->getBoundingBox(mvp);
    return b.inView();
}

bool Scene::shouldDraw()
{
    bool isGoodType = type == None || MainWidget::singleton->currentType == type;
    return geometry && inView() && isGoodType;
}

QMatrix4x4 Scene::getGlobalMatrix() const
{
    return globalMatrix;
}

BoundingBox Scene::getGeometryBoundingBox() const
{
    return geometry.value().get()->getBoundingBox(globalMatrix);
}

bool Scene::collide(Camera camera) {
    if (shouldCollide && geometry && getGeometryBoundingBox().collide(camera.getBoundingBox())) {
        return true;
    }
    bool collide = false;
    for (auto * c : children) {
        if (c->collide(camera)) {
            collide = true;
        }
    }
    return collide;
}

bool Scene::getShouldCollide() const
{
    return shouldCollide;
}

void Scene::setShouldCollide(bool value)
{
    shouldCollide = value;
}

bool Scene::hasChild(Scene *s) const
{
    for (size_t i = 0; i<children.size(); i++) {
        if (children[i] == s) {
            return true;
        }
    }
    return false;
}

void Scene::setType(Type t)
{
    type = t;
}

void Scene::draw(QOpenGLShaderProgram* program)
{
    if (shouldDraw()) {
        program->setUniformValue("model", globalMatrix);
        program->setUniformValue("normal_matrix", globalMatrix.inverted().transposed());
        geometry.value().get()->draw(program);
    }
    for (auto const& c : children) {
        c->draw(program);
    }
}
