#pragma once

#include <glm/common.hpp>

#ifndef __gl_h_
#include "../glad/glad.h"
#endif // !__gl_h_

#include "Device.h"
#include "../glHelper.h"
#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"

#ifdef _WIN32
#define SC_SHADER_PATH "../../src/GPU/solveConstraints.frag"
#define VI_SHADER_PATH "../../src/GPU/verletIntegration.frag"
#define UV_SHADER_PATH "../../src/GPU/updateVelocity.frag"
#define UP_SHADER_PATH "../../src/GPU/updatePosition.frag"
#define VERTEX_SHADER_PATH "../../src/GPU/passthrough.vert"
#elif __unix__
#define SC_SHADER_PATH "../src/GPU/solveConstraints.frag"
#define VI_SHADER_PATH "../src/GPU/verletIntegration.frag"
#define UV_SHADER_PATH "../src/GPU/updateVelocity.frag"
#define UP_SHADER_PATH "../src/GPU/updatePosition.frag"
#define VERTEX_SHADER_PATH "../src/GPU/passthrough.vert"
#endif

#define target GL_TEXTURE_RECTANGLE

/***************************************************************/
/* For the verlet integration step, only position and velocity */
/* is required in order to do the computation. It is therefore */
/* unnecessary to store AoS. The constraint information, on    */
/* the other hand, is always queried all at the same time.     */
/* Thusly, particle info is SoA. Constraint data is AoS        */
/***************************************************************/

/***************************************************************/
/* The current implementation iterates over particles. How to  */
/* actually do a constraint-centric solve is not clear at the  */
/* moment as that would require both atomic updates and        */
/* mapping the solve step outputs into a smaller texture.      */
/* The tradeoff for these methods are atomic operations,       */
/* memory fetches and actual required computation.             */
/***************************************************************/

class GPU : public Device
{
public:
    
    GPU(ParticleData & particles, ConstraintData & constraints, std::vector<Triangle> & triangles);
    ~GPU();

    // Start and stop does not allow for smooth transition
    void start(ParticleData & particles, ConstraintData & constraints);
    void stop(ParticleData & particles);

    void verletIntegrate(float dt, glm::vec3 forces);

    void solveConstraintsJacobi(int iterations, float omega);

    void updateParticles(float dt);


private:

    /* Geometry to cover the whole screen */
     GLfloat vertices[20] = {  
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };

    GLuint constraintSolveProgram, verletIntegrationProgram, updateVelocityProgram, updatePositionProgram;

    GLuint vao;
    GLuint vbo;
    GLuint fbo;

    // Worksize
    GLuint pWidth, pHeight, cWidth, cHeight;

    /************************************/
    /* +----------+----------+--------+ */
    /* | variable | internal | format | */
    /* +----------+----------+--------+ */
    /* | bufMain  | RGB32F   | RGB    | */
    /* | bufSub   | RGB32F   | RGB    | */
    /* | posStart | RGB32F   | RGB    | */
    /* | invmass  | R32F     | RED    | */
    /* |constraint| RGBA32F  | RGBA   | */
    /* | segments | R32UI    | RED    | */ 
    /* | indirect | R32UI    | RED    | */
    /* +----------+----------+--------+ */
    /************************************/
    GLuint bufMain, bufSub; // Ping pong buffers
    GLuint posStart, invmasses;          
    GLuint boundConstraints; // TODO Remove in favor of GS
    /**************************************************/
    /*         Distance constraint parameters         */
    /* This represents a RGBA32F texture, i.e. vec4.  */
    /* vec4.x and vec4.y is affected particle.        */
    /* vec4.z is rest length. vec4.a is the stiffnes. */
    /**************************************************/
    GLuint constraint; // TODO is particles needed? Use indirection matrix

    /****************************************************/
    /* These textures are stored as integer values.     */
    /* Therefore note that texelFetch() is used instead */
    /* of texture() when accesing elements from segment */
    /****************************************************/
    GLuint segment, indirect;

    // Inherited via Device
    virtual void solveConstraintsGaussSeidel(int iterations, float omega) override;
    virtual void updateParticles(float dt, float pSleeping) override;
    virtual void detectCollisionsTriangles() override;
    virtual void detectCollisionParticles() override;
    virtual void stabilizationPass(int stabilizationIterations) override;
    virtual void dampCollision(float restitutionCoefficientT, float restitutionCoefficientN) override;
    virtual void renderParticles(glm::mat4 & modelViewProjectionMatrix, glm::mat4 & modelViewMatrix, glm::vec3 & viewSpaceLightPosition, glm::mat4 & projectionMatrix, int height) override;
};