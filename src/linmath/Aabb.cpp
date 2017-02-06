#include "Aabb.h"
#include "float4x4.h"
#include <float.h>
#include <algorithm>

namespace chag
{


Aabb make_aabb(const float3 &min, const float3 &max)
{
  Aabb result = { min, max };
  return result;
}


Aabb make_inverse_extreme_aabb()
{
  return make_aabb(make_vector(FLT_MAX, FLT_MAX, FLT_MAX), make_vector(-FLT_MAX, -FLT_MAX, -FLT_MAX));
}



Aabb make_aabb(const float3 *positions, const size_t numPositions)
{
  Aabb result = make_inverse_extreme_aabb();

  for (size_t i = 0; i < numPositions; ++i)
  {
    result = combine(result, positions[i]);
  }

  return result;
}




} // namespace chag
