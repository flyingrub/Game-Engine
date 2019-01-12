#ifndef LIGHT_H
#define LIGHT_H
#include <qvector3d.h>
#include <qopenglshaderprogram.h>
#include "scene.h"
#include "geometry/sphere.h"
using namespace std;

struct Light {
    QMatrix4x4 matrix;
    std::optional<Scene*> followScene;
    QVector3D position = {0,0,3};
    QVector3D color = {1,0,0};
    bool isDir = false;
    bool isActive = false;

    float constant = 1;
    float linear = 0;
    float quadratic = 0.1f;
    float power = 0.5;


    Light(){}

    Light(QVector3D position, QVector3D color) : position(position), color(color), isActive(true) {}

    void follow(Scene* s) {
        followScene = s;
    }

    void addTo(QOpenGLShaderProgram *p, string base)
    {
        if (followScene) {
            matrix = followScene.value()->getGlobalMatrix();
        } else {
            matrix = QMatrix4x4();
        }
        p->setUniformValue((base + "color").c_str(), color * power);
        p->setUniformValue((base + "position").c_str(), matrix * position);
        p->setUniformValue((base + "isDir").c_str(), isDir);
        p->setUniformValue((base + "isActive").c_str(), isActive);
        p->setUniformValue((base + "constant").c_str(), constant);
        p->setUniformValue((base + "linear").c_str(), linear);
        p->setUniformValue((base + "quadratic").c_str(), quadratic);
    }

    bool collide(BoundingBox b) {
        return BoundingBox::fromPoint(position, 2).collide(b);
    }

    void increasePower() {
        linear = 0.09;
        quadratic = 0.032;
        power = 1;
    }

    void resetPower() {
        linear = 0;
        quadratic = 0.1f;
        power = 0.5;
    }

    void debug(QOpenGLShaderProgram *p) {
        if (!isActive || isDir) return;
        Scene scene;
        p->setUniformValue("currentLightColor", color);
        scene.translate(matrix * position);
        scene.updateGlobalMatrix();
        scene.setGeometry(make_shared<Sphere>(0.05));
        scene.draw(p);
    }

};

struct Lights {
    Light* lights;
    int size;

    Lights(int size) {
        this->size = size;
        lights = new Light[size];
    }

    void toProgram(QOpenGLShaderProgram *program) {
        string base = "lights[0].";
        for (int i =0; i<size; i++) {
            base[base.size()-3] = '0' + (char) i;
            lights[i].addTo(program, base);
        }
    }
    void display(QOpenGLShaderProgram *p) {
        for (size_t i = 0; i<size; i++) {
            lights[i].debug(p);
        }
    }
};

#endif // LIGHT_H
