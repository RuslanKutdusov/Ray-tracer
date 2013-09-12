#include "raytracer.h"
#include <stdio.h>
#include <math.h>

void RayTracer::prepare_scene()
{
    Material m1(Color(0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0), Color(0.5, 0.5, 0.5), 5, 15, 0, 0, "wood_006.png");
    Material m2(Color(0.0, 0.1, 0.0), Color(0.1, 0.4, 0.1), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m3(Color(0.0, 0.0, 0.1), Color(0.1, 0.1, 0.4), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m4(Color(0.1, 0.1, 0.0), Color(0.4, 0.4, 0.1), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m5(Color(0.0, 0.1, 0.1), Color(0.1, 0.4, 0.4), Color(0.5, 0.5, 0.5), 5, 8, 0, 0);
    Material m6(Color(), Color(), Color(), 0.01, 2, 0, 0);
    Material m7(Color(), Color(), Color(0.5, 0.5, 0.5), 0, 10, 1, 0.5);
    Material m8(Color(), Color(0.2, 0.7, 0.5), Color(0.5, 0.5, 0.5), 0.5, 10, 0, 0);
    Material m9(Color(), Color(), Color(0.5, 0.5, 0.5), 10000, 10, 1, 0.6);

    double box_size = 12;

    //YZ far
    Matrix m = Matrix::TranslateMatrix(-box_size / 2, 0, 0);
    m = Matrix::RotateY(PI / 2) * m;
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));
    //XY top
    m = Matrix::TranslateMatrix(0, 0, box_size / 2);
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1, true));
    //XY bottom
    m = Matrix::TranslateMatrix(0, 0, -box_size / 2);
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));
    //XZ left
    m = Matrix::TranslateMatrix(0, box_size / 2, 0);
    m = Matrix::RotateX(PI / 2) * m;
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));
    //XZ right
    m = Matrix::TranslateMatrix(0, -box_size / 2, 0);
    m = Matrix::RotateX(-PI / 2) * m;
    objects.push_back(new ObjectPlane(m, box_size, box_size, m1));

    objects.push_back(new ObjectBox(Vector(0, -2, -box_size / 2 + 1.5), Vector(0, 0, -0.5), 3, m8));

    objects.push_back(new ObjectSphere(Vector(1.5, 1.5, -box_size / 2 + 2), 2, m6));
    objects.push_back(new ObjectSphere(Vector(1.5, 2, 2), 2, m9));
    objects.push_back(new ObjectSphere(Vector(0, -2, -box_size / 2 + 4.5), 1.5, m7));

    double x = 0, y = 0;
    for(x = 0; x < 1; x += 0.2)
        for(y = 0; y < 1; y += 0.2)
        {
            lights.push_back(ObjectLight(Vector(x + 2,-4 + y, 2), Color(0.03, 0.03, 0.03)));
            lights.push_back(ObjectLight(Vector(x + 4,y + 4, 3), Color(0.03, 0.03, 0.03)));
        }
}

RayTracer::RayTracer( size_t width, size_t height )
{
	buf_width = width;
	prepare_scene();

	buf = new Color[ width * height ];

	start_ray_tracing( width, height );

	image_t image;
	image.height = height;
	image.width = width;
	image.image = buf;

	save_png("out.png", image );
}

RayTracer::~RayTracer()
{
    delete[] buf;

    for( auto ptr : objects )
    	delete ptr;
}

#define MAX_DEPTH  5

Color RayTracer::ray_tracing(const Ray & ray, const int &depth, int & rays_count, double * distance)
{
    if (depth == MAX_DEPTH)
        return Color(0, 0, 0);
    Color ret(0, 0, 0);
    Intersection intr;
    int i_object = -1;
    double distance2obj = INFINITY;
    for(size_t i = 0; i < objects.size(); i++){
        Intersection in;
        if (objects[i]->RayIntersect(ray, in)){
            double dist = in.point.distance(ray.start_point);
            if (dist < distance2obj){
                distance2obj = dist;
                intr = in;
                i_object = i;
            }
        }
    }
    if (i_object == -1)
        return ret;

    rays_count++;

    if (distance)
        *distance = distance2obj;

    int depth_ = depth + 1;

    Color diffuse;
    Color specular;
    for(size_t i = 0; i < lights.size(); i++){
        double distance2light = lights[i].distance(intr.point);
        Ray to_light(lights[i].m_center, intr.point);

        //проверям, в тени какого либо объекта или нет
        bool i_object_in_shadow = false;
        for(size_t j = 0; j < objects.size(); j++){
            Intersection intr2;
            if (objects[j]->RayIntersect(to_light, intr2)){
                double distance_ = intr2.point.distance(intr.point);
                if (distance_ < distance2light){
                    i_object_in_shadow = true;
                    break;
                }
            }
        }
        if (i_object_in_shadow)
            continue;

        double angle_cos = to_light.vector.dot(intr.normal);
        if (angle_cos > 0){
            if (!objects[i_object]->m_material.m_diffuse.is_black())
                diffuse = diffuse + lights[i].m_color * (angle_cos );
            if (!objects[i_object]->m_material.m_specular.is_black())
                specular = specular + lights[i].m_color * pow(angle_cos, objects[i_object]->m_material.m_phong) ;
        }
    }

    double d = 0;

    const double & R = intr.reflect_amount;
    double T = 1.0 - R;

    Color reflect_ray_color = ray_tracing(intr.reflect_ray, depth_, rays_count, &d);
    reflect_ray_color = reflect_ray_color * exp(-objects[i_object]->m_material.m_beta) * R;

    Color refract_ray_color;
    if (objects[i_object]->m_material.m_refract_amount > 0 && T > EPSILON)
        refract_ray_color = ray_tracing(intr.refract_ray, depth_, rays_count, NULL) * objects[i_object]->m_material.m_refract_amount * T;

    ret = objects[i_object]->m_material.m_ambient +
          objects[i_object]->m_material.m_diffuse * diffuse * intr.pixel +
          objects[i_object]->m_material.m_specular * specular +
            reflect_ray_color +
            refract_ray_color ;
    //fflush(stdout);
    return ret;
}

void RayTracer::start_ray_tracing(const size_t & width, const size_t & height)
{
    double ratio = (double)width / (double)height;
    Vector camera_pos(17, 0, 0);
    double w = 6;
    double h = w / ratio;
    double f = 12;
    Viewport viewport(Vector(f, -w/2, h/2),
                      Vector(f, w/2, h/2),
                      Vector(f, -w/2, -h/2),
                      Vector(f, w/2, -h/2));
    double step_y = w / width;

    int j = 0;
    double z = viewport.m_p1.z;
    //for(double y = viewport.m_p1.y; y < viewport.m_p2.y; y += step_y){
    for( int xx = 0; xx < width; xx++ )
    {
    	//y += step_y;
    	z -= step_y;
        int rays_count = 0;
        double y = viewport.m_p1.y;
        //for(double z = viewport.m_p1.z; z > viewport.m_p3.z; z -= step_y){
        for( int yy = 0; yy < height; yy++ ){
        	//z -= step_y;
        	y += step_y;
            Matrix m = Matrix::RotateY(-0.1);
            Vector diffs[4] = { Vector(0, 0, 0), Vector(0, step_y / 2, 0), Vector(0, step_y / 2, step_y / 2), Vector(0, 0, step_y / 2) };
            buf[j] = Color();
            size_t samples = 1;
            int rays_count_per_sample = 0;
            for(size_t i = 0; i < samples; i++){
                Vector viewport_point(viewport.m_p1.x, y, z);
                viewport_point = viewport_point + diffs[i];
                Ray first_ray(viewport_point, camera_pos);
                first_ray.start_point = camera_pos + Vector(0, 0, -1);
                first_ray.vector = m.mul(first_ray.vector);
                buf[j] = buf[j] + ray_tracing(first_ray, 0, rays_count_per_sample, NULL);
                rays_count += rays_count_per_sample;
            }
            double lum = buf[j].luminance();
            buf[j] = buf[j] / ( lum + 1.0 );
            buf[j] = ( buf[j] / samples ) ^ ( 1.0f / 2.2f );
            j++;
            //buf[(int)xx][(int)yy] = ray_tracing(first_ray, 0, rays_count, NULL);
        }
        printf("%d/%u rays=%d\n", (int)xx, width, rays_count);
        fflush(stdout);
    }
}
