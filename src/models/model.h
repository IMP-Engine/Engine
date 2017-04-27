#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../imgui/imgui.h"
#ifndef __gl_h_
#include "../glad/glad.h"
#endif // !__gl_h_

#ifndef _MODEL_CONFIG
#include "modelConfig.h"
#endif // !_MODEL_CONFIG


#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"
#include "../models/ModelData.h"

#include "Box.h"
#include "Cloth.h"

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
	void loadPredefinedModel(std::string model, ParticleData &particles, ConstraintData &constraints, ModelConfig config);

	/*
		Loads the object located at path into particles and adds simple 
		distance constraints to (potential) 26 nearest neighbours into constraints.
	*/
	void loadModel(std::string name, ParticleData &particles, ConstraintData &constraints, ModelConfig c, ModelData &modelData);

	/*
		Displays a window where the user can select different type of models.
	*/
	void gui(bool *show, ParticleData &particles, ConstraintData &constraints, std::vector< std::tuple<std::string, ModelConfig> > &objects, ModelData &modelData);
}
