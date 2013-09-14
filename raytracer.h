#pragma once
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <list>
#include "Object.hpp"
#include "Color.hpp"

#define THREADS 2
#define MAX_DEPTH  5

class RayTracer
{
private:
    std::vector< Object* > objects;
    std::vector< ObjectLight > lights;
    size_t 			m_buf_size;
    image_t			m_image;
    uint32_t		m_aaSamples;
    Vector			m_cameraPos;
    Viewport		m_viewport;

    struct thread_task
    {
    	uint32_t 	pixel_index;
    	Vector		pointOnViewport;
    	thread_task( uint32_t pixel_index_, const Vector & pointOnViewport_ )
    		: pixel_index( pixel_index_ ), pointOnViewport( pointOnViewport_ )
    	{}
    	thread_task()
    		: pixel_index( 0 )
    	{}
    };

    std::list< thread_task >	m_tasks;
    size_t						m_tasks_count;
    std::recursive_mutex 		m_mutex;
    std::thread* 				m_threads[ THREADS ];

    void thread( uint8_t thread_index );

    Color ray_tracing( const Ray & ray, const int & depth, int & rays_count, float *distance );
    void start_ray_tracing();
    void prepare_scene();
    RayTracer()
     	 : m_buf_size( 0 ), m_aaSamples( 1 ), m_tasks_count( 0 )
    {}
public:
    RayTracer( size_t width, size_t height );
    ~RayTracer();
};
