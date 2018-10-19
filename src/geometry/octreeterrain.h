#ifndef OCTREETERRAIN_H
#define OCTREETERRAIN_H

#include <array>

class QuadtreeTerrain
{
public:
    QuadtreeTerrain();
private:
    QuadtreeTerrain* children;
};

#endif // OCTREETERRAIN_H
