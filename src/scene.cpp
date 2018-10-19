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

Scene* Scene::getParent()
{
    return parent;
}

void Scene::setGeometry(std::shared_ptr<Geometry> g)
{
    geometry = g;
}

void Scene::updateLocalMatrix(QMatrix4x4 newLocalMatrix)
{
    localMatrix = newLocalMatrix;
    updateGlobalMatrix();
}

void Scene::updateGlobalMatrix()
{
    globalMatrix = localMatrix * parent->globalMatrix;
    for (auto const& c : children) {
        c->updateGlobalMatrix();
    }
}

void Scene::draw(QOpenGL)
{
    if (geometry) {
        geometry.value().get()->draw();
    }
}
