#include "geometry.h"
#include <fstream>
#include <iostream>
#include <sstream>

Geometry::Geometry() : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();
}

Geometry::Geometry(QString objFilename) : Geometry()
{
    createGeometryFromObj(objFilename);
    bind();
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

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    offset += sizeof(QVector2D);

    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

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

void Geometry::createGeometryFromObj(QString filename)
{
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    vector<QVector3D> temp_vertices;
    vector<QVector2D> temp_uvs;
    vector<QVector3D> temp_normals;

    string ignore;
    float f1,f2,f3;

    ifstream obj(filename.toStdString());
    string line;
    istringstream iss;
    if (obj.is_open()) {
        while ( getline (obj,line) ) {
            string firstWord = line.substr(0, line.find(" "));
            if (firstWord == "v") {
                iss = istringstream(line);
                iss >> ignore >> f1 >> f2 >> f3;
                temp_vertices.push_back({f1,f2,f3});
            }
            if (firstWord == "vt") {
                iss = istringstream(line);
                iss >> ignore >> f1 >> f2;
                temp_uvs.push_back({f1,f2});
            }
            if (firstWord == "vn") {
                iss = istringstream(line);
                iss >> ignore >> f1 >> f2 >> f3;
                temp_vertices.push_back({f1,f2,f3});
            }
            if (firstWord == "f") {
                QString l = QString::fromStdString(line);
                QStringList face = l.split(" ");
                for(int i = 1; i <= 3; i++) {
                    QStringList arg = face[i].split("/");
                    QVector3D vertex = temp_vertices[arg[0].toInt()];
                    QVector2D uv = temp_uvs[arg[1].toInt()];
                    QVector3D normal = temp_normals[arg[2].toInt()];
                    vertices.push_back({vertex, uv, normal});
                    indices.push_back(vertices.size() -1);
                }
            }
        }
        obj.close();
    }
}

void Geometry::optimizeIndex()
{
    indices.clear();
    vector<VertexData> optimisedVertices;
    map<VertexData, int> vertexIndex;
    map<VertexData, int>::iterator it;
    for (auto const & v : vertices) {
        it = vertexIndex.find(v);
        bool found = it != vertexIndex.end();
        if (found) {
            indices.push_back(it->second);
        } else {
            int i = optimisedVertices.size();
            vertexIndex[v] = i;
            indices.push_back(i);
            optimisedVertices.push_back(v);
        }
    }
    vertices = optimisedVertices;
}
