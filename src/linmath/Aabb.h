#ifndef _chag_Aabb_h
#define _chag_Aabb_h

#include "float3.h"
#include "Common.h"


namespace chag
{

/**
 * An aabb defined by the min and max extrema.
 */
class Aabb
{
public:
  float3 min;
  float3 max;

  /**
   */
  const float3 getCentre() const { return (min + max) * 0.5f; }

  /**
   */
  const float3 getHalfSize() const { return (max - min) * 0.5f; }

  /**
   */
  float getVolume() const { float3 d = max - min; return d.x * d.y * d.z; }
};



/**
 */
inline Aabb combine(const Aabb &a, const Aabb &b)
{
  Aabb result = { min(a.min, b.min), max(a.max, b.max) };
  return result;
}

/**
 *
 */
inline Aabb combine(const Aabb &a, const float3 &pt)
{
  Aabb result = { min(a.min, pt), max(a.max, pt) };
  return result;
}
/**
 * creates an aabb that has min = FLT_MAX and max = -FLT_MAX.
 */
Aabb make_inverse_extreme_aabb();


/**
 */
Aabb make_aabb(const float3 &min, const float3 &max);

/**
 */
inline Aabb make_aabb(const float3 &position, const float radius)
{
  Aabb result = { position - radius, position + radius };
  return result;
}

/**
 */
Aabb make_aabb(const float3 *positions, const size_t numPositions);

/**
 */
inline bool overlaps(const Aabb &a, const Aabb &b)
{
  return a.max.x > b.min.x && a.min.x < b.max.x
    &&   a.max.y > b.min.y && a.min.y < b.max.y
    &&   a.max.z > b.min.z && a.min.z < b.max.z;

}


/**
 */
Aabb operator * (const float4x4 &tfm, const Aabb &a);


} // namespace chag

#endif // _chag_Aabb_h
