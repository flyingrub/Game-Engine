#include "terrain.h"
#include<vector>
using namespace std;

Terrain::Terrain(): width(10), height(10){
    createGeometry();
}

Terrain::Terrain(int width, int height): width(width), height(height){

}

Terrain::Terrain(QString filename, int with, int height): width(width), height(height) {
    QImage img = QImage(filename);
    heightmap = img;
    if (img.isNull()) {
        qDebug() << "Invalid Image";
    }
}

void Terrain::createGeometry() {
    for (int i=0; i<height; i++) {
        for (int j=0; j<width;j++) {
            float x = i;
            float y = j;
            float z = 0;
            if (heightmap) {
                QRgb c = heightmap.value().pixel(i,j);
                float greyValue = qGray(c);
                z = 2.0f * greyValue / 255.0f;
            }
            float positionX = (float) i / (float) (width);
            float positionY = (float) j / (float) (height);
            vertexBuffer.push_back({QVector3D(x, y,  z), QVector2D(positionX,positionY)});
        }
    }

    for (int row = 0; row < height; row++) {
        if (row != 0) indexBuffer.push_back(row * width);
        for (int col = 0; col < width; col++) {
            indexBuffer.push_back(row * width + col);
            indexBuffer.push_back((row + 1) * width + col);
        }
        if (row != height - 1) indexBuffer.push_back((row + 1) * width + width - 1);
    }
}
