#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "Ray.hpp"
#include "Vector.hpp"
#include "Material.hpp"
#define EPSILON 0.001
#define PI 3.1415926

struct Intersection{
    Vector  point;
    Vector  normal;
    Ray     reflect_ray;
    Ray     refract_ray;
};

class Object{
private:

public:
    Material m_material;
    Object() = default;
    Object(const Material & material)
        : m_material(material)
    {}
    virtual ~Object(){
    }
    void get_reflect_refract_rays(const Ray & ray, Intersection & intersection){
        double cos_ = ray.vector.dot(intersection.normal);
        Vector normal = cos_ > 0 ? intersection.normal : intersection.normal.scalar(-1);
        intersection.reflect_ray.vector = ray.vector.reflect(normal);
        intersection.reflect_ray.vector.normalize();
        intersection.reflect_ray.start_point = intersection.point;
        //reflect_angle_cos = ray.vector.dot(normal) / ray.vector.length() / normal.length();
        if (m_material.m_refract_amount > 0){
            double refract_coef = cos_ > 0 ? m_material.m_refract_coef : 1 /  m_material.m_refract_coef;
            //cos_ = cos_ > 0 ? cos_ : -cos_;
            double sin_2 = refract_coef*refract_coef * (1 - cos_*cos_);
            if (sin_2 <= 1){
//                intersection.refract_ray.vector = ray.vector.scalar(refract_coef) +
//                        normal.scalar(refract_coef * cos_ - sqrt(1 - sin_2));
                double sign = cos_ > 0 ? 1: -1;
                double c = sqrt(1 - sin_2);
                intersection.refract_ray.vector = ray.vector.scalar(refract_coef)+
                        normal.scalar(sign*(c - sign*refract_coef*cos_));
            }
            else
                intersection.refract_ray.vector = intersection.reflect_ray.vector;
            intersection.refract_ray.vector.normalize();
            intersection.refract_ray.start_point = intersection.point;
        }

    }
    bool is_in_border(const double & v, const double & b1, const double & b2){
                                if ((v >= b1 && v <= b2) || (v >= b2 && v <= b1))
                return true;
        if (fabs(v - b1) < EPSILON || fabs(v - b2) < EPSILON)
            return true;
        return false;
    }
    virtual bool RayIntersect(const Ray & ray, Intersection & intersection) = 0;
};

class ObjectPlane : public Object{
private:
    Vector4 abcd;
    Vector normal;
    Vector b1;
    Vector b4;
    Matrix m_inverse;
public:
    ObjectPlane() = default;
    ObjectPlane(const Matrix & m, const double & width, const double & height,
                const Material & material, bool inverse_normal = false)
        : Object(material)
    {
        m_inverse = m.inverse();
        b1 = Vector(-width / 2, height / 2, 0);
        Vector b2 = Vector(width / 2, height / 2, 0);
        b4 = Vector(width / 2, -height / 2, 0);
        Vector c = m.mul(Vector(0, 0, 0));
        Vector u = m.mul(b1) - c;
        Vector v = m.mul(b2) - c;
        normal = (u * v);
        if (!inverse_normal)
            normal = normal.scalar(-1);
        abcd = Vector4(normal.x, normal.y, normal.z, -normal.dot(c));
        normal.normalize();
    }
    virtual bool RayIntersect(const Ray &ray, Intersection & intersection){
        intersection.normal = normal;
        double & A = abcd.x;
        double & B = abcd.y;
        double & C = abcd.z;
        double & D = abcd.w;
        const double & x0 = ray.start_point.x;
        const double & y0 = ray.start_point.y;
        const double & z0 = ray.start_point.z;
        const double & alfa =  ray.vector.x;
        const double & beta = ray.vector.y;
        const double & gamma = ray.vector.z;
        //(A,B,C) -normal
        //(a,b,g) - ray direction
        double scalar = A * alfa + B * beta + C * gamma;
        //прамая || плоскости
        if (fabs(scalar) < EPSILON){
            //printf("<e\n");
            return false;
        }
        double t = (-D - A * x0 - B * y0 - C * z0) / scalar;
        //точка должна быть по направлению луча
        if (t < 0 || fabs(t) < EPSILON){
            //printf("invalid dir\n");
            return false;
        }
        intersection.point = ray.point(t);
        if (A * intersection.point.x + B * intersection.point.y + C * intersection.point.z + D < EPSILON){
            Vector intr = m_inverse.mul(intersection.point);
            if (!is_in_border(intr.x, b1.x, b4.x) ||
                !is_in_border(intr.y, b1.y, b4.y))
                return false;
            get_reflect_refract_rays(ray, intersection);
            return true;
        }
        //printf("no %f\n",A * intersection.point.x + B * intersection.point.y + C * intersection.point.z + D);

        return false;
    }
};

class ObjectBox : public Object{
private:
    ObjectPlane planes[6];
    void init_plane(const size_t & index, const Vector & pos, const Vector & rotate, const double & size,
                    const Material & material, Matrix & m)
    {
        m = m * Matrix::RotateX(rotate.x) * Matrix::RotateY(rotate.y) * Matrix::RotateZ(rotate.z) * Matrix::TranslateMatrix(pos);
        planes[index] =  ObjectPlane(m, size, size, material);
    }

public:
    ObjectBox(const Vector & pos, const Vector & rotate, const double & size,// const double & height,const double & depth,
              const Material & material)
        : Object(material)
    {
        Matrix m = Matrix::TranslateMatrix(0, 0, -size / 2);
        init_plane(0, pos, rotate, size, material, m);

        m = Matrix::TranslateMatrix(0, 0, size / 2);
        init_plane(1, pos, rotate, size, material, m);

        m = Matrix::TranslateMatrix(-size / 2, 0, 0);
        m = Matrix::RotateY(-PI / 2) * m;
        init_plane(2, pos, rotate, size, material, m);

        m = Matrix::TranslateMatrix(size / 2, 0, 0);
        m = Matrix::RotateY(PI / 2) * m;
        init_plane(3, pos, rotate, size, material, m);;

        m = Matrix::TranslateMatrix(0, size / 2, 0);
        m = Matrix::RotateX(-PI / 2) * m;
        init_plane(4, pos, rotate, size, material, m);

        m = Matrix::TranslateMatrix(0, -size / 2, 0);
        m = Matrix::RotateX(PI / 2) * m;
        init_plane(5, pos, rotate, size, material, m);;
    }
    virtual bool RayIntersect(const Ray &ray, Intersection &intersection){
        Intersection intr;
        int i_plane = -1;
        double distance2obj = INFINITY;
        for(size_t i = 0; i < 6; i++){
            Intersection in;
            if (planes[i].RayIntersect(ray, in)){
                double dist = in.point.distance(ray.start_point);
                if (dist < distance2obj){
                    distance2obj = dist;
                    intr = in;
                    i_plane = i;
                }
            }
        }
        if (i_plane == -1)
            return false;
        intersection = intr;
        return true;
    }
};

class ObjectSphere : public Object{
private:
    ObjectSphere() = default;
public:
    Vector m_center;
    double m_radius;
    ObjectSphere(const Vector & center, const double & radius, const Material & material)
        : Object(material), m_center(center), m_radius(radius)
    {
    }
    bool intersect(const Ray &ray, const double & t, Intersection & intersection){
        if (fabs(t) < EPSILON)
            return false;
        intersection.point = ray.point(t);
        if (fabs((intersection.point - m_center).length() - m_radius) > EPSILON)
                return false;
        intersection.normal = intersection.point - m_center;
        intersection.normal.normalize();
        get_reflect_refract_rays(ray, intersection);
        return true;
    }
    virtual bool RayIntersect(const Ray &ray, Intersection & intersection){
        double & R = m_radius;
        Vector v = ray.start_point - m_center;
        double B = v.dot(ray.vector);
        double C = v.dot(v) - R * R;
        double D = sqrt(B*B - C);
        if (D < 0)
            return false;
        double t1 = (-B - D);
        double t2 = (-B + D);
        if (t1 < t2){
            if (t1 < 0)
                return intersect(ray, t2, intersection);
            return intersect(ray, t1, intersection);
        }
        else
        if (t2 < t1){
            if (t2 < 0)
                return intersect(ray, t1, intersection);
            return intersect(ray, t2, intersection);
        }
        else if(t1 > 0 && t2 > 0)
            return intersect(ray, t1, intersection);
        return false;
    }
};

class ObjectLight {
private:
    ObjectLight() = default;
public:
    Color m_color;
    Vector m_center;
    double m_radius;
    ObjectLight(const Vector & center, const Color & color)
        : m_color(color), m_center(center), m_radius(1)
    {}
    bool RayIntersectLight(const Ray &ray){
        Vector v(m_center.x - ray.start_point.x,
                 m_center.y - ray.start_point.y,
                 m_center.z - ray.start_point.z);
        double distance = (v * ray.vector).length() / ray.vector.length();
        return !(distance > m_radius);
    }
    double distance(const Vector & point){
        return m_center.distance(point);
    }
};

#endif // OBJECT_HPP
