#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>

#include <glm/glm.hpp>
#include "../imgui/imgui.h"

#include "modelConfig.h"
#include "../particles/Particle.h"
#include "../constraints/Constraint.h"
#include "../constraints/DistanceConstraint.h"

#include "Box.h"

//http://www.softagalleria.net/download/dirent/
#include <dirent.h>

namespace model {

	/*
		Loads modelnames into memory
	*/
	void loadModelNames();

	/*
		Loads a non sdf model into particles and adds object specific constraints.
	*/
	void loadPredefinedModel(std::string model, std::vector<Particle> &particles, std::vector<Constraint*> &constraints, modelConfig config);

	/*
		Loads the object located at path into particles and adds simple 
		distance constraints to (potential) 26 nearest neighbours into constraints.
	*/
	void loadModel(std::string name, std::vector<Particle> &particles, std::vector<Constraint*> &constraints, modelConfig c);

	/*
		Displays a window where the user can select different type of models.
	*/
	void gui(bool *show);
}
