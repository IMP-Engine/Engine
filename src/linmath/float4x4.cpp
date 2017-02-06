#include "float4x4.h"
#include "float3x3.h"
#include "inverse.h"

#include <cassert>
#include <memory.h>
#ifndef M_PI
#define M_PI 3.14159265f
#endif
namespace chag
{



float4& float4x4::operator [] (const size_t i)
{ 
  assert( i < 3 );
  return *(&c1 + i);
}



const float4& float4x4::operator [] (const size_t i) const
{ 
	assert( i < 3 );
  return *(&c1 + i);
}



bool float4x4::operator == (const float4x4& m) const
{ 
  return (m.c1 == c1) && (m.c2 == c2) && (m.c3 == c3) && (m.c4 == c4); 
}



bool float4x4::operator != (const float4x4& m) const 
{ 
  return !(m == *this); 
}



const float4 float4x4::row(int i) const
{
  assert( i < 4 );

  switch(i)
  {
  case 0: 
    return make_vector(c1[0], c2[0], c3[0], c4[0]);
  case 1: 
    return make_vector(c1[1], c2[1], c3[1], c4[1]);
  case 2: 
    return make_vector(c1[2], c2[2], c3[2], c4[2]);
  /*case 3*/
  default: 
    return make_vector(c1[3], c2[3], c3[3], c4[3]);
  }
}



const float4x4 float4x4::operator + (const float4x4& m)
{
  return make_matrix(c1 + m.c1, 
		  c2 + m.c2, 
		  c3 + m.c3, 
		  c4 + m.c4);
}



const float4x4 float4x4::operator * (const float& s) const
{
  return make_matrix(s * c1, s * c2, s * c3, s * c4);
}



const float4 float4x4::operator * (const float4& v) const
{
  return make_vector(c1[0] * v.x + c2[0] * v.y + c3[0] * v.z + c4[0] * v.w, 
				c1[1] * v.x + c2[1] * v.y + c3[1] * v.z + c4[1] * v.w, 
				c1[2] * v.x + c2[2] * v.y + c3[2] * v.z + c4[2] * v.w, 
				c1[3] * v.x + c2[3] * v.y + c3[3] * v.z + c4[3] * v.w);
}



const float4x4 float4x4::operator * (const float4x4& b) const
{
#if 1
  // a textbook implementation...
  float4x4 c;
  const float4x4 &a = *this;

  for (int i = 1; i <= 4; ++i)
  {
      for (int k = 1; k <= 4; ++k)
      {
        c(i, k) = a(i, 1) * b(1, k) 
                + a(i, 2) * b(2, k) 
                + a(i, 3) * b(3, k) 
                + a(i, 4) * b(4, k);
      }
  }

  return c;
#else
  // assuredly slow, since row() does a lot of logic...
  return make_matrix(dot(row(0), m.c1), dot(row(0), m.c2), dot(row(0), m.c3), dot(row(0), m.c4),  
		  dot(row(1), m.c1), dot(row(1), m.c2), dot(row(1), m.c3), dot(row(1), m.c4), 
		  dot(row(2), m.c1), dot(row(2), m.c2), dot(row(2), m.c3), dot(row(2), m.c4), 
		  dot(row(3), m.c1), dot(row(3), m.c2), dot(row(3), m.c3), dot(row(3), m.c4));
#endif
}



template <>
const float4x4 make_identity<float4x4>()
{
  float4x4 m = 
  { 
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
  };
  return m;
}

template<>
const float4x4 make_scale<float4x4>(const float3 &s)
{
  float4x4 m = 
  { 
    { s.x, 0.0f, 0.0f, 0.0f },
    { 0.0f, s.y, 0.0f, 0.0f },
    { 0.0f, 0.0f, s.z, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f}
  };
  return m;	
}



template<>
const float4x4 make_scale<float4x4>(const float &s)
{
  float4x4 m = 
  { 
    { s,    0.0f, 0.0f, 0.0f },
    { 0.0f,    s, 0.0f, 0.0f },
    { 0.0f, 0.0f,    s, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f}
  };
  return m;	
}



template <>
const float4x4 make_matrix<float4x4>(const float *e)
{
  float4x4 m;
  memcpy(&m, e, sizeof(m));
  return m;
}



const float4x4 make_matrix(
  float m11, float m12, float m13, float m14, 
  float m21, float m22, float m23, float m24, 
  float m31, float m32, float m33, float m34,
  float m41, float m42, float m43, float m44)
{
  float4x4 m = 
  { 
    /*c1*/{m11, m21, m31, m41}, 
    /*c2*/{m12, m22, m32, m42}, 
    /*c3*/{m13, m23, m33, m43}, 
    /*c4*/{m14, m24, m34, m44} 
  };
  return m;
}

   
const float4x4 make_matrix(const float4& c1, const float4& c2, const float4& c3, const float4& c4)
{
  float4x4 m = { c1, c2, c3, c4 };
  return m;
}



const float4x4 make_matrix(const float3x3 &r, const float3 &pos)
{
  float4x4 m = 
  { 
    { r.c1.x, r.c1.y, r.c1.z, 0.0f }, 
    { r.c2.x, r.c2.y, r.c2.z, 0.0f },
    { r.c3.x, r.c3.y, r.c3.z, 0.0f },
    { pos.x,  pos.y,  pos.z,  1.0f }
  };
  return m;
}

// Equivalent to glFrustum
const float4x4 make_frustum(float left, float right, float bottom, 
							float top, 	float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	float4x4 m = 
	{ 
		/*c1*/{temp / temp2, 0.0f, 0.0f, 0.0f}, 
		/*c2*/{0.0f, temp / temp3, 0.0f, 0.0f}, 
		/*c3*/{(right + left) / temp2, (top + bottom) / temp3, (-zfar - znear) / temp4, -1.0f}, 
		/*c4*/{0.0f, 0.0f, (-temp * zfar) / temp4, 0.0f} 
	};
	return m;
}
const float4x4 make_frustum_inv(float left, float right, float bottom, 
								float top, 	float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	float4x4 m = 
	{ 
		/*c1*/{temp2 / temp, 0.0f, 0.0f, 0.0f}, 
		/*c2*/{0.0f, temp3 / temp, 0.0f, 0.0f}, 
		/*c3*/{0.0f, 0.0f, 0.0f, -temp4 / (temp*zfar)}, 
		/*c4*/{(right+left)/temp, (top+bottom)/temp, -1.0f, (zfar + znear)/(temp*zfar)} 
	};
	return m;
}

// Equivalent to gluPerspective
const float4x4 make_perspective(float fov, float aspect_ratio, float near, float far)
{
	float ymax = near * tanf(fov * M_PI / 360.0f);
	float xmax = ymax * aspect_ratio;
	return make_frustum(-xmax, xmax, -ymax, ymax, near, far);	
}
const float4x4 make_perspective_inv(float fov, float aspect_ratio, float near, float far)
{
	float ymax = near * tanf(fov * M_PI / 360.0f);
	float xmax = ymax * aspect_ratio;
	return make_frustum_inv(-xmax, xmax, -ymax, ymax, near, far);	
}

// Equivalent to glOrtho
const float4x4 make_ortho(float r, float l, float t, float b, float f, float n)
{
	float4x4 m = 
	{
		/*c1*/{2.0f/(r - l), 0.0f, 0.0f, 0.0f},
		/*c2*/{0.0f, 2.0f/(t - b), 0.0f, 0.0f},
		/*c3*/{0.0f, 0.0f, -2.0f/(f - n), 0.0f},
		/*c4*/{-((r+l)/(r-l)), -((t+b)/(t-b)), -((f+n)/(f-n)), 1.0f}
	};
	return m;
}
const float4x4 make_ortho_inv(float r, float l, float t, float b, float f, float n)
{
	float4x4 m = 
	{
		/*c1*/{(r - l)/2.0f, 0.0f, 0.0f, 0.0f},
		/*c2*/{0.0f, (t - b)/2.0f, 0.0f, 0.0f},
		/*c3*/{0.0f, 0.0f, (f - n)/2.0f, 0.0f},
		/*c4*/{(r+l)/2.0f, (t+b)/2.0f, (f+n)/2.0f, 1.0}
	};
	return m;
}
// Equivalent to gluOrtho2d
const float4x4 make_ortho2d(float r, float l, float t, float b)
{
	return make_ortho(r, l, t, b, -1.0f, 1.0f);
}


const float4x4 make_translation(const float3 &pos)
{
  float4x4 m = make_identity<float4x4>();
  m.c4 = make_vector(pos.x, pos.y, pos.z, 1.0f);
  return m;
}




const float4x4 make_matrix_from_zAxis(const float3 &pos, const float3 &zAxis, const float3 &yAxis)
{
  float3 z = normalize(zAxis);
  float3 x = normalize(cross(yAxis, z));
  float3 y = cross(z, x);
  
  float4x4 m = 
  { 
    { x.x, x.y, x.z, 0.0f }, 
    { y.x, y.y, y.z, 0.0f },
    { z.x, z.y, z.z, 0.0f },
    { pos.x,  pos.y,  pos.z,  1.0f }
  };
  return m;
}



const float4x4 make_matrix_from_yAxis(const float3 &pos, const float3 &yAxis, const float3 &zAxis)
{
  float3 y = normalize(yAxis);
  float3 x = normalize(cross(y, zAxis));
  float3 z = cross(x, y);
  
  float4x4 m = 
  { 
    { x.x, x.y, x.z, 0.0f }, 
    { y.x, y.y, y.z, 0.0f },
    { z.x, z.y, z.z, 0.0f },
    { pos.x,  pos.y,  pos.z,  1.0f }
  };
  return m;
}



template <>
const float4x4 make_rotation_x<float4x4>(float angle)
{
  return make_matrix(make_rotation_x<float3x3>(angle), make_vector(0.0f, 0.0f, 0.0f));
}



template <>
const float4x4 make_rotation_y<float4x4>(float angle)
{
  return make_matrix(make_rotation_y<float3x3>(angle), make_vector(0.0f, 0.0f, 0.0f));
}



template <>
const float4x4 make_rotation_z<float4x4>(float angle)
{
  return make_matrix(make_rotation_z<float3x3>(angle), make_vector(0.0f, 0.0f, 0.0f));
}



template <>
const float4x4 make_rotation<float4x4>(const float3& axis, float angleRad)
{
  return make_matrix(make_rotation<float3x3>(axis, angleRad), make_vector(0.0f, 0.0f, 0.0f));
}



float determinant(const float4x4 &m)
{
  float3x3 s1 = make_matrix(m.c2[1], m.c3[1], m.c4[1],
			 m.c2[2], m.c3[2], m.c4[2],
			 m.c2[3], m.c3[3], m.c4[3]);
  float3x3 s2 = make_matrix(m.c3[1], m.c4[1], m.c1[1],
			 m.c3[2], m.c4[2], m.c1[2],
			 m.c3[3], m.c4[3], m.c1[3]);
  float3x3 s3 = make_matrix(m.c4[1], m.c1[1], m.c2[1],
			 m.c4[2], m.c1[2], m.c2[2],
			 m.c4[3], m.c1[3], m.c2[3]);
  float3x3 s4 = make_matrix(m.c1[1], m.c2[1], m.c3[1],
			 m.c1[2], m.c2[2], m.c3[2],
			 m.c1[3], m.c2[3], m.c3[3]);
  
  float det1 = determinant(s1);
  float det2 = determinant(s2);
  float det3 = determinant(s3);
  float det4 = determinant(s4);

  return m.c1[0]*det1 - m.c2[0]*det2 + m.c3[0]*det3 - m.c4[0]*det4;
}


const float4x4 transpose(const float4x4 &m)
{
  return make_matrix(m.c1[0], m.c1[1], m.c1[2], m.c1[3], 
    m.c2[0], m.c2[1], m.c2[2], m.c2[3],
    m.c3[0], m.c3[1], m.c3[2], m.c3[3], 
    m.c4[0], m.c4[1], m.c4[2], m.c4[3]);
}



const float4x4 transpose(const float4x4 &m);

const float4x4 inverse(const float4x4 &m); 



// Find the solution v to 
// u = Mv
const float4 cramers(const float4x4 &m, const float4& u)
{
  float4 a = { determinant(make_matrix(u, m.c2, m.c3, m.c4)), 
	   determinant(make_matrix(m.c1, u, m.c3, m.c4)), 
	   determinant(make_matrix(m.c1, m.c2, u, m.c4)), 
     determinant(make_matrix(m.c1, m.c2, m.c3, u)) };

  return (1.0f / determinant(m)) * a;
}



const float4x4 inverse(const float4x4 &m)
{
	float4x4 result; 
	inverse_v1(reinterpret_cast<s_matrix *>(&result), reinterpret_cast<const s_matrix *>(&m)); 
	return result; 
}



const float3 transformPoint(const float4x4 &m, const float3 &p)
{
  float4 r = m * make_vector4(p, 1.0f);
  return make_vector(r.x, r.y, r.z);
}



const float3 transformDirection(const float4x4 &m, const float3 &d)
{
  float4 r = m * make_vector4(d, 0.0f);
  return make_vector(r.x, r.y, r.z);
}


} // namespace chag
