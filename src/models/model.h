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
#include "../glad/glad.h"

#include "modelConfig.h"
#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"
#include "../models/ModelData.h"

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
	void loadPredefinedModel(std::string model, ParticleData &particles, ConstraintData &constraints, modelConfig config);

	/*
		Loads the object located at path into particles and adds simple 
		distance constraints to (potential) 26 nearest neighbours into constraints.
	*/
	void loadModel(std::string name, ParticleData &particles, ConstraintData &constraints, modelConfig c, ModelData &modelData);

	/*
		Displays a window where the user can select different type of models.
	*/
	void gui(bool *show, ParticleData &particles, ConstraintData &constraints, ModelData &modelData);
}
