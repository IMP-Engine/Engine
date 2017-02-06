#include "Quaternion.h"
#include "float3x3.h"
#include <cmath>

namespace chag
{



Quaternion make_quaternion_axis_angle(const float3 &_axis, float angle)
{
  float3 axis = normalize(_axis);

  float halfAngle = 0.5f * angle;
  float sinHalfAngle = sinf(halfAngle);
  return Quaternion(axis * sinHalfAngle, cosf(halfAngle));
}

/**
 */
Quaternion make_quaternion(const float3x3 &rot)
{
  Quaternion result;
  /* Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  * article "Quaternion Calculus and Fast Animation".
  */
  static const int next[3] = { 1, 2, 0 };
  float trace = rot[0][0]+rot[1][1]+rot[2][2];
  float root;
  float* quat[3];
  int i, j, k;

  if ( trace > 0.0 )
  {
    // |w| > 1/2, may as well choose w > 1/2
    root = sqrtf(trace+1.0f);  // 2w
    result.w = 0.5f * root;
    root = 0.5f / root;  // 1/(4w)
    result.v.x = (rot[1][2]-rot[2][1])*root;
    result.v.y = (rot[2][0]-rot[0][2])*root;
    result.v.z = (rot[0][1]-rot[1][0])*root;
  }
  else
  {
    // |w| <= 1/2
    i = 0;
    if ( rot[1][1] > rot[0][0] )
      i = 1;
    if ( rot[2][2] > rot[i][i] )
      i = 2;
    j = next[i];
    k = next[j];

    root = sqrt(rot[i][i]-rot[j][j]-rot[k][k]+1.0f);
    /* quat[3] = { &(result.x), &(result.y), &(result.z) };*/
    quat[0]=&(result.v.x); 
    quat[1]=&(result.v.y); 
    quat[2]=&(result.v.z);

    *quat[i] = 0.5f*root;
    root = 0.5f/root;
    result.w = (rot[j][k]-rot[k][j])*root;
    *quat[j] = (rot[i][j]+rot[j][i])*root;
    *quat[k] = (rot[i][k]+rot[k][i])*root;
  }

  return result;
}

/**
 */
Quaternion make_quaternion(const float4x4 &rot)
{
  return make_quaternion(make_matrix3x3(rot));
}


const float n(const Quaternion& q) 
{
  return std::sqrt(q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z + q.w * q.w);
};



const Quaternion inv(const Quaternion& q)
{
  return q.conj() * (1.0f / n(q));
}



const float4x4 makematrix(const Quaternion& q)
{
  // This should be simplified for unit quaternions
  float4x4 m;
  float s = 2.0f / n(q);
  m.c1 = make_vector(1.0f - s*(q.v.y*q.v.y + q.v.z*q.v.z), 
		  s*(q.v.x*q.v.y + q.w*q.v.z), 
		  s*(q.v.x*q.v.z - q.w*q.v.y), 
		  0.0f);
  m.c2 = make_vector(s*(q.v.x*q.v.y - q.w*q.v.z), 
		  1.0f - s*(q.v.x*q.v.x + q.v.z*q.v.z), 
		  s*(q.v.y*q.v.z + q.w*q.v.x), 
		  0.0f);
  m.c3 = make_vector(s*(q.v.x*q.v.z + q.w*q.v.y), 
		  s*(q.v.y*q.v.z - q.w*q.v.x), 
		  1.0f - s*(q.v.x*q.v.x + q.v.y*q.v.y), 
		  0.0f);
  m.c4 = make_vector(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}



const float3x3 toMatrix3x3(const Quaternion& q)
{
  // This should be simplified for unit quaternions
  float3x3 m;
  float s = 2.0f / n(q);
  m.c1 = make_vector(1.0f - s*(q.v.y*q.v.y + q.v.z*q.v.z), 
		  s*(q.v.x*q.v.y + q.w*q.v.z), 
		  s*(q.v.x*q.v.z - q.w*q.v.y));
  m.c2 = make_vector(s*(q.v.x*q.v.y - q.w*q.v.z), 
		  1.0f - s*(q.v.x*q.v.x + q.v.z*q.v.z), 
		  s*(q.v.y*q.v.z + q.w*q.v.x));
  m.c3 = make_vector(s*(q.v.x*q.v.z + q.w*q.v.y), 
		  s*(q.v.y*q.v.z - q.w*q.v.x), 
		  1.0f - s*(q.v.x*q.v.x + q.v.y*q.v.y));
  return m;
}


const float phi(Quaternion q, Quaternion r)
{
	float qdotr = q.v.x*r.v.x + q.v.y*r.v.y + q.v.z*r.v.z + q.w*r.w;
	return std::acos(qdotr);
}



/*
const Quaternion slerp(Quaternion q, Quaternion r, float t)
{
	float p = phi(q, r);
	return q * (sin(p*(1.0 - t))/sin(p)) + r * (sin(p*t)/sin(p));
}*/


/*
const float phi(Quaternion q, Quaternion r)
{
	float arg = q.v.x*r.v.x + 
	  q.v.y*r.v.y + 
	  q.v.z*r.v.z + 
	  q.w*r.w;

	if(arg < 0) cout << "arg < 0" << endl; 

  return sqrt(abs(q.v.x*r.v.x + 
	  q.v.y*r.v.y + 
	  q.v.z*r.v.z + 
	  q.w*r.w));
}*/



const Quaternion slerp(Quaternion q, Quaternion r, float t)
{
	double cosHalfTheta = q.w * r.w + q.v.x * r.v.x + q.v.y * r.v.y + q.v.z * r.v.z;

	// if q=r or q=-r then theta = 0 and we can return q
	if (std::abs(cosHalfTheta) >= 0.999999999)
  {
		return q;
	}

	// Calculate temporary values.
	double halfTheta = std::acos(cosHalfTheta);

	double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb

	if (std::abs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
		return (q * 0.5) + (r * 0.5); 
	}

	double ratioA = std::sin((1 - t) * halfTheta) / sinHalfTheta;
	double ratioB = std::sin(t * halfTheta) / sinHalfTheta; 
	//calculate Quaternion.
	return q * float(ratioA) + r * float(ratioB);
}

/*
const Quaternion slerp(Quaternion qa, Quaternion r, float t) {
	// quaternion to return
	Quaternion qm;
	// Calculate angle between them.
	double cosHalfTheta = qa.w * r.w + qa.v.x * r.v.x + qa.v.y * r.v.y + qa.v.z * r.v.z;
	// if qa=r or qa=-r then theta = 0 and we can return qa
	if (abs(cosHalfTheta) >= 1.0){
		qm.w = qa.w;qm.v.x = qa.v.x;qm.v.y = qa.v.y;qm.v.z = qa.v.z;
		return qm;
	}
	// Calculate temporary values.
	double halfTheta = acos(cosHalfTheta);
	double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or r
	if (fabs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
		qm.w = (qa.w * 0.5 + r.w * 0.5);
		qm.v.x = (qa.v.x * 0.5 + r.v.x * 0.5);
		qm.v.y = (qa.v.y * 0.5 + r.v.y * 0.5);
		qm.v.z = (qa.v.z * 0.5 + r.v.z * 0.5);
		return qm;
	}
	double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
	double ratioB = sin(t * halfTheta) / sinHalfTheta; 
	//calculate Quaternion.
	qm.w = (qa.w * ratioA + r.w * ratioB);
	qm.v.x = (qa.v.x * ratioA + r.v.x * ratioB);
	qm.v.y = (qa.v.y * ratioA + r.v.y * ratioB);
	qm.v.z = (qa.v.z * ratioA + r.v.z * ratioB);
	return qm;
}
*/

} // namespace chag
