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
#include "../imgui/imgui.h"
#include "../glad/glad.h"

#ifndef _MODEL_CONFIG
#include "modelConfig.h"
#endif // !_MODEL_CONFIG


#include "../particles/ParticleData.h"
#include "../constraints/ConstraintData.h"

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
	void loadPredefinedModel(std::string model, ParticleData &particles, ConstraintData &constraints, ModelConfig config);

    /*
        Calculates normals from given vertices for given elements.
    */
    void calculateNormals(std::vector<glm::vec3> &normals, std::vector<glm::vec4> &vertices, std::vector<GLushort> &elements);

	/*
		Loads the object located at path into particles and adds simple 
		distance constraints to (potential) 26 nearest neighbours into constraints.
	*/
	void loadModel(std::string name, ParticleData &particles, ConstraintData &constraints, ModelConfig c);

	/*
		Displays a window where the user can select different type of models.
	*/
	void gui(bool *show, ParticleData &particles, ConstraintData &constraints, std::vector< std::tuple<std::string, ModelConfig> > &objects);
}
