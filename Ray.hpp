#ifndef RAY_HPP
#define RAY_HPP

#include "Vector.hpp"

class Ray{
private:
public:
    Vector vector;
    Vector  start_point;
    Ray() = default;
    Ray(const Vector & end_point, const Vector & start_point_)
        : vector(end_point.x - start_point_.x, end_point.y - start_point_.y, end_point.z - start_point_.z),
          start_point(start_point_)
    {
        vector.normalize();
    }
    virtual ~Ray(){
    }
    Vector point(const double & t) const{
        const double& x0 = start_point.x;
        const double & y0 = start_point.y;
        const double & z0 = start_point.z;
        const double & alfa =  vector.x;
        const double & beta = vector.y;
        const double & gamma = vector.z;
        return Vector(x0 + alfa * t,
                   y0 + beta * t,
                   z0 + gamma * t);
    }
};

#endif // RAY_HPP
