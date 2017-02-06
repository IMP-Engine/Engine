#ifndef _chag_SmallVector4_h
#define _chag_SmallVector4_h

#include <math.h>
#include "SmallVector3.h"

#ifdef min
  #undef min
#endif
#ifdef max
  #undef max
#endif

// 
namespace chag
{

template <typename T>
class SmallVector4
{
public:
  T x;
  T y;
  T z;
  T w;

  // Indexation
  T& operator [] (const size_t i);
  const T& operator [] (const size_t i) const;
 
  bool operator == (const SmallVector4& v) const;
  bool operator != (const SmallVector4& v) const;
  
  // Negation
  const SmallVector4 operator - () const;
  
  const SmallVector4& operator += (const SmallVector4& v);
  
  const SmallVector4& operator -= (const SmallVector4& v);
  
  template <typename S>
  const SmallVector4& operator *= (const S& s);

  template <typename S>
  const SmallVector4& operator += (const S& s);

  template <typename S>
  const SmallVector4& operator /= (const S& s);

  const SmallVector4 operator + (const SmallVector4& v) const;
  const SmallVector4 operator - (const SmallVector4& v) const;
  
  template <typename S>
  const SmallVector4 operator * (const S& s) const;

  template <typename S>
  const SmallVector4 operator / (const S& s) const;

};



template <typename T>
const T dot(const SmallVector4<T>& a, const SmallVector4<T>& b);



template <typename T>
const T dot3(const SmallVector4<T>& a, const SmallVector4<T>& b);



template <typename T>
const T length(const SmallVector4<T>& v);



template <typename T>
const T lengthSquared(const SmallVector4<T> &v);



template <typename T>
const T length3(const SmallVector4<T>& v);



template <typename T>
const SmallVector4<T> normalize(const SmallVector4<T>& v);



template <typename S, typename T>
const SmallVector4<T> operator * (const S &s, const SmallVector4<T>& v);



template <typename T>
const SmallVector4<T> make_vector(const T& x, const T& y, const T& z, const T& w);



template <typename T>
const SmallVector4<T> make_vector4(const T *v);


template <typename T>
const SmallVector4<T> make_vector4(const SmallVector3<T> &xyz, const T &w);



template <typename T>
const SmallVector4<T> min(const SmallVector4<T> &a, const SmallVector4<T> &b);



template <typename T>
const SmallVector4<T> max(const SmallVector4<T> &a, const SmallVector4<T> &b);



template <typename T>
const SmallVector3<T> cross3(const SmallVector4<T> &a, const SmallVector4<T> &b);



template <typename T>
inline const SmallVector3<T> make_vector3(const SmallVector4<T> &v) { return make_vector(v.x, v.y, v.z); }


} // namespace chag

#include "SmallVector4.inl"

#endif // _chag_SmallVector4_h
