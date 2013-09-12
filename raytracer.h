#pragma once
#include <vector>
#include <memory>
#include "Object.hpp"
#include "Color.hpp"

class RayTracer
{
private:
    std::vector< Object* > objects;
    std::vector< ObjectLight > lights;
    Color * buf;
    int buf_width;
    Color ray_tracing( const Ray & ray, const int & depth, int & rays_count, double *distance );
    void start_ray_tracing( const size_t & width, const size_t & height );
    void prepare_scene();
    RayTracer(){}
public:
    RayTracer( size_t width, size_t height );
    ~RayTracer();
};
