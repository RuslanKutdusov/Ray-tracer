#include "Vector.hpp"

#define SSE_DOT 	0
#define SSE_ADD_SUB 0//win!!
#define SSE_MUL		0
#define SSE_SCALAR	0
#define SSE_LENGTH	0//win!!
#define SSE_DISTANCE 0

Vector::Vector()
	: x( 0.0f ), y( 0.0f ), z( 0.0f ), w( 0.0f )
{
}

#if SSE
Vector::Vector( const __m128 & v )
	: x( 0.0f ), y( 0.0f ), z( 0.0f ), w( 0.0f )
{
	*(__m128*)this = v;
}
#endif

Vector::Vector(const float & x_, const float & y_, const float & z_)
	: x(x_), y(y_), z(z_), w( 0.0f )
{

}

float Vector::dot(const Vector& v) const
{
#if SSE_DOT
	__m128* a = (__m128*)this;
	__m128* b = (__m128*)&v;
	float r;
	asm("movaps 	(%1), %%xmm0;\n"
		"movaps		(%2), %%xmm1;\n"

		"mulps		%%xmm1, %%xmm0;\n"

		"haddps		%%xmm0, %%xmm0;\n"
		"haddps		%%xmm0, %%xmm0;\n"

		"movss		%%xmm0, (%0);\n"

		:
		: "r"(&r), "r"(a), "r"(b)
		: "%xmm0", "%xmm1"
	);
	return r;
#else
	return x * v.x + y * v.y + z * v.z;
#endif
}

Vector Vector::operator+(const Vector& v) const
{
#if SSE_ADD_SUB
	return Vector( _mm_add_ps( *(__m128*)this, *(__m128*)&v ) );
#else
	return Vector(x + v.x, y + v.y, z + v.z);
#endif
}

Vector Vector::operator-(const Vector& v) const
{
#if SSE_ADD_SUB
	return Vector( _mm_sub_ps( *(__m128*)this, *(__m128*)&v ) );
#else
	return Vector(x - v.x, y - v.y, z - v.z);
#endif
}

Vector Vector::operator*(const Vector& v) const
{
#if SSE_MUL
	__m128* a = (__m128*)this;
	__m128* b = (__m128*)&v;
	__m128 r;
	__m128 * rp = &r;
	asm("movaps 	(%0), %%xmm0;\n"
		"movaps		(%1), %%xmm1;\n"
		"movaps		(%0), %%xmm2;\n"
		"movaps		(%1), %%xmm3;\n"

		"shufps     $0xc9, %%xmm0, %%xmm0;\n"//v1 yzx
		"shufps		$0xd2, %%xmm1, %%xmm1;\n"//v2 zxy
		"mulps		%%xmm1, %%xmm0;\n"

		"shufps     $0xd2, %%xmm2, %%xmm2;\n"//v1 zxy
		"shufps     $0xc9, %%xmm3, %%xmm3;\n"//v2 yzx
		"mulps		%%xmm3, %%xmm2;\n"

		"subps		%%xmm2, %%xmm0;\n"
		"movaps		%%xmm0, (%2);\n"

		:
		: "r"(a), "r"(b), "r"(rp)
		: "%xmm0", "%xmm1", "%mm2", "%xmm3"
	);
	return Vector(r);
#else
	return Vector(y * v.z - z * v.y,
			  	  z * v.x - x * v.z,
				  x * v.y - y * v.x);
#endif
}

Vector Vector::scalar(const float & s) const
{
#if SSE_SCALAR
	return Vector( _mm_mul_ps( *(__m128*)this, _mm_set1_ps(s) ) );
#else
	return Vector(x * s, y * s, z * s);
#endif
}


float Vector::length() const
{
#if SSE_LENGTH
	asm("movaps 	(%0), %%xmm0;\n"

		"mulps		%%xmm0, %%xmm0;\n"

		"haddps		%%xmm0, %%xmm0;\n"
		"haddps		%%xmm0, %%xmm0;\n"

		"sqrtss		%%xmm0, %%xmm0;\n"
		"retq;\n"

		:
		: "r"( (__m128*)this )
		: "%xmm0"
	);
#else
	return sqrt(x * x + y * y + z * z);
#endif
}


float Vector::distance(const Vector & v)const
{
#if SSE_DISTANCE
	asm("movaps		(%0), %%xmm0;\n"
		"movaps		(%1), %%xmm1;\n"

		"subps		%%xmm1, %%xmm0;\n"
		"mulps		%%xmm0, %%xmm0;\n"

		"haddps		%%xmm0, %%xmm0;\n"
		"haddps		%%xmm0, %%xmm0;\n"

		"sqrtss		%%xmm0, %%xmm0;\n"

		"ret;\n"

		:
		: "r"((__m128*)this), "r"((__m128*)&v)
		: "%xmm0", "%xmm1"
	);
#else
	return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
#endif
}

Vector Vector::reflect(const Vector& normal) const{
	return *this - normal.scalar(2 * dot(normal));
}

void Vector::normalize(){
#if SSE_LENGTH
	asm("movaps 	(%0), %%xmm0;\n"
		"movaps 	(%0), %%xmm1;\n"

		"mulps		%%xmm0, %%xmm0;\n"

		"haddps		%%xmm0, %%xmm0;\n"
		"haddps		%%xmm0, %%xmm0;\n"

		"sqrtss		%%xmm0, %%xmm0;\n"//length

		"shufps		$0x00, %%xmm0, %%xmm0;\n"

		"divps		%%xmm0, %%xmm1;\n"
		"movaps		%%xmm1, (%0);\n"

		:
		: "r"( (__m128*)this )
		: "%xmm0"
	);
#else
	float l = length();
	x = x / l;
	y = y / l;
	z = z / l;
#endif
}

Vector Vector::move(const Vector & v) const{
	return Vector(x + v.x, y + v.y, z + v.z);
}
