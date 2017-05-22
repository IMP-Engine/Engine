// https://streamcomputing.eu/blog/2016-02-09/atomic-operations-for-floats-in-opencl-improved/
// Modified for float3
inline void atomicAdd_g_f(volatile __global float3 *addr, float3 val)
{
    union {
        unsigned int u32;
        float3        f32;
    } next, expected, current;
    current.f32 = *addr;
    do {
        expected.f32 = current.f32;
        next.f32 = expected.f32 + val;
        current.u32 = atomic_cmpxchg((volatile __global unsigned int *)addr,
            expected.u32, next.u32);
    } while (current.u32 != expected.u32);
}

__kernel void helloworld(
    __global float3 * pPos,
    __global float * invmass,
    __global int * numBoundConstraints,
    __global int2 * particles,
    __global float * distance,
    __global bool * equality,
    __global float3 * delta)
{
    int constraintIndex = get_global_id(0);

    int firstParticleIndex = particles[constraintIndex].x;
    int secondParticleIndex = particles[constraintIndex].y;

    float3 p1 = pPos[firstParticleIndex];
    float3 p2 = pPos[secondParticleIndex];

    float3 diff = p1 - p2;

    float c = length(diff) - distance[constraintIndex]; // fast_length?

    if (!equality[constraintIndex] && c >= 0)
        return;

    float3 grad = normalize(diff);
    float inv1 = invmass[firstParticleIndex];
    float inv2 = invmass[secondParticleIndex];
    grad /= inv1 + inv2;

    float3 delta1 = -inv1 * c * grad;
    float3 delta2 = inv2 * c * grad;

    // TODO iterate over particles...
    //__global float3 *d1 = &delta[firstParticleIndex];
    atomicAdd_g_f(&delta[firstParticleIndex], delta1);
    atomicAdd_g_f(&delta[secondParticleIndex], delta2);


}