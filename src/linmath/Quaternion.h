#ifndef _chag_Quaternion_h
#define _chag_Quaternion_h

#include "float3.h"
#include "float4x4.h"



namespace chag
{
class Quaternion
{
public:
  float3 v;
  float w;
  // The default constructor creates an identity quaternion
  Quaternion():v(), w(1.0) {};  
  Quaternion(float3 mv, float mw):v(mv), w(mw) {};

  const Quaternion operator + (const Quaternion& q) const
  {
    return Quaternion(v + q.v, w + q.w);
  };  

  const Quaternion operator * (const Quaternion& q) const
  {
    return Quaternion(cross(v, q.v) + q.w * v + w * q.v, 
      w * q.w - dot(v, q.v));
  }

  const Quaternion operator * (const float& f) const
  {
    return Quaternion(v * f, w * f);
  }

  // Conjugate
  const Quaternion conj() const
  {
    return Quaternion(-v, w);
  }


};

/**
 */
Quaternion make_quaternion_axis_angle(const float3 &axis, float angle);

/**
 */
Quaternion make_quaternion(const float3x3 &rot);

/**
 */
Quaternion make_quaternion(const float4x4 &rot);


// Norm
const float n(const Quaternion& q); 
// Inverse
const Quaternion inv(const Quaternion& q);

// Creating a matrix from a quaternion
const float3x3 toMatrix3x3(const Quaternion& q);
const float4x4 makematrix(const Quaternion& q);

const Quaternion slerp(Quaternion q, Quaternion r, float t);

} // namespace chag

#endif // _chag_Quaternion_h
