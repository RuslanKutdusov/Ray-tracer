#include "raytracer.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

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

    float box_size = 12;

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

    float x = 0.0f, y = 0.0f;
    float light_intensity = 0.03f;
    for(x = 0; x < 1; x += 0.2f)
        for(y = 0; y < 1; y += 0.2f)
        {
            lights.push_back(ObjectLight(Vector(x + 2,-4 + y, 2), Color( light_intensity )));
            lights.push_back(ObjectLight(Vector(x + 4,y + 4, 3), Color( light_intensity )));
        }
}

RayTracer::RayTracer( size_t width, size_t height )
	: m_tasks_count( 0 )
{
	prepare_scene();

	m_buf_size = width * height;
	m_image.height = height;
	m_image.width = width;
	m_image.image = new Color[ m_buf_size ];

	float aspectRatio = (float)width / (float)height;
	m_cameraPos = Vector( 17.0f, 0.0f, 0.0f );
	float viewportWidth = 6.0f;
	float viewportHeight = viewportWidth / aspectRatio;
	float f = 12.0f;
	m_viewport = Viewport(Vector(f, -viewportWidth / 2.0f,  viewportHeight / 2.0f),
					  	  Vector(f,  viewportWidth / 2.0f,  viewportHeight / 2.0f),
					  	  Vector(f, -viewportWidth / 2.0f, -viewportHeight / 2.0f),
					  	  Vector(f,  viewportWidth / 2.0f, -viewportHeight / 2.0f));

	m_aaSamples = 1;

	//

	timespec tp;
	double startTime, endTime;

	clock_gettime( CLOCK_REALTIME, &tp );
	startTime = tp.tv_sec + tp.tv_nsec / 1000000000.0;

	start_ray_tracing();

	for( size_t i = 0; i < THREADS; i++ )
		m_threads[ i ] = new std::thread( &RayTracer::thread, this, i );

	for( size_t i = 0; i < THREADS; i++ )
		m_threads[ i ]->join();

	clock_gettime( CLOCK_REALTIME, &tp );
	endTime = tp.tv_sec + tp.tv_nsec / 1000000000.0;

	double renderTime = endTime - startTime;
	printf( "Render time: %g\n", renderTime );

	save_png("out.png", m_image );
}

RayTracer::~RayTracer()
{
//    delete[] buf;
//
//    for( auto ptr : objects )
//    	delete ptr;
}

Color RayTracer::ray_tracing(const Ray & ray, const int &depth, int & rays_count, float * distance)
{
    if (depth == MAX_DEPTH)
        return Color(0, 0, 0);
    Color ret(0, 0, 0);
    Intersection intr;
    int i_object = -1;
    float distance2obj = INFINITY;
    for(size_t i = 0; i < objects.size(); i++){
        Intersection in;
        if (objects[i]->RayIntersect(ray, in)){
            float dist = in.point.distance(ray.start_point);
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
        float distance2light = lights[i].distance(intr.point);
        Ray to_light(lights[i].m_center, intr.point);

        //проверям, в тени какого либо объекта или нет
        bool i_object_in_shadow = false;
        for(size_t j = 0; j < objects.size(); j++){
            Intersection intr2;
            if (objects[j]->RayIntersect(to_light, intr2)){
                float distance_ = intr2.point.distance(intr.point);
                if (distance_ < distance2light){
                    i_object_in_shadow = true;
                    break;
                }
            }
        }
        if (i_object_in_shadow)
            continue;

        float angle_cos = to_light.vector.dot(intr.normal);
        if (angle_cos > 0){
            if (!objects[i_object]->m_material.m_diffuse.is_black())
                diffuse = diffuse + lights[i].m_color * (angle_cos );
            if (!objects[i_object]->m_material.m_specular.is_black())
                specular = specular + lights[i].m_color * pow(angle_cos, objects[i_object]->m_material.m_phong) ;
        }
    }

    float d = 0;

    const float & R = intr.reflect_amount;
    float T = 1.0 - R;

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
    return ret;
}

void RayTracer::start_ray_tracing()
{
    float step_y = m_viewport.m_p2.y * 2.0f / (float)m_image.width;
    int pixel_index = 0;
    float z = m_viewport.m_p1.z;

    for( int xx = 0; xx < m_image.width; xx++ )
    {
    	z -= step_y;

        float y = m_viewport.m_p1.y;

        for( int yy = 0; yy < m_image.height; yy++ )
        {
        	y += step_y;

            //Vector diffs[4] = { Vector(0, 0, 0), Vector(0, step_y / 2.0f, 0), Vector(0, step_y / 2.0f, step_y / 2.0f), Vector(0, 0, step_y / 2.0f) };

            m_image.image[pixel_index] = Color();

            Vector viewport_point( m_viewport.m_p1.x, y, z );
  		   	//viewport_point = viewport_point + diffs[sample];
            //Ray first_ray( viewport_point, camera_pos );

			m_tasks.push_back( thread_task( pixel_index, viewport_point ) );
			m_tasks_count++;
            pixel_index++;
        }
    }
}

void RayTracer::thread( uint8_t thread_index )
{
	int rays_count = 0;
	while( 1 ){
		thread_task task;
		{
			std::lock_guard<std::recursive_mutex> lock( m_mutex );

			if( m_tasks_count % 10000 == 0 )
				printf("thread%u tasks done %u/%u\n", thread_index, m_tasks_count, m_buf_size );

			if( m_tasks_count == 0 )
				break;

			task = m_tasks.front();
			m_tasks.pop_front();

			m_tasks_count--;
		}

		Ray first_ray( task.pointOnViewport, m_cameraPos );

		const uint32_t & j = task.pixel_index;
		m_image.image[ j ] = m_image.image[ j ] + ray_tracing( first_ray, 0, rays_count, nullptr );
		if ( j == m_aaSamples - 1)
		{
			m_image.image[j].tone_mapping();
			// gamma correction
			m_image.image[j] = ( m_image.image[j] / m_aaSamples ) ^ ( 1.0f / 2.2f );
		}
	}
	printf("Thread%u done, rays calculated=%d\n", thread_index, rays_count );
}
