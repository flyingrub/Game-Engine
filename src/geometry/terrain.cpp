#include "terrain.h"
#include<vector>
using namespace std;

Terrain::Terrain(): Terrain(10,10) {

}

Terrain::Terrain(int width, int height): width(width), height(height){
    createGeometry();
    bind();
    calcBoundingBox();
}

Terrain::Terrain(QString filename, int width, int height): width(width), height(height) {
    QImage img = QImage(filename);
    heightmap = img;
    if (img.isNull()) {
        qDebug() << "Invalid Image";
    }
}

void Terrain::createGeometry() {
    for (int i=0; i<height; i++) {
        for (int j=0; j<width;j++) {
            float x = i - height / 2;
            float y = j - width / 2;
            float z = 0;
            if (heightmap) {
                QRgb c = heightmap.value().pixel(i,j);
                float greyValue = qGray(c);
                z = 2.0f * greyValue / 255.0f;
            }
            float positionX = (float) i / (float) (width-1);
            float positionY = (float) j / (float) (height-1);
            if (i%2 == 0) {
                positionY = 1;
            } else {
                positionY = 0;
            }
            if (j%2 == 0) {
                positionX = 1;
            } else {
                positionX = 0;
            }
            vertices.push_back({QVector3D(x, y,  z), QVector2D(positionX,positionY)});
        }
    }

    for (int row = 0; row < height; row++) {
        if (row != 0) indices.push_back(row * width);
        for (int col = 0; col < width; col++) {
            indices.push_back(row * width + col);
            indices.push_back((row + 1) * width + col);
        }
        if (row != height - 1) indices.push_back((row + 1) * width + width - 1);
    }
}
