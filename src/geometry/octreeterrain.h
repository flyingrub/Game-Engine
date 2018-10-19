#ifndef OCTREETERRAIN_H
#define OCTREETERRAIN_H

#include <array>

class OctreeTerrain
{
public:
    OctreeTerrain();
private:
    std::array<4, OctreeTerrain*> children;
};

#endif // OCTREETERRAIN_H
