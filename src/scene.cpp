#include "scene.h"

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

std::optional<Scene*> Scene::getParent()
{
    return parent;
}

void Scene::setGeometry(std::shared_ptr<Geometry> g)
{
    geometry = g;
}

QMatrix4x4 Scene::getLocalMatrix()
{
    return localMatrix;
}

void Scene::rotate(float angle, const QVector3D &vector)
{
    localMatrix.rotate(angle,vector);
    updateGlobalMatrix();
}

void Scene::scale(const QVector3D &vector)
{
    localMatrix.scale(vector);
    updateGlobalMatrix();
}

void Scene::translate(const QVector3D &vector)
{
    localMatrix.translate(vector);
    updateGlobalMatrix();
}


void Scene::updateMatrix(QMatrix4x4 newLocalMatrix)
{
    localMatrix = newLocalMatrix;
    updateGlobalMatrix();
}

void Scene::updateGlobalMatrix()
{
    if (parent) {
        globalMatrix = localMatrix * parent.value()->globalMatrix;
    } else {
        globalMatrix = localMatrix;
    }
    for (auto const& c : children) {
        c->updateGlobalMatrix();
    }
}

void Scene::draw(QOpenGLShaderProgram* program)
{
    if (geometry) {
        program->setUniformValue("matrix", globalMatrix);
        geometry.value().get()->draw(program);
    }
    for (auto const& c : children) {
        c->draw(program);
    }
}
