#ifndef _chag_SmallVector3_inl
#define _chag_SmallVector3_inl


#include "SmallVector3.h"
#include <algorithm>

// 
namespace chag
{


template <typename T>
inline T& SmallVector3<T>::operator [] (size_t i)
{  
  return *(&x + i); 
}



template <typename T>
inline const T& SmallVector3<T>::operator [] (size_t i) const
{  
  return *(&x + i); 
}



template <typename T>
inline bool SmallVector3<T>::operator == (const SmallVector3<T>& v) const
{ 
  return (v.x == x) && (v.y == y) && (v.z == z); 
}



template <typename T>
inline bool SmallVector3<T>::operator != (const SmallVector3<T>& v) const 
{ 
  return !(v == *this); 
}



template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator - () const
{ 
  return make_vector<T>(-x, -y, -z); 
}



template <typename T>
inline const SmallVector3<T>& SmallVector3<T>::operator = (const SmallVector3<T>& v)
{
  x = v.x; 
  y = v.y;
  z = v.z;
  return *this;
}



template <typename T>
inline const SmallVector3<T>& SmallVector3<T>::operator += (const SmallVector3<T>& v)
{
  x += v.x; 
  y += v.y;
  z += v.z;
  return *this;
}



template <typename T>
inline const SmallVector3<T>& SmallVector3<T>::operator -= (const SmallVector3<T>& v)
{
  x -= v.x; 
  y -= v.y;
  z -= v.z;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector3<T>& SmallVector3<T>::operator *= (const S &s)
{
  x *= s; 
  y *= s;
  z *= s;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector3<T>& SmallVector3<T>::operator /= (const S& s)
{
  x /= s; 
  y /= s;
  z /= s;
  return *this;
}



template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator + (const SmallVector3<T>& v) const 
{
  return make_vector<T>(x + v.x, y + v.y, z + v.z);
}



template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator + (const T &s) const 
{
  return make_vector<T>(x + s, y + s, z + s);
}


template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator - (const SmallVector3<T>& v) const 
{
  return make_vector<T>(x - v.x, y - v.y, z - v.z);
}


template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator - (const T &s) const 
{
  return make_vector<T>(x - s, y - s, z - s);
}



template <typename T>
template <typename S>
inline const SmallVector3<T> SmallVector3<T>::operator * (const S& s) const
{
  return make_vector<T>(x * s, y * s, z * s);
}



template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator * (const SmallVector3<T>& v) const
{
  return make_vector<T>(x * v.x, y * v.y, z * v.z);
}



template <typename T>
template <typename S>
inline const SmallVector3<T> SmallVector3<T>::operator / (const S& s) const
{
  T r = 1.0f / s;
  return make_vector<T>(x * r, y * r, z * r);
}



template <typename T>
inline const SmallVector3<T> SmallVector3<T>::operator / (const SmallVector3<T>& v) const
{
  return make_vector<T>(x / v.x, y / v.y, z / v.z);
}



template <typename T>
inline const T dot(const SmallVector3<T>& a, const SmallVector3<T>& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}



template <typename T>
inline const T length(const SmallVector3<T> &v)
{
  return (T)sqrt(dot(v,v));
}



template <typename T>
inline const T lengthSquared(const SmallVector3<T> &v)
{
  return dot(v,v);
}



template <typename T>
inline const SmallVector3<T> normalize(const SmallVector3<T>& v)
{
  return v / length(v);
}



template <typename S, typename T>
inline const SmallVector3<T> operator * (const S &s, const SmallVector3<T>& v)
{
  return v * s;
}



template <typename T>
inline const SmallVector3<T> make_vector(const T& x, const T& y, const T& z)
{
  SmallVector3<T> r;// = { x, y, z };
  r.x = x;
  r.y = y;
  r.z = z;
  return r;
};



template <typename T>
inline const SmallVector3<T> make_vector3(const T *v)
{
  SmallVector3<T> r;
  r.x = v[0];
  r.y = v[1];
  r.z = v[2];
  return r;
};



template <typename T>
const SmallVector3<T> min(const SmallVector3<T> &a, const SmallVector3<T> &b)
{
  return make_vector(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}



template <typename T>
const SmallVector3<T> max(const SmallVector3<T> &a, const SmallVector3<T> &b)
{
  return make_vector(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}



template <typename T>
const SmallVector3<T> cross(const SmallVector3<T> &a, const SmallVector3<T> &b)
{
  return make_vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}



template <typename T>
const SmallVector3<T> perpendicular(const SmallVector3<T> &v)
{
  if (fabsf(v.x) < fabsf(v.y))
  {
      return make_vector(0.0f, -v.z, v.y);
  }

  return make_vector(-v.z, 0.0f, v.x);
}


} // namespace chag


#endif // _chag_SmallVector3_inl
