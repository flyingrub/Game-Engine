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
#include <iostream>
#include <QPainter>
#include "geometry/sphere.h"
#include "geometry/cube.h"
#include "geometry/terrain.h"
#include "noise/SimplexNoise.h"

float MainWidget::rotation_speed = 0.5;
MainWidget* MainWidget::singleton;

MainWidget::MainWidget(QWidget *parent, int update_fps) :
    QOpenGLWidget(parent),
    texture(0),
    update_fps(update_fps)
{
    singleton = this;
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
    if (event->key() == Qt::Key_P) {
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    }
    camera.handleInput(event);
}

void MainWidget::keyReleaseEvent(QKeyEvent* event) {
    camera.handleInput(event);
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
    timeElapsed = last_time.msecsTo(new_time);
    last_time = new_time;
    setWindowTitle(QString("Lux | FPS : %1").arg((int) (1000 / timeElapsed)));
    collideCheck();

    if (fmod(rotation_angle, 360) == 0) rotation_angle -= 360;
    rotation_angle = rotation_speed * timeElapsed / 10.0;
    //cubeScene->rotate({0,0,rotation_angle*0.5f});
    update();
}

void MainWidget::collideCheck() {
    for (int i = 0; i<lights.size; i++) {
        if (lights.lights[i].collide(camera.getBoundingBox())) {
            lights.lights[currentType].resetPower();
            currentType = static_cast<Type>(i);
            lights.lights[currentType].increasePower();
            vectorialMode = false;
            return;
        }
    }
    vectorialMode = false;
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

    Scene* cubeScene = new Scene();
    shared_ptr<Geometry> cube = make_shared<Geometry>("geometries/Cube.obj");
    cubeScene->setGeometry(cube);
    cubeScene->translate({0,0,1});
    cubeScene->setType(Type::Red);

    Scene* cubeScene1 = new Scene();
    cubeScene1->setGeometry(cube);
    cubeScene1->translate({0,0,4});
    cubeScene1->setType(Type::Pink);
    cubeScene1->scale({5,2,3});

    Scene* cubeScene2 = new Scene();
    cubeScene2->setGeometry(cube);
    cubeScene2->translate({0,0,5});
    cubeScene2->setType(Type::Blue);
    cubeScene2->scale({3,3,4});

    Scene* cubeScene3 = new Scene();
    cubeScene3->setGeometry(cube);
    cubeScene3->translate({0,0,2});
    cubeScene3->setType(Type::Green);
    cubeScene3->scale({2,2,1});

    terrainScene = new Scene();
    shared_ptr<Geometry> terrain = make_shared<Terrain>(300,300);
    terrainScene->setGeometry(terrain);

    scene.addChild(terrainScene);
    terrainScene->addChild(cubeScene);
    terrainScene->addChild(cubeScene1);
    terrainScene->addChild(cubeScene2);
    terrainScene->addChild(cubeScene3);



    lights.lights[0] = Light({-10,-10,3},{1,0,0});
    lights.lights[1] = Light({10,10,3}, {0,1,0});
    lights.lights[2] = Light({10,-10,3},{0,0,1});
    lights.lights[3] = Light({-10,10,3},{1,0,1});
    lights.lights[0].follow(cubeScene);
    lights.lights[1].follow(cubeScene);
    lights.lights[2].follow(cubeScene);
    lights.lights[3].follow(cubeScene);

    // Use QBasicTimer because its faster than QTimer
    timer.start(1000.0 / update_fps, this);
}


void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();

    // Compile fragment shader
    if (!lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lamp.glsl"))
        close();

    // Link shader pipeline
    if (!lightProgram.link())
        close();

    // SCENE RENDER SHADER
    // Compile vertex shader
    if (!colorLightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();

    // Compile fragment shader
    if (!colorLightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/colorAndLight.glsl"))
        close();

    // Link shader pipeline
    if (!colorLightProgram.link())
        close();

    // NORMAL SHADER
    // Compile vertex shader
    if (!normalColorProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();

    // Compile fragment shader
    if (!normalColorProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/normalColor.glsl"))
        close();

    // Link shader pipeline
    if (!normalColorProgram.link())
        close();

    // OUTLINE SHADER
    // Compile vertex shader
    if (!outlineProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();

    // Compile fragment shader
    if (!outlineProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/outlineShader.glsl"))
        close();

    // Link shader pipeline
    if (!outlineProgram.link())
        close();

    // HDR SHADER (Tonemapping)
    // Compile vertex shader
    if (!hdrToneMappingProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();

    // Compile fragment shader
    if (!hdrToneMappingProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/hdr.glsl"))
        close();

    // Link shader pipeline
    if (!hdrToneMappingProgram.link())
        close();

    // HDR SHADER (Tonemapping)
    // Compile vertex shader
    if (!bloomProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();

    // Compile fragment shader
    if (!bloomProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/bloom.glsl"))
        close();

    // Link shader pipeline
    if (!bloomProgram.link())
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

Camera MainWidget::getCamera() const
{
    return camera;
}

QMatrix4x4 MainWidget::getProjection() const
{
    return projection;
}

void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    const qreal zNear = 0.1, zFar = 1000.0, fov = 60.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL()
{
    camera.update(timeElapsed);
    scene.updateGlobalMatrix();
    glEnable(GL_MULTISAMPLE);
    if (vectorialMode) {
        renderVectorialWithBloom();
    } else {
        renderColorWithBloom();
    }
}

void MainWidget::renderVectorialWithBloom() {
    QOpenGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(GL_RGBA16F);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObjectFormat multisamples;
    multisamples.setInternalTextureFormat(GL_RGBA16F);
    multisamples.setSamples(4);
    multisamples.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject frameNormal = QOpenGLFramebufferObject(size(), format);
    frameNormal.bind();
    renderNormal();
    frameNormal.release();

    QOpenGLFramebufferObject frameHDRmultisamples = QOpenGLFramebufferObject(size(), multisamples);
    QOpenGLFramebufferObject frameHDR = QOpenGLFramebufferObject(size(), format);
    frameHDRmultisamples.bind();
    renderVectorial(&frameNormal);
    frameHDRmultisamples.release();
    QOpenGLFramebufferObject::blitFramebuffer(&frameHDR, &frameHDRmultisamples);
    GLuint textureBloom = this->bloom(frameHDR.texture());

    hdrToneMappingProgram.bind();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureBloom);
    hdrToneMappingProgram.setUniformValue("bloomTexture", 1);
    hdrToneMappingProgram.setUniformValue("exposure", 1);
    renderQuad(&hdrToneMappingProgram, frameHDR.texture());


    glDeleteTextures(1, &textureBloom);
}


void MainWidget::renderColorWithBloom() {
    QOpenGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(GL_RGBA16F);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    //QOpenGLFramebufferObject frameHDRmultisamples = QOpenGLFramebufferObject(size(), multisamples);
    QOpenGLFramebufferObject frameHDR = QOpenGLFramebufferObject(size(), format);

    frameHDR.addColorAttachment(size());
    frameHDR.bind();
    render();
    frameHDR.release();

    GLuint textureHDR = frameHDR.takeTexture(0);
    GLuint textureBright = frameHDR.takeTexture(1);

    GLuint textureBloom = this->bloom(textureBright);

    hdrToneMappingProgram.bind();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureBloom);
    hdrToneMappingProgram.setUniformValue("bloomTexture", 1);
    hdrToneMappingProgram.setUniformValue("exposure", 1);
    renderQuad(&hdrToneMappingProgram, textureHDR);

    GLuint toRemove[3] = {
        textureHDR,
        textureBright,
        textureBloom
    };
    glDeleteTextures(3, toRemove);
}

GLuint MainWidget::bloom(GLuint texture, int amount) {
    QOpenGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(GL_RGBA16F);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject pingPong[2] = {
        QOpenGLFramebufferObject(size(), format),
        QOpenGLFramebufferObject(size(), format)
    };

    bool horizontal = true, first_iteration = true;
    GLuint current_texture = texture;

    for (int i = 0; i < amount; i++) {
        pingPong[horizontal].bind();
        bloomProgram.bind();
        bloomProgram.setUniformValue("horizontal", horizontal);
        renderQuad(&bloomProgram, current_texture);
        horizontal = !horizontal;
        current_texture = pingPong[!horizontal].texture();
    }
    return pingPong[horizontal].takeTexture(0);
}

void MainWidget::renderNormal() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    normalColorProgram.bind();
    normalColorProgram.setUniformValue("view", camera.getMatrix());
    normalColorProgram.setUniformValue("projection", projection);

    scene.draw(&normalColorProgram);
}

void MainWidget::render() {
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    f->glDrawBuffers(2, buffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    colorLightProgram.bind();
    texture->bind();
    colorLightProgram.setUniformValue("texture", 0);
    colorLightProgram.setUniformValue("view", camera.getMatrix());
    colorLightProgram.setUniformValue("projection", projection);
    colorLightProgram.setUniformValue("time", (float) start_time.elapsed() / 1000.0f);

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    lights.toProgram(&colorLightProgram);
    scene.draw(&colorLightProgram);

    lightProgram.bind();
    lightProgram.setUniformValue("view", camera.getMatrix());
    lightProgram.setUniformValue("projection", projection);
    lights.display(&lightProgram);

}

void MainWidget::renderVectorial(QOpenGLFramebufferObject* frameNormal) {
    glClear(GL_COLOR_BUFFER_BIT);
    outlineProgram.bind();
    QVector3D currentColor = lights.lights[static_cast<int>(currentType)].color;
    outlineProgram.setUniformValue("edgeColor", currentColor);
    outlineProgram.setUniformValue("threshold", 0.1f);
    renderQuad(&outlineProgram, frameNormal->texture());

    lightProgram.bind();
    lightProgram.setUniformValue("view", camera.getMatrix());
    lightProgram.setUniformValue("projection", projection);
    lights.display(&lightProgram);
}

void MainWidget::renderQuad(QOpenGLShaderProgram* program, GLuint texture) {
    glDisable(GL_DEPTH_TEST);
    program->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    program->setUniformValue("texture", 0);
    program->setUniformValue("u_resolution", size());

    QOpenGLBuffer quadVerticesBuff;
    quadVerticesBuff.create();
    quadVerticesBuff.bind();
    quadVerticesBuff.allocate(quadVertices, sizeof(quadVertices));

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 4*sizeof(float));

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, 2*sizeof(float), 2, 4*sizeof(float));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
