#include "Vector.hpp"
#include "Ray.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include <stdint.h>
#include <stdio.h>

#define EPSILON 0.0001f
#define PI 3.1415926f

__constant__ Material 		g_materials[100];
__constant__ uint32_t 		g_materialsNumber;


struct Intersection
{
    Vector  point;
    Vector  normal;
    float	tx;
    float 	ty;
    float 	padding[ 2 ];
} __attribute__ ( ( aligned( 16 ) ) );


//
HOST_DEVICE void GetReflectRefractVectors( const Material& material, const Ray & ray, const Intersection& intersection, Vector& reflect, Vector& refract, float& reflectAmount )
{
    Vector i = ray.vector;
    Vector n = intersection.normal;
    reflect = i.reflect( n );
    reflect.normalize();
    reflectAmount = 1.0f;
    if ( material.m_refract_amount > 0 )
    {
        float refract_coef = material.m_refract_coef;
        float cos_i = -i.dot( n );
        if( cos_i < 0.0f )
        {
            n = n.scalar( -1 );
            cos_i = -i.dot( n );
            refract_coef = 1.0f / refract_coef;
        }
        float sin2_t = refract_coef * refract_coef * ( 1.0f - cos_i * cos_i );

        if( sin2_t <= 1.0f  )
        {
            float cos_t = sqrt( 1.0f - sin2_t );
            refract = i.scalar( refract_coef ) + n.scalar( refract_coef * cos_i - cos_t );
            refract.normalize();
            float Rorto  = ( cos_i - refract_coef * cos_t ) / ( cos_i + refract_coef * cos_t );
            float Rparal = ( refract_coef * cos_i - cos_t ) / ( refract_coef * cos_i + cos_t );
            reflectAmount = ( Rorto * Rorto + Rparal * Rparal ) / 2.0f;
        }
        else
        {
            refract = reflect;
        }
    }
}


//
HOST_DEVICE bool is_in_border( const float & v, const float & b1, const float & b2, float & t )
{
	t = ( v - b1 ) / ( b2 - b1 );
	t = t > 1.0f ? 1.0f : t;
	if( ( v >= b1 && v <= b2 ) )// || ( v >= b2 && v <= b1 ) )
			return true;
	if( fabs( v - b1 ) < EPSILON || fabs( v - b2 ) < EPSILON )
		return true;
	return false;
}


//
struct ObjectPlane
{
	uint32_t 	material;
	Vector 		abcd;
	Vector 		normal;
	Vector 		b1;
	Vector 		b4;
	Matrix 		inverse;
	HOST_DEVICE ObjectPlane()
	{

	}
	HOST_DEVICE ObjectPlane( const Matrix & m, float width, float height,
							uint32_t material_, bool inverse_normal = false )
	{
		inverse = m.inverse();
		b1 = Vector( -width / 2.0f, height / 2.0f, 0.0f );
		Vector b2 = Vector( width / 2.0f, height / 2.0f, 0.0f );
		b4 = Vector( width / 2.0f, -height / 2.0f, 0.0f );
		Vector c = m.mul( Vector( 0.0f, 0.0f, 0.0f ) );
		Vector u = m.mul( b1 ) - c;
		Vector v = m.mul( b2 ) - c;
		normal = ( u * v );
		if( !inverse_normal )
			normal = normal.scalar( -1 );
		normal.normalize();
		abcd = Vector( normal.x, normal.y, normal.z, -normal.dot( c ) );
		material = material_;
	}
	HOST_DEVICE bool CheckIntersection( const Ray& ray, Intersection & intersection )
	{
		intersection.normal = normal;
		const float& A = abcd.x;
		const float& B = abcd.y;
		const float& C = abcd.z;
		const float& D = abcd.w;
		const float& x0 = ray.start_point.x;
		const float& y0 = ray.start_point.y;
		const float& z0 = ray.start_point.z;
		const float& alfa =  ray.vector.x;
		const float& beta = ray.vector.y;
		const float& gamma = ray.vector.z;
		//( A,B,C ) -normal
		//( a,b,g ) - ray direction
		float scalar = A * alfa + B * beta + C * gamma;
		//прамая || плоскости
		if( fabs( scalar ) < EPSILON )
			return false;
		float t = ( -D - A * x0 - B * y0 - C * z0 ) / scalar;
		//точка должна быть по направлению луча
		if( t < 0 || fabs( t ) < EPSILON )
			return false;
		intersection.point = ray.point( t );

		Vector intr = inverse.mul( intersection.point );
		if ( !is_in_border( intr.x, b1.x, b4.x, intersection.tx ) || !is_in_border( intr.y, b4.y, b1.y, intersection.ty ) )
			return false;

		return true;
	}
};

struct ObjectSphere
{
public:
	uint32_t 	material;
	Vector  	position;
    float  		radius;

    HOST_DEVICE ObjectSphere() 
    {

    }

    HOST_DEVICE ObjectSphere( const Vector & position_, float radius_, uint32_t material_ )
       : position( position_ ), radius( radius_ ), material( material_ )
    {

    }

    HOST_DEVICE bool intersect( const Ray &ray, const float & t, Intersection & intersection ) const
    {
        intersection.point = ray.point( t );
        if( fabs( ( intersection.point - position ).length() - radius ) > EPSILON )
               return false;

        intersection.normal = intersection.point - position;
        intersection.normal.normalize();
        intersection.tx = 0.0f;
        intersection.ty = 0.0f;

        return true;
    }

    HOST_DEVICE bool CheckIntersection( const Ray &ray, Intersection & intersection ) const
    {
        const float & R = radius;
        Vector v = ray.start_point - position;
        float B = v.dot( ray.vector );
        float C = v.dot( v ) - R * R;
        float D = B * B - C;
        if( D < 0.0f )
        	return false;
        D = sqrtf( D );
        float t1 = ( -B - D );
        float t2 = ( -B + D );
        if( t1 < 0.0f && t2 < 0.0f )
            return false;
        float min_t = fmin( t1, t2 );
        float max_t = fmax( t1, t2 );
        float t = ( min_t >= 0 ) ? min_t : max_t;
        if( fabs( t ) < EPSILON )
        {
            if ( t < EPSILON )
                t = max_t;
            if ( t < EPSILON )
                return false;
        }
        return intersect( ray, t, intersection );
    }
};

struct PointLight
{
	Color		color;
	Vector 		position;
	float		radius;

	HOST_DEVICE PointLight()
	{

	}

	HOST_DEVICE PointLight( const Color & c, const Vector & pos, float radius_ )
	{
		color = c;
		position = pos;
		radius = radius_;
	}
	
	PointLight operator=( const PointLight& pl )
	{
		color = pl.color;
		position = pl.position;
		radius = pl.radius;
		return *this;
	}

	HOST_DEVICE float distance( const Vector & point ) const
	{
		return position.distance( point );
	}
};

__constant__ PointLight 	g_pointLights[100];
__constant__ uint32_t 		g_pointLightsNumber;
__constant__ ObjectPlane 	g_objPlanes[100];
__constant__ uint32_t 		g_objPlanesNumber;
__constant__ ObjectSphere 	g_objSpheres[100];
__constant__ uint32_t 		g_objSpheresNumber;


//
__device__ bool CheckIntersection( uint32_t objectIndex, const Ray & ray, Intersection & intr )
{
	if( objectIndex < g_objPlanesNumber )
		return g_objPlanes[ objectIndex ].CheckIntersection( ray, intr );
	const uint32_t& nextIndex = objectIndex - g_objPlanesNumber;
	if( nextIndex < g_objSpheresNumber )
		return g_objSpheres[ nextIndex ].CheckIntersection( ray, intr );
	return false;
}


//
__device__ const Material& GetMaterial( uint32_t objectIndex )
{
	if( objectIndex < g_objPlanesNumber )
		return g_materials[ g_objPlanes[ objectIndex ].material ];
	const uint32_t& nextIndex = objectIndex - g_objPlanesNumber;
	if( nextIndex < g_objSpheresNumber )
		return g_materials[ g_objSpheres[ nextIndex ].material ];
	return g_materials[ nextIndex ];
}


//
__device__ Color ray_trace( const Ray & ray, Intersection & intr, uint32_t objNumber )
{
	uint32_t i_object = ~0u;
	float distance2obj = INFINITY;

	for( uint32_t i = 0; i < objNumber; i++ )
	{
		Intersection in;
		if( CheckIntersection( i, ray, in ) )
		{
			const float& dist = in.point.distance( ray.start_point );
			if( dist < distance2obj )
			{
				distance2obj = dist;
				intr = in;
				i_object = i;
			}
		}
	}
	if( i_object == ~0u )
		return Color( 0.0f, 0.0f, 0.0f );

	Ray reflectRay;
	Ray refractRay;
	float reflectAmount;

	const Material& material = GetMaterial( i_object );
	Color ret = material.m_ambient;

	reflectRay.start_point = intr.point;
	refractRay.start_point = intr.point;
	GetReflectRefractVectors( material, ray, intr, reflectRay.vector, refractRay.vector, reflectAmount );

	for( uint32_t i = 0; i < g_pointLightsNumber; i++ )
	{
		const float distance2light = g_pointLights[ i ].distance( intr.point );
		Vector fromLight = intr.point - g_pointLights[ i ].position;
		Ray to_light( g_pointLights[ i ].position, intr.point );

		//проверям, в тени какого либо объекта или нет
		bool i_object_in_shadow = false;
		for( uint32_t j = 0; j < objNumber; j++ )
		{
			Intersection intr2;
			if( CheckIntersection( j, to_light, intr2 ) )
			{
				const float& distance_ = intr2.point.distance( intr.point );
				if( distance_ < distance2light )
				{
					i_object_in_shadow = true;
					break;
				}
			}
		}
		if( i_object_in_shadow )
			continue;

		float attenuation = distance2light / g_pointLights[ i ].radius;
		if( attenuation > 1.0f )
			continue;
		attenuation = 1.0f - attenuation;

		float angle_cos = to_light.vector.dot( intr.normal );
		if( angle_cos > 0 )
			if( !material.m_diffuse.is_black() )
				ret = ret + g_pointLights[ i ].color * angle_cos * material.m_diffuse * attenuation;

		angle_cos = to_light.vector.dot( reflectRay.vector );
		if( angle_cos > 0 )
			if( !material.m_specular.is_black() )
				ret = ret + g_pointLights[ i ].color * pow( angle_cos, material.m_phong ) * attenuation;
	}

	return ret;
}

__global__ void calculate_light( Color* image, uint32_t width, uint32_t height, Vector cameraPos, Viewport viewport, uint32_t objNumber )
{
	uint32_t pixel_index = blockIdx.x * blockDim.x + threadIdx.x;
	uint32_t yi = pixel_index / width;
	uint32_t xi = pixel_index - yi * width;
	float delta_x = ( viewport.m_p2.x - viewport.m_p1.x ) / ( float )width;
	float delta_z = ( viewport.m_p2.z - viewport.m_p1.z ) / ( float )width;
	float delta_y = ( viewport.m_p1.y - viewport.m_p3.y ) / ( float )height;
	float x = delta_x * xi + viewport.m_p1.x;
	float y = viewport.m_p1.y - delta_y * yi;
	float z = delta_z * xi + viewport.m_p1.z;

	Ray ray( Vector( x, y, z ), cameraPos );
	Intersection intersection;

	uint32_t i_object = ~0u;
	float distance2obj = INFINITY;

	for( uint32_t i = 0; i < objNumber; i++ )
	{
		Intersection in;
		if( CheckIntersection( i, ray, in ) )
		{
			const float& dist = in.point.distance( ray.start_point );
			if( dist < distance2obj )
			{
				distance2obj = dist;
				intersection = in;
				i_object = i;
			}
		}
	}
	if( i_object == ~0u )
		return Color( 0.0f, 0.0f, 0.0f );

	Ray reflectRay;
	Ray refractRay;
	float reflectAmount;

	const Material& material = GetMaterial( i_object );
	Color ret = material.m_ambient;

	reflectRay.start_point = intersection.point;
	refractRay.start_point = intersection.point;
	GetReflectRefractVectors( material, ray, intersection, reflectRay.vector, refractRay.vector, reflectAmount );

	for( uint32_t i = 0; i < g_pointLightsNumber; i++ )
	{
		const float distance2light = g_pointLights[ i ].distance( intersection.point );
		Vector fromLight = intersection.point - g_pointLights[ i ].position;
		Ray to_light( g_pointLights[ i ].position, intersection.point );

		//проверям, в тени какого либо объекта или нет
		bool i_object_in_shadow = false;
		for( uint32_t j = 0; j < objNumber; j++ )
		{
			Intersection intr2;
			if( CheckIntersection( j, to_light, intr2 ) )
			{
				const float& distance_ = intr2.point.distance( intersection.point );
				if( distance_ < distance2light )
				{
					i_object_in_shadow = true;
					break;
				}
			}
		}
		if( i_object_in_shadow )
			continue;

		float attenuation = distance2light / g_pointLights[ i ].radius;
		if( attenuation > 1.0f )
			continue;
		attenuation = 1.0f - attenuation;

		float angle_cos = to_light.vector.dot( intersection.normal );
		if( angle_cos > 0 )
			if( !material.m_diffuse.is_black() )
				ret = ret + g_pointLights[ i ].color * angle_cos * material.m_diffuse * attenuation;

		angle_cos = to_light.vector.dot( reflectRay.vector );
		if( angle_cos > 0 )
			if( !material.m_specular.is_black() )
				ret = ret + g_pointLights[ i ].color * pow( angle_cos, material.m_phong ) * attenuation;
	}

	image[ pixel_index ] = ret;

//	float d = 0;

//	const float & R = intr.reflect_amount;
//	float T = 1.0 - R;
//
//	Color reflect_ray_color = ray_tracing( intr.reflect_ray, depth_, rays_count, &d );
//	reflect_ray_color = reflect_ray_color * exp( -objects[i_object]->m_material.m_beta ) * R;
//
//	Color refract_ray_color;
//	if ( objects[i_object]->m_material.m_refract_amount > 0 && T > EPSILON )
//		refract_ray_color = ray_tracing( intr.refract_ray, depth_, rays_count, NULL ) * objects[i_object]->m_material.m_refract_amount * T;
//
//	ret = objects[i_object]->m_material.m_ambient +
//		  objects[i_object]->m_material.m_diffuse * diffuse * intr.pixel +
//		  objects[i_object]->m_material.m_specular * specular +
//			reflect_ray_color +
//			refract_ray_color ;


//	image[ pixel_index ].r = 0;
//	image[ pixel_index ].g = 0;
//	image[ pixel_index ].b = 0;
//	if( fabs( x ) < 0.1f && y > 0.0f )
//		image[ pixel_index ].r = 1.0f;
//	if( fabs( x ) < 0.1f && y < 0.0f )
//		image[ pixel_index ].g = 1.0f;
//	if( fabs( y ) < 0.1f && x > 0.0f )
//		image[ pixel_index ].b = 1.0f;
//	if( fabs( y ) < 0.1f && x < 0.0f )
//	{
//		image[ pixel_index ].g = 1.0f;
//		image[ pixel_index ].b = 0.5f;
//	}
}

#define CUDA_CHECK_RETURN( value ) {											\
	cudaError_t _m_cudaStat = value;										\
	if ( _m_cudaStat != cudaSuccess ) {										\
		fprintf( stderr, "Error '%s' at line %d in file %s\n",					\
				cudaGetErrorString( _m_cudaStat ), __LINE__, __FILE__ );		\
		exit( 1 );															\
	} }

int main()
{
	Material m1( Color( 0.01f, 0.01f, 0.01f ), Color( 0.8f, 0.8f, 0.8f ), Color( 0.5f, 0.5f, 0.7f ), 5.0f, 20.0f, 0.0f, 0.0f );
	uint32_t matNumber = 1;

	const uint32_t lightsNumber = 2;
	PointLight pointLights[ lightsNumber ];
	pointLights[ 0 ] = PointLight( Color( 1.0f, 0.0f, 0.0f ), Vector( -2.0f, 4.0f, -1.0f ), 10.0f );
	pointLights[ 1 ] = PointLight( Color( 0.0f, 1.0f, 0.0f ), Vector( 2.0f, 4.0f, -1.0f ), 10.0f );

	Matrix m = Matrix::RotateX( PI / 2.0f );
	m = m * Matrix::TranslateMatrix( 0.0f, -3.0f, 0.0f );
	ObjectPlane plane( m, 100.0f, 100.0f, 0, true );
	uint32_t planesNumber = 1;

	ObjectSphere sphere( Vector( 0.0f, 0.0f, 0.0f ), 1.5f, 0 );
	uint32_t spheresNumber = 1;

	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_materials, &m1, sizeof( Material ), 0 ) );
	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_materialsNumber, &matNumber, sizeof( uint32_t ) * matNumber, 0 ) );

	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_pointLights, &pointLights, sizeof( PointLight ) * lightsNumber, 0 ) );
	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_pointLightsNumber, &lightsNumber, sizeof( uint32_t ), 0 ) );

	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_objPlanes, &plane, sizeof( ObjectPlane ) * planesNumber, 0 ) );
	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_objPlanesNumber, &planesNumber, sizeof( uint32_t ), 0 ) );

	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_objSpheres, &sphere, sizeof( ObjectSphere ) * spheresNumber, 0 ) );
	CUDA_CHECK_RETURN( cudaMemcpyToSymbol( g_objSpheresNumber, &spheresNumber, sizeof( uint32_t ), 0 ) );

	image_t image;
	image.width = 2048;
	image.height = 2048;
	uint32_t image_size = image.width * image.height;

	image.image = new Color[ image_size ];

	float aspectRatio = ( float )image.width / ( float )image.height;
	Vector cameraPos( 0.0f, 0.0f, -10.0f );
	float viewportWidth = 6.0f;
	float viewportHeight = viewportWidth / aspectRatio;
	float f = -5.0f;
	Viewport viewport( Vector( -viewportWidth / 2.0f,  viewportHeight / 2.0f, f ),
					  Vector( viewportWidth / 2.0f,  viewportHeight / 2.0f, f ),
					  Vector( -viewportWidth / 2.0f, -viewportHeight / 2.0f, f ),
					  Vector( viewportWidth / 2.0f, -viewportHeight / 2.0f, f ) );

	Color* image_device;
	CUDA_CHECK_RETURN( cudaMalloc( &image_device, image_size * sizeof( Color ) ) );
//	ObjectPlane* objPlane_device;
//	CUDA_CHECK_RETURN( cudaMalloc( &objPlane_device, 1 * sizeof( ObjectPlane ) ) );
//	CUDA_CHECK_RETURN( cudaMemcpy( objPlane_device, &plane, sizeof( ObjectPlane ), cudaMemcpyHostToDevice ) );
	//CUDA_CHECK_RETURN( cudaMemset( image_device, -1, image_size * sizeof( Color ) ) );

	uint32_t gridSize = image_size / 256;
	printf( "%u\n", gridSize );
	calculate_light<<< gridSize, 256 >>>( image_device, image.width, image.height, cameraPos, viewport, planesNumber + spheresNumber );


	CUDA_CHECK_RETURN( cudaThreadSynchronize() );
	CUDA_CHECK_RETURN( cudaGetLastError() );

	CUDA_CHECK_RETURN( cudaMemcpy( image.image, image_device, image_size * sizeof( Color ), cudaMemcpyDeviceToHost ) );

	CUDA_CHECK_RETURN( cudaFree( ( void* )image_device ) );
	//CUDA_CHECK_RETURN( cudaFree( ( void* )objPlane_device ) );
	CUDA_CHECK_RETURN( cudaDeviceReset() );

	save_png( "out.png", image );

	return 0;
}

/*
#include <cuda_runtime.h>
#include <math.h>
#include <vector_types.h>
#include <vector_functions.h>
#include "cuda.hpp"

#define EPSILON 0.001

HOST_DEVICE inline float4 operator+( const float4 & a, const float4 & b )
{
	return make_float4( a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w );
}

HOST_DEVICE inline float4 operator-( const float4 & a, const float4 & b )
{
	return make_float4( a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w );
}

HOST_DEVICE inline float4 operator*( const float4 & a, const float4 & b )
{
	return make_float4( a.y * b.z - a.z * b.y,
		  	  	  	    a.z * b.x - a.x * b.z,
		  	  	  	    a.x * b.y - a.y * b.x,
		  	  	  	    0.0f );
}

HOST_DEVICE inline float4 operator*( const float4 & a, const float & b )
{
	return make_float4( a.x * b, a.y * b, a.z * b, a.w * b );
}

HOST_DEVICE inline float dot( const float4 & a, const float4 & b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

HOST_DEVICE inline float length( const float4 & a )
{
	return sqrtf( a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w );
}

HOST_DEVICE inline void normalize( float4 & v )
{
	float l = length( v );
	v.x = v.x / l;
	v.y = v.y / l;
	v.z = v.z / l;
	v.w = v.w / l;
}

HOST_DEVICE inline float4 reflect( const float4 & v, const float4 & normal )
{
	return v - normal * 2.0f * dot( v, normal ) ;
}

struct Ray
{
	float4 		m_start_point;
	float4 		m_vector;
	HOST_DEVICE Ray(){}
	HOST_DEVICE Ray( const float4 & end_point, const float4 & start_point )
		: m_vector( end_point - start_point ),
		  m_start_point( start_point )
	{
		normalize( m_vector );
	}
	HOST_DEVICE void operator()( const float4 & end_point, const float4 & start_point )
	{
		m_vector = end_point - start_point;
		m_start_point = start_point;
		normalize( m_vector );
	}
	HOST_DEVICE float4 point( const float & t ) const
	{
		const float & x0 = m_start_point.x;
		const float & y0 = m_start_point.y;
		const float & z0 = m_start_point.z;
		const float & w0 = m_start_point.w;
		const float & alfa  = m_vector.x;
		const float & beta  = m_vector.y;
		const float & gamma = m_vector.z;
		const float & delta = m_start_point.w;
		return make_float4( x0 + alfa * t,
				            y0 + beta * t,
			                z0 + gamma * t,
			                w0 + delta * t );
	}
	HOST_DEVICE ~Ray(){}
} __attribute__ ( ( aligned( 16 ) ) );


struct Intersection
{
    float4  	point;
    int     	at_object;
    float4  	normal;
    Ray 	    reflect_ray;
    Ray 		refract_ray;
    float   	reflect_amount;
    float   	pixel;
} __attribute__ ( ( aligned( 16 ) ) );


struct Material
{
	float4 		m_ambient;
	float4 		m_diffuse;
	float4 		m_specular;
	float 		m_beta;
	float 		m_phong;
	float 		m_refract_amount;
	float 		m_refract_coef;
} __attribute__ ( ( aligned( 16 ) ) );

/*
struct Object
{
	Material 	m_material;
	virtual ~Object() {}
	void get_reflect_refract_rays( const Ray & ray, Intersection & intersection )
	{
		float4 i = ray.m_vector;
		float4 n = intersection.normal;

		intersection.reflect_ray.m_vector = reflect( i, n );
		normalize( intersection.reflect_ray.m_vector );
		intersection.reflect_ray.m_start_point = intersection.point;
		intersection.reflect_amount = 1.0f;

		if( m_material.m_refract_amount > 0.0f ){
			float refract_coef = m_material.m_refract_coef;
			float cos_i = -dot( i, n );
			if( cos_i < 0.0f ){
				n = n * -1.0f;
				cos_i = -dot( i, n );
				refract_coef = 1.0f / refract_coef;
			}
			float sin2_t = refract_coef * refract_coef * ( 1.0f - cos_i*cos_i );
			if( sin2_t <= 1.0f ){
				float cos_t = sqrt( 1.0f - sin2_t );
				intersection.refract_ray.m_vector = i * refract_coef  + n * ( refract_coef * cos_i - cos_t );
				normalize( intersection.refract_ray.m_vector );
				intersection.refract_ray.m_start_point = intersection.point;
				float Rorto  = ( cos_i - refract_coef * cos_t ) / ( cos_i + refract_coef * cos_t );
				float Rparal = ( refract_coef * cos_i - cos_t ) / ( refract_coef * cos_i + cos_t );
				intersection.reflect_amount = ( Rorto * Rorto + Rparal * Rparal ) / 2.0f;
			}
			else{
				intersection.refract_ray = intersection.reflect_ray;
			}
		}

	}
	bool is_in_border( const float & v, const float & b1, const float & b2, float & t ){
		t = ( v - b1 ) / ( b2 - b1 );
		t = t > 1.0f ? 1.0f : t;
		if( v >= b1 && v <= b2 )// || ( v >= b2 && v <= b1 ) )
				return true;
		if( fabs( v - b1 ) < EPSILON || fabs( v - b2 ) < EPSILON )
			return true;
		return false;
	}
	virtual bool RayIntersect( const Ray & ray, Intersection & intersection ) = 0;
};
*/

