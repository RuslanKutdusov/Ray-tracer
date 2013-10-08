#ifndef RAY_HPP
#define RAY_HPP

#include "cuda.hpp"
#include "Vector.hpp"

class Ray{
private:
public:
    Vector	vector;
    Vector 	start_point;
    HOST_DEVICE Ray() {}
    HOST_DEVICE Ray( const Vector & end_point, const Vector & start_point_ )
        : vector( end_point.x - start_point_.x, end_point.y - start_point_.y, end_point.z - start_point_.z ),
          start_point( start_point_ )
    {
        vector.normalize();//sure?
    }
    HOST_DEVICE ~Ray()
    {
    }
    HOST_DEVICE Vector point( const float & t ) const
    {
        const float& x0 = start_point.x;
        const float & y0 = start_point.y;
        const float & z0 = start_point.z;
        //const float & w0 = start_point.w;
        const float & alfa =  vector.x;
        const float & beta = vector.y;
        const float & gamma = vector.z;
        //const float & delta = vector.w;
        return Vector(x0 + alfa * t,
                   y0 + beta * t,
                   z0 + gamma * t,
                   1.0f );
    }
};

#endif // RAY_HPP
