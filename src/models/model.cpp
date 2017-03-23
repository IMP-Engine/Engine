#include "model.h"

#ifdef _WIN32
#define MODEL_FOLDER "../../src/models/"
#elif __unix__
#define MODEL_FOLDER "../src/models/"
#endif 

model::modelConfig config = {0.1f, vec3(0), 0, 0.5f, 0.5, vec3(1), ivec3(4)};
std::vector<std::string> predefinedModels{ "Box" };
std::vector<std::string> models;
int selected = 0;

// Temporary solution. Reconsider when cleaning code/changing way that particles and constraints are handled
extern std::vector<Particle> particles;
extern std::vector<Constraint*> constraints;

void model::loadModelNames() {
	models.clear();
	models.insert(models.end(), predefinedModels.begin(), predefinedModels.end());
	DIR *dir = opendir(MODEL_FOLDER);
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		std::string filename(ent->d_name);
		if (filename.size() > 4 && filename.substr(filename.size() - 4) == std::string(".sdf"))
			models.push_back(filename.substr(0, filename.size() - 4));
	}
	closedir(dir);
}

// This 
void model::loadPredefinedModel(std::string model, ParticleData &particles, std::vector<Constraint*> &constraints, modelConfig config)
{
	if (model == "Box")
	{
		Box::makeBox(particles, constraints, config);
	}
}

// For more information on what *max, origin and spacing is, refer to https://github.com/christopherbatty/SDFGen
// Explanation is found in main.cpp
void model::loadModel(std::string model, ParticleData &particles, std::vector<Constraint*> &constraints, modelConfig config)
{

	std::vector<Particle>::size_type start = particles.size();

	std::ifstream file(MODEL_FOLDER + model + ".sdf");
	if (!file)
	{
		std::cerr << "Error opening file " << model << ".sdf" << std::endl;
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
			p.radius = d / 2;

			particles.push_back(p);
		}
	}

	float maxDist = glm::length(d*config.scale);
	for (std::vector<Particle>::size_type i = start; i < particles.size(); i++) for (std::vector<Particle>::size_type j = i+1; j < particles.size(); j++)
	{
			if (glm::distance(particles[i].pos, particles[j].pos) <= maxDist )
			{
				Constraint* c = new DistanceConstraint(
					&particles[i],
					&particles[j],
					config.stiffness,
					config.distanceThreshold,
					glm::distance(particles[i].pos, particles[j].pos));
				constraints.push_back(c);
				particles[i].numBoundConstraints++;
				particles[j].numBoundConstraints++;
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
	ImGui::Combo("Choose model", &selected, [](void* a, int b, const char** c) -> bool { *c = ((std::vector<std::string>*)a)->at(b).c_str(); return true; }, &models, models.size());
	
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		particles.clear();
		constraints.clear();
		
		if ((unsigned int)selected >= predefinedModels.size())
		{
			loadModel(models[selected], particles, constraints, config);
		}
		else
		{
			loadPredefinedModel(models[selected], particles, constraints, config);
		}
	}
	// Below, sliders for deciding grejs
	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	ImGui::DragFloat3("Origin", &config.centerPos.x, 1.f, -(std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	ImGui::DragFloat3("Scale", &config.scale.x, 1.f, -(std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	ImGui::DragFloat("Invmass", &config.invmass, 0.005f, 0, 1000);
	ImGui::DragFloat("Distance treshold", &config.distanceThreshold, 0.001f, 0, 10, "%.7f");
	ImGui::SliderFloat("Stiffness", &config.stiffness, 0, 1);
	ImGui::InputInt("Phase", &config.phase);
	if (selected == 0)
	{
		ImGui::SliderInt("Particles x", &config.numParticles.x, 1, 10);
		ImGui::SliderInt("Particles y", &config.numParticles.y, 1, 10);
		ImGui::SliderInt("Particles z", &config.numParticles.z, 1, 10);
	}

	ImGui::End();
}
