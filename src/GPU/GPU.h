#pragma once

#ifndef __gl_h_
#include "../glad/glad.h"
#endif // !__gl_h_

#include "../glHelper.h"
#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"

#ifdef _WIN32
#define SC_SHADER_PATH "../../src/GPU/solveConstraints.frag"
#define VERTEX_SHADER_PATH "../../src/GPU/passthrough.vert"
#elif __unix__
#define SC_SHADER_PATH "../src/GPU/solveConstraints.frag"
#define VERTEX_SHADER_PATH "../src/GPU/passthrough.vert"
#endif

#define target GL_TEXTURE_RECTANGLE

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

    /* Geometry to cover the whole screen and accompanying texcoords */
     GLfloat vertices[20] = {  
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    GLuint program;

    GLuint vao;
    GLuint vbo;
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