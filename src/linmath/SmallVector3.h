#ifndef _chag_SmallVector3_h
#define _chag_SmallVector3_h

#include <math.h>
#include <cstddef>


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
class SmallVector3
{
public:
  T x;
  T y;
  T z;

  // Indexation
  T& operator [] (const size_t i);
  const T& operator [] (const size_t i) const;
 
  bool operator == (const SmallVector3& v) const;
  bool operator != (const SmallVector3& v) const;
  
  // Negation
  const SmallVector3 operator - () const;

  // Assignement 
  const SmallVector3& operator = (const SmallVector3& v);
  
  const SmallVector3& operator += (const SmallVector3& v);
  
  const SmallVector3& operator -= (const SmallVector3& v);
  
  template <typename S>
  const SmallVector3& operator *= (const S& s);

  template <typename S>
  const SmallVector3& operator /= (const S& s);

  const SmallVector3 operator + (const SmallVector3& v) const;
  const SmallVector3 operator + (const T &s) const;
  const SmallVector3 operator - (const SmallVector3& v) const;
  const SmallVector3 operator - (const T &s) const;
  
  template <typename S>
  const SmallVector3 operator * (const S& s) const;
  const SmallVector3 operator * (const SmallVector3& v) const;

  template <typename S>
  const SmallVector3 operator / (const S& s) const;
  const SmallVector3 operator / (const SmallVector3& v) const;
};



template <typename T>
const T dot(const SmallVector3<T>& a, const SmallVector3<T>& b);



template <typename T>
const T length(const SmallVector3<T>& v);



template <typename T>
const T lengthSquared(const SmallVector3<T> &v);



template <typename T>
const SmallVector3<T> normalize(const SmallVector3<T>& v);



template <typename S, typename T>
const SmallVector3<T> operator * (const S &s, const SmallVector3<T>& v);



template <typename T>
const SmallVector3<T> make_vector(const T& x, const T& y, const T& z);



template <typename T>
const SmallVector3<T> make_vector3(const T *v);



template <typename T>
const SmallVector3<T> min(const SmallVector3<T> &a, const SmallVector3<T> &b);



template <typename T>
const SmallVector3<T> max(const SmallVector3<T> &a, const SmallVector3<T> &b);



template <typename T>
const SmallVector3<T> cross(const SmallVector3<T> &a, const SmallVector3<T> &b);



template <typename T>
const SmallVector3<T> perpendicular(const SmallVector3<T> &v);




} // namespace chag

#include "SmallVector3.inl"

#endif // _chag_SmallVector3_h
