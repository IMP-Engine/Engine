#pragma once

#ifndef __gl_h_
#include "../glad/glad.h"
#endif // !__gl_h_

#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"


#define target GL_TEXTURE_2D

/***************************************************************/
/* For the verlet integration step, only position and velocity */
/* is required in order to do the computation. It is therefore */
/* unnecessary to store AoS. The constraint information, on    */
/* the other hand is always queried all at the same time.      */
/* Thusly, particle info is SoA. Constraint data is AoS        */
/***************************************************************/

class GPU
{
public:
    
    GPU();
    ~GPU();

    void restart(ParticleData & particles, ConstraintData & constraints);

    void run(ParticleData & particles, ConstraintData & constraints);

private:
   
    GLuint fbo;

    /************************************/
    /* +----------+----------+--------+ */
    /* | variable | internal | format | */
    /* +----------+----------+--------+ */
    /* | posMain  | RGB32F   | RGB    | */
    /* | posSub   | RGB32F   | RGB    | */
    /* | invmass  | R32F     | RED    | */
    /* +----------+----------+--------+ */
    /************************************/
    GLuint posMain, posSub, invmasses;          

    /**************************************************/
    /*         Distance constraint parameters         */
    /* This represents a RGBA32F texture, i.e. vec4.  */
    /* vec4.x and vec4.y is affected particle.        */
    /* vec4.z is rest length. vec4.a is the stiffnes. */
    /**************************************************/
    GLuint constraint; // TODO is particles needed? Use indirection matrix
};