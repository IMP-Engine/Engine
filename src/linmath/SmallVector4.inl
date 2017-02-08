#ifndef _chag_SmallVector4_inl
#define _chag_SmallVector4_inl


#include "SmallVector4.h"

// 
namespace chag
{


template <typename T>
inline T& SmallVector4<T>::operator [] (size_t i)
{  
  return *(&x + i); 
}



template <typename T>
inline const T& SmallVector4<T>::operator [] (size_t i) const
{  
  return *(&x + i); 
}



template <typename T>
inline bool SmallVector4<T>::operator == (const SmallVector4<T>& v) const
{ 
  return (v.x == x) && (v.y == y) && (v.z == z) && (v.w == w); 
}



template <typename T>
inline bool SmallVector4<T>::operator != (const SmallVector4<T>& v) const 
{ 
  return !(v == *this); 
}



template <typename T>
inline const SmallVector4<T> SmallVector4<T>::operator - () const
{ 
  return make_vector<T>(-x, -y, -z, -w); 
}




template <typename T>
inline const SmallVector4<T>& SmallVector4<T>::operator += (const SmallVector4<T>& v)
{
  x += v.x; 
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}



template <typename T>
inline const SmallVector4<T>& SmallVector4<T>::operator -= (const SmallVector4<T>& v)
{
  x -= v.x; 
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector4<T>& SmallVector4<T>::operator *= (const S &s)
{
  x *= s; 
  y *= s;
  z *= s;
  w *= s;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector4<T>& SmallVector4<T>::operator += (const S &s)
{
  x += s; 
  y += s;
  z += s;
  w += s;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector4<T>& SmallVector4<T>::operator /= (const S& s)
{
  x /= s; 
  y /= s;
  z /= s;
  w /= s;
  return *this;
}



template <typename T>
inline const SmallVector4<T> SmallVector4<T>::operator + (const SmallVector4<T>& v) const 
{
  return make_vector<T>(x + v.x, y + v.y, z + v.z, w + v.w);
}



template <typename T>
inline const SmallVector4<T> SmallVector4<T>::operator - (const SmallVector4<T>& v) const 
{
  return make_vector<T>(x - v.x, y - v.y, z - v.z, w - v.w);
}



template <typename T>
template <typename S>
inline const SmallVector4<T> SmallVector4<T>::operator * (const S& s) const
{
  return make_vector<T>(x * s, y * s, z * s, w * s);
}



template <typename T>
template <typename S>
inline const SmallVector4<T> SmallVector4<T>::operator / (const S& s) const
{
  return (*this) * (1.0f / s);
}



template <typename T>
inline const T dot(const SmallVector4<T>& a, const SmallVector4<T>& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}



template <typename T>
inline const T dot3(const SmallVector4<T>& a, const SmallVector4<T>& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}



template <typename T>
inline const T dot3(const SmallVector4<T>& a, const SmallVector3<T>& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}



template <typename T>
inline const T length(const SmallVector4<T> &v)
{
  return (T)sqrt(dot(v,v));
}



template <typename T>
inline const T lengthSquared(const SmallVector4<T> &v)
{
  return dot(v,v);
}



template <typename T>
inline const T length3(const SmallVector4<T> &v)
{
  return (T)sqrt(dot3(v,v));
}


template <typename T>
inline const SmallVector4<T> normalize(const SmallVector4<T>& v)
{
  return v / length(v);
}



template <typename S, typename T>
inline const SmallVector4<T> operator * (const S &s, const SmallVector4<T>& v)
{
  return v * s;
}



template <typename T>
inline const SmallVector4<T> make_vector(const T& x, const T& y, const T& z, const T& w)
{
  SmallVector4<T> r;
  r.x = x;
  r.y = y;
  r.z = z;
  r.w = w;
  return r;
};



template <typename T>
inline const SmallVector4<T> make_vector4(const T *v)
{
  return make_vector(v[0], v[1], v[2], v[3]);
};


template <typename T>
inline const SmallVector4<T> make_vector4(const SmallVector3<T> &xyz, const T &w)
{
  return make_vector(xyz.x, xyz.y, xyz.z, w);
};



template <typename T>
const SmallVector4<T> min(const SmallVector4<T> &a, const SmallVector4<T> &b)
{
  return make_vector(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
}



template <typename T>
const SmallVector4<T> max(const SmallVector4<T> &a, const SmallVector4<T> &b)
{
  return make_vector(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
}



template <typename T>
const SmallVector3<T> cross3(const SmallVector4<T> &a, const SmallVector4<T> &b)
{
  return make_vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}



#if 0
template <typename T>
const SmallVector3<T> cross(const SmallVector4<T> &a, const SmallVector4<T> &b)
{
  return make_vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}
#endif


} // namespace chag


#endif // _chag_SmallVector4_inl
