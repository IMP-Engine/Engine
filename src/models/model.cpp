#include "model.h"

model::modelConfig config;
std::vector<std::string> models;
int current = -1;

extern std::vector<Particle> particles;
extern std::vector<Constraint*> constraints;

void model::initModels() {
	DIR *dir = opendir("../../src/models/");
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		std::string filename(ent->d_name);
		if (filename.size() > 4 && filename.substr(filename.size() - 4) == std::string(".sdf"))
			models.push_back(filename.substr(0, filename.size() - 4));
	}
	closedir(dir);
	delete ent;

	config.scale = vec3(1);
}

// For more information on what *max, origin and spacing is, refer to https://github.com/christopherbatty/SDFGen
// Explanation is found in main.cpp
void model::loadModel(std::string model, std::vector<Particle>* particles, std::vector<Constraint*>* constraints, modelConfig config)
{

	std::vector<Particle>::size_type size = particles->size();

	std::ifstream file("../../src/models/" + model);
	if (!file)
	{
		std::cerr << "Error opening file " << model << std::endl;
	}

	std::string line;

	int imax, jmax, kmax;
	std::getline(file, line);
	std::stringstream data(line);
	data >> imax >> jmax >> kmax;

	vec3 origin;
	std::getline(file, line);
	std::stringstream data2(line);
	data2 >> origin.x >> origin.y >> origin.z;

	float spacing;
	std::getline(file, line);
	std::stringstream data3(line);
	data3 >> spacing;

	// Normalize lengths 
	// TODO Particle radius? 
	float d = spacing / glm::length(origin);
	origin /= glm::length(origin);

	for (int i = 0; i < imax; i++) for (int j = 0; j < jmax; j++) for (int k = 0; k < kmax; k++)
	{
		std::getline(file, line);
		// Negative inside, positive outside
		if (line[0] == '-')
		{
			std::stringstream data(line);
			Particle p;
			p.pos = config.centerPos + origin + vec3(i*d, j*d, k*d) * config.scale;
			p.invmass = config.invmass;
			p.numBoundConstraints = 0;
			p.phase = config.phase;
			particles->push_back(p);
		}
	}

	float maxDist = sqrt(glm::length(d*config.scale));
	for (std::vector<Particle>::size_type i = size; i < particles->size(); i++) for (std::vector<Particle>::size_type j = i+1; j < particles->size(); j++)
	{
			if (glm::distance((*particles)[i].pos, (*particles)[j].pos) <= maxDist )
			{
				Constraint* c = new DistanceConstraint(
					&(*particles)[i],
					&(*particles)[j],
					config.stiffness,
					config.distanceThreshold,
					glm::distance((*particles)[i].pos, (*particles)[j].pos));
				constraints->push_back(c);
				(*particles)[i].numBoundConstraints++;
				(*particles)[j].numBoundConstraints++;
			}
	}
}



void model::gui(bool *show)
{
	if (!*show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Models", show))
	{
		ImGui::End();
		return;
	}

	// Dropdown of all models in this folder
	ImGui::Combo("Choose model", &current, [](void* a, int b, const char** c) -> bool { *c = ((std::vector<std::string>*)a)->at(b).c_str(); return true; }, &models, models.size());
	
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		particles.clear();
		constraints.clear();
		loadModel(models[current] + ".sdf", &particles, &constraints, config);
	}
	// Below, sliders for deciding grejs
	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	ImGui::DragFloat3("Origin", &config.centerPos.x, 1.f, -(std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	ImGui::DragFloat3("Scale", &config.scale.x, 1.f, -(std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	ImGui::DragFloat("Invmass", &config.invmass, 0.05f, 0, 1000);
	ImGui::DragFloat("Distance treshold", &config.distanceThreshold, 0.001f, 0, 10, "%.7f");
	ImGui::DragFloat("Stiffness", &config.stiffness, 0.01f, 0, 1, "%.4f");

	ImGui::End();
}
