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
     setMouseTracking(true);
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    doneCurrent();
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

void MainWidget::initSounds() {
    lightSwitch.setSource(QUrl::fromLocalFile("../cube/sounds/switch.wav"));
    neonLight.setSource(QUrl::fromLocalFile("../cube/sounds/neon.wav"));
    neonRev.setSource(QUrl::fromLocalFile("../cube/sounds/neonrev.wav"));
    humming.setSource(QUrl::fromLocalFile("../cube/sounds/hum.wav"));
    humming.setLoopCount(QSoundEffect::Infinite);
    humming.setVolume(0.6);
    ambiant.setSource(QUrl::fromLocalFile("../cube/sounds/ambiant.wav"));
    ambiant.setLoopCount(QSoundEffect::Infinite);
    ambiant.setVolume(0.4);
    ambiant.play();
}


void MainWidget::initScene() {
    endScene = new Scene();
    endScene->setGeometry(make_shared<Sphere>());
    endScene->setShouldCollide(false);

    Scene* cubeScene = new Scene();
    shared_ptr<Geometry> cube = make_shared<Geometry>("geometries/Cube.obj");
    cubeScene->setGeometry(cube);
    cubeScene->translate({0,0,1});
    cubeScene->setType(Type::Red);

    Scene* cubeScene1 = new Scene();
    cubeScene1->setGeometry(cube);
    cubeScene1->translate({1,1,1});
    cubeScene1->setType(Type::Pink);

    Scene* cubeScene2 = new Scene();
    cubeScene2->setGeometry(cube);
    cubeScene2->translate({1,-1,1});
    cubeScene2->setType(Type::Blue);

    Scene* cubeScene3 = new Scene();
    cubeScene3->setGeometry(cube);
    cubeScene3->translate({-1,-1,1});
    cubeScene3->setType(Type::Pink);

    terrainScene = new Scene();
    shared_ptr<Geometry> terrain = make_shared<Terrain>(300,300);
    terrainScene->setGeometry(terrain);


    scene.addChild(terrainScene);
    terrainScene->addChild(cubeScene);
    terrainScene->addChild(cubeScene1);
    terrainScene->addChild(cubeScene2);
    terrainScene->addChild(cubeScene3);
}

void MainWidget::initLights() {
    lights.lights[0] = Light({-10,-10,3},{1,0,0});
    lights.lights[1] = Light({10,10,3}, {0,1,0});
    lights.lights[2] = Light({10,-10,3},{0,0,1});
    lights.lights[3] = Light({-10,10,3},{1,0,1});
}

void MainWidget::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();
    initScene();
    initSounds();
    initLights();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    timer.start(1000.0 / update_fps, this);
}


void MainWidget::initShaders()
{
    if (!lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();
    if (!lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lamp.glsl"))
        close();
    if (!lightProgram.link())
        close();
    if (!colorLightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();
    if (!colorLightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/colorAndLight.glsl"))
        close();
    if (!colorLightProgram.link())
        close();
    if (!normalColorProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera.glsl"))
        close();
    if (!normalColorProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/normalColor.glsl"))
        close();
    if (!normalColorProgram.link())
        close();
    if (!outlineProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();
    if (!outlineProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/outlineShader.glsl"))
        close();
    if (!outlineProgram.link())
        close();
    if (!hdrToneMappingProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();
    if (!hdrToneMappingProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/hdr.glsl"))
        close();
    if (!hdrToneMappingProgram.link())
        close();
    if (!bloomProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/screen.glsl"))
        close();
    if (!bloomProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/bloom.glsl"))
        close();
    if (!bloomProgram.link())
        close();
}
//! [3]

//! [4]
void MainWidget::initTextures()
{
    texture = new QOpenGLTexture(QImage(":/textures/test.png").mirrored());
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

bool MainWidget::cameraCollide()
{
    return scene.collide(camera);
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
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = 0.1, zFar = 1000.0, fov = 60.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_P) {
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
    } else if (event->key() == Qt::Key_Space) {
        if (currentLevel>2) {
            togglePlayerLight();
        }
    } else if (event->key() == Qt::Key_Up) {
        loadNextLevel();
    }
    camera.handleInput(event);
}

void MainWidget::togglePlayerLight() {
    lights.lights[4].isActive = !lights.lights[4].isActive;
    if (currentLevel == 3) {
        lightToggled.insert(Red);
        lightToggled.insert(Green);
        lightToggled.insert(Blue);
        lightToggled.insert(Pink);
    }

    if (lights.lights[4].isActive) {
        neonLight.play();
        humming.play();
    } else {
        humming.stop();
    }
}

void MainWidget::timerEvent(QTimerEvent *event)
{
    QTime new_time = QTime::currentTime();
    timeElapsed = last_time.msecsTo(new_time);
    last_time = new_time;
    setWindowTitle(QString("Lux | FPS : %1").arg((int) (1000 / timeElapsed)));

    scene.updateGlobalMatrix();
    camera.update(timeElapsed);
    collideCheck();

    updatePlayerLight();
    isAllLightToggled();
    updateHumVolume();
    update();
}

void MainWidget::updatePlayerLight() {
    float n = SimplexNoise::noise((float) start_time.elapsed() / 1000.0f);
    float power = 0.5+ (n+1)/4;
    lights.lights[4].power = power;
    lights.lights[4].position = camera.getPosition();
    float t = 1000;
    n = SimplexNoise::noise((float) (start_time.elapsed()+t) / 1000.0f);
    power = 0.5+ (n+1)/2;
    lights.lights[0].power = power;
    t+=1000;
    n = SimplexNoise::noise((float) (start_time.elapsed()+t) / 1000.0f);
    power = 0.5+ (n+1)/2;
    lights.lights[1].power = power;
    t+=1000;
    n = SimplexNoise::noise((float) (start_time.elapsed()+t) / 1000.0f);
    power = 0.5+ (n+1)/2;
    lights.lights[2].power = power;
    t+=1000;
    n = SimplexNoise::noise((float) (start_time.elapsed()+t) / 1000.0f);
    power = 0.5+ (n+1)/2;
    lights.lights[3].power = power;

}

void MainWidget::isAllLightToggled() {
    if (lightToggled.size()==4) {
        terrainScene->addChild(endScene);
        lightToggled = QSet<Type>();
    }
}

void MainWidget::updateHumVolume() {
    if (currentLevel !=2) {
        return;
    }
    float nearestDistance = 5000;
    for (int i = 0; i < 4; i++) {
        QVector3D lightPos = lights.lights[i].position;
        float current = camera.getPosition().distanceToPoint(lightPos);
        if (current < nearestDistance) {
            nearestDistance = current;
        }
    }
    float vol = float_map(nearestDistance, 0,20,1,0);
    humming.setVolume(vol);
}

void MainWidget::loadNextLevel() {
    qDebug() << currentLevel + 1;
    currentType = None;
    terrainScene->removeChild(endScene);
    if (currentLevel == 1) {
        lights.lights[0].isActive = false;
        lights.lights[1].isActive = false;
        lights.lights[2].isActive = false;
        lights.lights[3].isActive = false;
        togglePlayerLight();
    } else if (currentLevel == 2) {
        lights.lights[0].isActive = true;
        lights.lights[1].isActive = true;
        lights.lights[2].isActive = true;
        lights.lights[3].isActive = true;
        neonRev.play();
    }
    humming.setVolume(0.4);
    currentLevel++;
}

void MainWidget::collideCheck() {
    for (int i = 0; i<lights.size-1; i++) {
        if (currentLevel !=3 && lights.lights[i].collide(camera.getBoundingBox())) {
            if (i == currentType) return;
            lightSwitch.play();
            lights.lights[currentType].resetPower();
            currentType = static_cast<Type>(i);
            lights.lights[currentType].increasePower();
            lights.lights[4].color = lights.lights[currentType].color;
            lightToggled.insert(currentType);
            return;
        }
    }
    if (terrainScene->hasChild(endScene) &&
            endScene->getGeometryBoundingBox().collide(camera.getBoundingBox())) {
        loadNextLevel();
    }
}

void MainWidget::paintGL()
{
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
