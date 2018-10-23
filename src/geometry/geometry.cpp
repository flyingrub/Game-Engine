#include "geometry.h"

Geometry::Geometry() : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();
}

Geometry::~Geometry() {
    arrayBuf.destroy();
    indexBuf.destroy();
}

void Geometry::calcBoundingBox()
{
    boundingBox.min = vertices[0].position;
    boundingBox.max = vertices[0].position;
    for (auto const& p : vertices) {
        if (p.position.x() < boundingBox.min.x()) {
            boundingBox.min.setX(p.position.x());
        }
        if (p.position.y() < boundingBox.min.y()) {
            boundingBox.min.setY(p.position.y());
        }
        if (p.position.z() < boundingBox.min.z()) {
            boundingBox.min.setZ(p.position.z());
        }
        if (p.position.x() > boundingBox.max.x()) {
            boundingBox.max.setX(p.position.x());
        }
        if (p.position.y() > boundingBox.max.y()) {
            boundingBox.max.setY(p.position.y());
        }
        if (p.position.z() > boundingBox.max.z()) {
            boundingBox.max.setZ(p.position.z());
        }
    }
}

void Geometry::draw(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
}

void Geometry::bind() {
    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices.data(), vertices.size() * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices.data(), indices.size() * sizeof(GLint));
}
