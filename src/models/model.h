#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include <glm/glm.hpp>
#include "../imgui/imgui.h"

#include "../particles/Particle.h"
#include "../constraints/Constraint.h"
#include "../constraints/DistanceConstraint.h"

//http://www.softagalleria.net/download/dirent/
#include "dirent.h"

namespace model {

	// TODO Radius?
	struct modelConfig {
		float invmass;            // Particle mass
		vec3 centerPos;          // Where to place model
		int phase;                // Particle phase
		float stiffness;          // Constraint stiffness
		float distanceThreshold;  // Constraind distance treshold
		vec3 scale;               // Scale factor that model is scaled by (can be used to rotate)
	};

	/*
		Loads modelnames into memory
	*/
	void initModels();

	/*
		Loads the object located at path into particles and adds simple 
		distance constraints to (potential) 26 nearest neighbours into constraints.
	*/
	void loadModel(std::string name, std::vector<Particle>* particles, std::vector<Constraint*>* constraints, modelConfig c);

	/*
		Displays a window where the user can select different type of models.
	*/
	void gui(bool *show);
}
