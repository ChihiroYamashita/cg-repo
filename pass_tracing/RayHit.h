#ifndef RAYHIT_H
#define RAYHIT_H
//詳細はVS
struct RayHit
{
    double t;
    double alpha;
    double beta;
    int mesh_idx; // < -1: no intersection, -1: area light, >= 0: object_mesh
    int primitive_idx; // < 0: no intersection
    bool isFront;
};
#endif // RAYHIT_H
