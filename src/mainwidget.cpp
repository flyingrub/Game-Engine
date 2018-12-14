/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>
#include <cmath>
#include <QPainter>
#include "geometry/cube.h"
#include "geometry/terrain.h"

float MainWidget::rotation_speed = 0.05;

struct Light {
    QVector3D position;
    QVector3D color;

    float constant;
    float linear;
    float quadratic;
};

struct Lights {
    Light* lights;
    int size;
    Lights(int size) {
        this->size = size;
        lights = new Light[size];
    }
    void toProgram(QOpenGLShaderProgram *program) {
        string base= "pointLights[0].";
        for (int i =0; i<size; i++) {
            base[base.size()-3] += (char)i;
            program->setUniformValue((base + "color").c_str(), lights[i].color);
            program->setUniformValue((base + "position").c_str(), lights[i].position);
            program->setUniformValue((base + "constant").c_str(), lights[i].constant);
            program->setUniformValue((base + "linear").c_str(), lights[i].linear);
            program->setUniformValue((base + "quadratic").c_str(), lights[i].quadratic);
        }
    }
};


MainWidget::MainWidget(QWidget *parent, int update_fps) :
    QOpenGLWidget(parent),
    texture(0),
    update_fps(update_fps)
{
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    doneCurrent();
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Up) {
        rotation_speed += 0.001;
    } else if (event->key() == Qt::Key_Down) {
        rotation_speed -= 0.001;
    }
    camera.handleInput(event);
    update();
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
}

void MainWidget::mouseMoveEvent(QMouseEvent* event) {

    QVector2D center(width() / 2, height() / 2);
    camera.handleMouseMove(event,  center);
    QCursor c = cursor();
    c.setPos(mapToGlobal(center.toPoint()));
    c.setShape(Qt::BlankCursor);
    setCursor(c);
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void MainWidget::timerEvent(QTimerEvent *event)
{
    QTime new_time = QTime::currentTime();
    float timeElapsed = last_time.msecsTo(new_time);
    last_time = new_time;

    if (fmod(rotation_angle, 360) == 0) rotation_angle -= 360;
    rotation_angle = rotation_speed * timeElapsed / 10.0;
//    cubeScene->rotate(rotation_angle, {0,0,1});
    cubeScene->rotate({0,0,rotation_angle*0.5f});
    update();
}

void MainWidget::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    cubeScene = new Scene();
    shared_ptr<Geometry> cube = make_shared<Geometry>("geometries/Cube.obj");
    cubeScene->setGeometry(cube);
    cubeScene->translate({-1,1,1});

    terrainScene = new Scene();
    shared_ptr<Geometry> terrain = make_shared<Terrain>();
    terrainScene->setGeometry(terrain);

    Scene* wallS = new Scene();
    shared_ptr<Geometry> wall = make_shared<Geometry>("geometries/Cube.obj");
    wallS->setGeometry(wall);
    wallS->translate({2,2,2});

    Scene* stairScene = new Scene();
    shared_ptr<Geometry> stairs = make_shared<Geometry>("geometries/Cube.obj");
    stairScene->setGeometry(stairs);
    stairScene->translate({1,1,1});

    scene.addChild(terrainScene);
    terrainScene->addChild(cubeScene);
    terrainScene->addChild(wallS);
    cubeScene->addChild(stairScene);


    // Use QBasicTimer because its faster than QTimer
    timer.start(1000.0 / update_fps, this);
}

void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!colorLightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();

    // Compile fragment shader
    if (!colorLightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/colorAndLight.glsl"))
        close();

    // Link shader pipeline
    if (!colorLightProgram.link())
        close();

    // Compile vertex shader
    if (!normalColorProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();

    // Compile fragment shader
    if (!normalColorProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/normalColor.glsl"))
        close();

    // Link shader pipeline
    if (!normalColorProgram.link())
        close();

    // Compile vertex shader
    if (!outlineProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();

    // Compile fragment shader
    if (!outlineProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/outlineShader.glsl"))
        close();

    // Link shader pipeline
    if (!outlineProgram.link())
        close();

}
//! [3]

//! [4]
void MainWidget::initTextures()
{
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/textures/test.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}
//! [4]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    const qreal zNear = 0.1, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    scene.updateGlobalMatrix();

    glEnable(GL_MULTISAMPLE);
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Normal Rendering
    QOpenGLFramebufferObject frameNormal = QOpenGLFramebufferObject(size(), format);
    glEnable(GL_MULTISAMPLE);
    frameNormal.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    normalColorProgram.bind();
    normalColorProgram.setUniformValue("view", camera.getMatrix());
    normalColorProgram.setUniformValue("projection", projection);

    scene.draw(&colorLightProgram);
    frameNormal.release();


    // Light Render
//    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    colorLightProgram.bind();
    texture->bind();
    colorLightProgram.setUniformValue("texture", 0);
    colorLightProgram.setUniformValue("view", camera.getMatrix());
    colorLightProgram.setUniformValue("projection", projection);
    colorLightProgram.setUniformValue("time", (float) start_time.elapsed() / 1000.0f);
    colorLightProgram.setUniformValue("dirLight.position", QVector3D{-1,-1,-1});
    colorLightProgram.setUniformValue("dirLight.color", QVector3D{0.5,0.5,0.5});

    Lights lights(2);
    lights.lights[0] = {
        {0,0,6},
        {1,0,0.5},
        1,0.045f,0.0075f
    };
    lights.lights[1] = {
        {6,6,6},
        {0,0,0.5},
        1,0.022f,0.0019
    };
    lights.toProgram(&colorLightProgram);
    scene.draw(&colorLightProgram);

    // Outline Detection
//    glDisable(GL_DEPTH_TEST);
//    outlineProgram.bind();
//    glBindTexture(GL_TEXTURE_2D, frameNormal.texture());
//    outlineProgram.setUniformValue("texture", 0);
//    outlineProgram.setUniformValue("u_resolution", size());

//    QOpenGLBuffer quadVerticesBuff;
//    quadVerticesBuff.create();
//    quadVerticesBuff.bind();
//    quadVerticesBuff.allocate(quadVertices, sizeof(quadVertices));

//    int vertexLocation = outlineProgram.attributeLocation("a_position");
//    outlineProgram.enableAttributeArray(vertexLocation);
//    outlineProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 4*sizeof(float));

//    int texcoordLocation = outlineProgram.attributeLocation("a_texcoord");
//    outlineProgram.enableAttributeArray(texcoordLocation);
//    outlineProgram.setAttributeBuffer(texcoordLocation, GL_FLOAT, 2*sizeof(float), 2, 4*sizeof(float));
////    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
//    glDrawArrays(GL_TRIANGLES, 0, 6);

//    quadVerticesBuff.destroy();
}
