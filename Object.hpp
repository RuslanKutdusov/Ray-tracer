#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "Ray.hpp"
#include "Vector.hpp"
#include "Material.hpp"
#define EPSILON 0.001
#define PI 3.1415926

struct Intersection{
    Vector  point;
    int     at_object;
    Vector  normal;
    Ray     reflect_ray;
    Ray     refract_ray;
    float   reflect_amount;
    Color   pixel;
};

class Object{
private:

public:
    Material m_material;
    HOST_DEVICE Object(){

    }

    HOST_DEVICE Object( const Material & material )
        : m_material( material )
    {

    }
    HOST_DEVICE virtual ~Object(){
    }
    HOST_DEVICE void get_reflect_refract_rays(const Ray & ray, Intersection & intersection) const{
        Vector i = ray.vector;
        Vector n = intersection.normal;
        intersection.reflect_ray.vector = i.reflect(n);
        intersection.reflect_ray.vector.normalize();
        intersection.reflect_ray.start_point = intersection.point;
        intersection.reflect_amount = 1.0f;
        if (m_material.m_refract_amount > 0){
            float refract_coef = m_material.m_refract_coef;
            float cos_i = -i.dot(n);
            if( cos_i < 0.0f ){
                n = n.scalar(-1);
                cos_i = -i.dot(n);
                refract_coef = 1.0f / refract_coef;
            }
            float sin2_t = refract_coef*refract_coef * (1.0f - cos_i*cos_i);
            if (sin2_t <= 1.0f ){
                float cos_t = sqrt(1.0f - sin2_t);
                intersection.refract_ray.vector = i.scalar(refract_coef) + n.scalar(refract_coef * cos_i - cos_t);
                intersection.refract_ray.vector.normalize();
                intersection.refract_ray.start_point = intersection.point;
                float Rorto  = (cos_i - refract_coef * cos_t) / (cos_i + refract_coef * cos_t);
                float Rparal = (refract_coef * cos_i - cos_t) / (refract_coef * cos_i + cos_t);
                intersection.reflect_amount = (Rorto * Rorto + Rparal * Rparal) / 2.0f;
            }
            else{
                intersection.refract_ray = intersection.reflect_ray;
            }
        }

    }
    HOST_DEVICE bool is_in_border(const float & v, const float & b1, const float & b2, float & t) const{
        t = (v - b1) / (b2 - b1);
        t = t > 1.0f ? 1.0f : t;
        if ((v >= b1 && v <= b2))// || (v >= b2 && v <= b1))
                return true;
        if (fabs(v - b1) < EPSILON || fabs(v - b2) < EPSILON)
            return true;
        return false;
    }
    HOST_DEVICE virtual bool RayIntersect(const Ray & ray, Intersection & intersection) const = 0;
};

class ObjectPlane : public Object{
private:
    Vector abcd;
    Vector normal;
    Vector b1;
    Vector b4;
    Matrix m_inverse;
public:
    HOST_DEVICE ObjectPlane(){}
    HOST_DEVICE ObjectPlane(const Matrix & m, const float & width, const float & height,
                const Material & material, bool inverse_normal = false)
        : Object(material)
    {
        m_inverse = m.inverse();
        b1 = Vector( -width / 2.0f, height / 2.0f, 0.0f );
        Vector b2 = Vector( width / 2.0f, height / 2.0f, 0.0f );
        b4 = Vector( width / 2.0f, -height / 2.0f, 0.0f);
        Vector c = m.mul( Vector( 0.0f, 0.0f, 0.0f ) );
        Vector u = m.mul(b1) - c;
        Vector v = m.mul(b2) - c;
        normal = (u * v);
        if (!inverse_normal)
            normal = normal.scalar(-1);
        normal.normalize();
        abcd = Vector(normal.x, normal.y, normal.z, -normal.dot(c));
    }
    HOST_DEVICE virtual bool RayIntersect(const Ray &ray, Intersection & intersection) const{
        intersection.normal = normal;
        const float & A = abcd.x;
        const float & B = abcd.y;
        const float & C = abcd.z;
        const float & D = abcd.w;
        const float & x0 = ray.start_point.x;
        const float & y0 = ray.start_point.y;
        const float & z0 = ray.start_point.z;
        const float & alfa =  ray.vector.x;
        const float & beta = ray.vector.y;
        const float & gamma = ray.vector.z;
        //(A,B,C) -normal
        //(a,b,g) - ray direction
        float scalar = A * alfa + B * beta + C * gamma;
        //прамая || плоскости
        if (fabs(scalar) < EPSILON){
            //printf("<e\n");
            return false;
        }
        float t = (-D - A * x0 - B * y0 - C * z0) / scalar;
        //точка должна быть по направлению луча
        if (t < 0 || fabs(t) < EPSILON){
            //printf("invalid dir\n");
            return false;
        }
        intersection.point = ray.point(t);

        Vector intr = m_inverse.mul(intersection.point);
        float tx, ty;
        if (!is_in_border(intr.x, b1.x, b4.x, tx) ||
            !is_in_border(intr.y, b4.y, b1.y, ty))
            return false;
        intersection.pixel = m_material.get_color(tx, ty);
        //printf("%f %f %f\n", intersection.pixel.r, intersection.pixel.g, intersection.pixel.b );
        get_reflect_refract_rays(ray, intersection);
        return true;
    }
};

class ObjectBox : public Object{
private:
	static const int PLANES_COUNT = 6;
    ObjectPlane planes[ PLANES_COUNT ];
    HOST_DEVICE void init_plane(const Vector & pos, const Vector & rotate, const float & size,
                    const Material & material, Matrix & m, int plane_index)
    {
        m = m * Matrix::RotateX(rotate.x) * Matrix::RotateY(rotate.y) * Matrix::RotateZ(rotate.z) * Matrix::TranslateMatrix(pos);
        planes[ plane_index ] = ObjectPlane(m, size, size, material);
    }

public:
    HOST_DEVICE ObjectBox(const Vector & pos, const Vector & rotate, const float & size,// const float & height,const float & depth,
              const Material & material)
        : Object(material)
    {
        Matrix m;
        //XY bottom
        m = Matrix::TranslateMatrix( 0.0f, 0.0f, -size / 2.0f );
        init_plane(pos, rotate, size, material, m, 0);
        //XY top
        m = Matrix::TranslateMatrix( 0.0f, 0.0f, size / 2.0f);
        init_plane(pos, rotate, size, material, m, 1);
        //YZ far
        m = Matrix::TranslateMatrix( -size / 2.0f, 0.0f, 0.0f);
        m = Matrix::RotateY(-PI / 2.0f) * m;
        init_plane(pos, rotate, size, material, m, 2);
        //YZ nead
        m = Matrix::TranslateMatrix(size / 2.0f, 0.0f, 0.0f);
        m = Matrix::RotateY(PI / 2.0f) * m;
        init_plane(pos, rotate, size, material, m, 3);
        //XZ right
        m = Matrix::TranslateMatrix(0.0f, size / 2.0f, 0);
        m = Matrix::RotateX(-PI / 2.0f) * m;
        init_plane(pos, rotate, size, material, m, 4);
        //XZ left
        m = Matrix::TranslateMatrix(0.0f, -size / 2.0f, 0);
        m = Matrix::RotateX(PI / 2.0f) * m;
        init_plane(pos, rotate, size, material, m, 5);
    }
    HOST_DEVICE virtual bool RayIntersect(const Ray &ray, Intersection &intersection) const{
        Intersection intr;
        int i_plane = -1;
        float distance2obj = INFINITY;
        for( size_t i = 0; i < PLANES_COUNT; i++ ){
            Intersection in;
            if (planes[i].RayIntersect(ray, in)){
                float dist = in.point.distance(ray.start_point);
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
    HOST_DEVICE ObjectSphere() {}
public:
    Vector m_center;
    float m_radius;
    HOST_DEVICE ObjectSphere(const Vector & center, const float & radius, const Material & material)
        : Object(material), m_center(center), m_radius(radius)
    {
    }
    HOST_DEVICE bool intersect(const Ray &ray, const float & t, Intersection & intersection) const{
        intersection.point = ray.point(t);
        if (fabs((intersection.point - m_center).length() - m_radius) > EPSILON)
                return false;
        intersection.normal = intersection.point - m_center;
        intersection.normal.normalize();
        get_reflect_refract_rays(ray, intersection);
        intersection.pixel = Color(1.0f, 1.0f, 1.0f);
        return true;
    }
    HOST_DEVICE virtual bool RayIntersect(const Ray &ray, Intersection & intersection) const{
        const float & R = m_radius;
        Vector v = ray.start_point - m_center;
        float B = v.dot(ray.vector);
        float C = v.dot(v) - R * R;
        float D = sqrt(B*B - C);
        if (isnan(D))
            return false;
        float t1 = (-B - D);
        float t2 = (-B + D);
        if (t1 < 0.0f && t2 < 0.0f)
            return false;
        float min_t = fmin(t1, t2);
        float max_t = fmax(t1, t2);
        float t = (min_t >= 0) ? min_t : max_t;
        if (fabs(t) < EPSILON){
            if (t < EPSILON)
                t = max_t;
            if (t < EPSILON)
                return false;
        }
        return intersect(ray, t, intersection);
    }
};

class ObjectLight {
private:
    ObjectLight(){}
public:
    Color m_color;
    Vector m_center;
    float m_radius;
    HOST_DEVICE ObjectLight(const Vector & center, const Color & color)
        : m_color(color), m_center(center), m_radius(0)
    {}
    HOST_DEVICE bool RayIntersectLight(const Ray &ray){
        Vector v(m_center.x - ray.start_point.x,
                 m_center.y - ray.start_point.y,
                 m_center.z - ray.start_point.z);
        float distance = (v * ray.vector).length() / ray.vector.length();
        return !(distance > m_radius);
    }
    HOST_DEVICE float distance(const Vector & point) const{
        return m_center.distance(point);
    }
};

#endif // OBJECT_HPP
