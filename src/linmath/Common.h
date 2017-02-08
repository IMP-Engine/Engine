#ifndef _chag_linmath_Common_h
#define _chag_linmath_Common_h


namespace chag
{

/**
 * Specialized for the matrix types to contruct identity matrix.
 */
template<typename T> 
const T make_identity();
template <typename T>
const T make_rotation_x(float angle);
template <typename T>
const T make_rotation_y(float angle);
template <typename T>
const T make_rotation_z(float angle);
template <typename T, typename U>
const T make_rotation(const U& axis, float angle);
template <typename T>
const T make_matrix(const float *e);
template <typename T, typename U>
const T make_scale(const U& scale);


class float3x3;
class float4x4;

template <typename T>
class SmallVector3;
typedef SmallVector3<float> float3;
typedef SmallVector3<int> int3;

template <typename T>
class SmallVector2;
typedef SmallVector2<float> float2;
typedef SmallVector2<int> int2;

template <typename T>
class SmallVector4;
typedef SmallVector4<float> float4;
typedef SmallVector4<int> int4;



template <typename S, typename T>
inline const S lerp(const S &a, const S &b, const T &t)
{
  return a + (b - a) * t;
}


} // namespace chag

#endif // _chag_linmath_Common_h
