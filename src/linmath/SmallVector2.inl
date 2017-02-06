#ifndef _chag_SmallVector2_inl
#define _chag_SmallVector2_inl


#include "SmallVector2.h"

// 
namespace chag
{


template <typename T>
inline T& SmallVector2<T>::operator [] (size_t i)
{  
  return *(&x + i); 
}



template <typename T>
inline const T& SmallVector2<T>::operator [] (size_t i) const
{  
  return *(&x + i); 
}



template <typename T>
inline const bool SmallVector2<T>::operator == (const SmallVector2<T>& v) const
{ 
  return (v.x == x) && (v.y == y); 
}



template <typename T>
inline const bool SmallVector2<T>::operator != (const SmallVector2<T>& v) const 
{ 
  return !(v == *this); 
}



template <typename T>
inline const SmallVector2<T> SmallVector2<T>::operator - () const
{ 
  return make_vector<T>(-x, -y); 
}



template <typename T>
inline const SmallVector2<T>& SmallVector2<T>::operator = (const SmallVector2<T>& v)
{
  x = v.x; 
  y = v.y;
  return *this;
}



template <typename T>
inline const SmallVector2<T>& SmallVector2<T>::operator += (const SmallVector2<T>& v)
{
  x += v.x; 
  y += v.y;
  return *this;
}



template <typename T>
inline const SmallVector2<T>& SmallVector2<T>::operator -= (const SmallVector2<T>& v)
{
  x -= v.x; 
  y -= v.y;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector2<T>& SmallVector2<T>::operator *= (const S &s)
{
  x *= s; 
  y *= s;
  return *this;
}



template <typename T>
template <typename S>
inline const SmallVector2<T>& SmallVector2<T>::operator /= (const S& s)
{
  x /= s; 
  y /= s;
  return *this;
}



template <typename T>
inline const SmallVector2<T> SmallVector2<T>::operator + (const SmallVector2<T>& v) const 
{
  return make_vector<T>(x + v.x, y + v.y);
}



template <typename T>
inline const SmallVector2<T> SmallVector2<T>::operator - (const SmallVector2<T>& v) const 
{
  return make_vector<T>(x - v.x, y - v.y);
}



template <typename T>
template <typename S>
inline const SmallVector2<T> SmallVector2<T>::operator * (const S& s) const
{
  return make_vector<T>(x * s, y * s);
}



template <typename T>
template <typename S>
inline const SmallVector2<T> SmallVector2<T>::operator / (const S& s) const
{
  T r = 1.0f / s;
  return make_vector<T>(x * r, y * r);
}



template <typename T>
inline const T dot(const SmallVector2<T>& a, const SmallVector2<T>& b)
{
  return a.x * b.x + a.y * b.y;
}



template <typename T>
inline const T length(const SmallVector2<T> &v)
{
  return (T)sqrt(dot(v,v));
}



template <typename T>
inline const T lengthSquared(const SmallVector2<T> &v)
{
  return dot(v,v);
}


template <typename T>
inline const SmallVector2<T> normalize(const SmallVector2<T>& v)
{
  return v / length(v);
}



template <typename S, typename T>
inline const SmallVector2<T> operator * (const S &s, const SmallVector2<T>& v)
{
  return v * s;
}



template <typename T>
inline const SmallVector2<T> make_vector(const T& x, const T& y)
{
  SmallVector2<T> r = { x, y };
  return r;
};



template <typename T>
inline const SmallVector2<T> make_vector2(const T *v)
{
  SmallVector2<T> r = { v[0], v[1] };
  return r;
};



template <typename T>
const SmallVector2<T> min(const SmallVector2<T> &a, const SmallVector2<T> &b)
{
  return make_vector(std::min(a.x, b.x), std::min(a.y, b.y));
}



template <typename T>
const SmallVector2<T> max(const SmallVector2<T> &a, const SmallVector2<T> &b)
{
  return make_vector(std::max(a.x, b.x), std::max(a.y, b.y));
}



} // namespace chag


#endif // _chag_SmallVector2_inl
