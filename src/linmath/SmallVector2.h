#ifndef _chag_SmallVector2_h
#define _chag_SmallVector2_h

#include <math.h>
#include <algorithm>

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
class SmallVector2
{
public:
  T x;
  T y;

  // Indexation
  T& operator [] (const size_t i);
  const T& operator [] (const size_t i) const;
 
  const bool operator == (const SmallVector2& v) const;
  const bool operator != (const SmallVector2& v) const;
  
  // Negation
  const SmallVector2 operator - () const;

  // Assignement 
  const SmallVector2& operator = (const SmallVector2& v);
  
  const SmallVector2& operator += (const SmallVector2& v);
  
  const SmallVector2& operator -= (const SmallVector2& v);
  
  template <typename S>
  const SmallVector2& operator *= (const S& s);

  template <typename S>
  const SmallVector2& operator /= (const S& s);

  const SmallVector2 operator + (const SmallVector2& v) const;
  const SmallVector2 operator - (const SmallVector2& v) const;
  
  template <typename S>
  const SmallVector2 operator * (const S& s) const;

  template <typename S>
  const SmallVector2 operator / (const S& s) const;
};



template <typename T>
const T dot(const SmallVector2<T>& a, const SmallVector2<T>& b);



template <typename T>
const T length(const SmallVector2<T>& v);



template <typename T>
const T lengthSquared(const SmallVector2<T> &v);



template <typename T>
const SmallVector2<T> normalize(const SmallVector2<T>& v);



template <typename S, typename T>
const SmallVector2<T> operator * (const S &s, const SmallVector2<T>& v);



template <typename T>
const SmallVector2<T> make_vector(const T& x, const T& y);



template <typename T>
const SmallVector2<T> make_vector2(const T *v);



template <typename T>
const SmallVector2<T> min(const SmallVector2<T> &a, const SmallVector2<T> &b);



template <typename T>
const SmallVector2<T> max(const SmallVector2<T> &a, const SmallVector2<T> &b);


} // namespace chag

#include "SmallVector2.inl"

#endif // _chag_SmallVector2_h
