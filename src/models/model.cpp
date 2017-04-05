#include "model.h"

#ifdef _WIN32
#define MODEL_FOLDER "../../src/models/"
#elif __unix__
#define MODEL_FOLDER "../src/models/"
#endif 

model::modelConfig config = { 0.8f, vec3(0), 0, 0.5f, 0.5, vec3(1), ivec3(4) };
std::vector<std::string> predefinedModels{ "Box" };
std::vector<std::string> models;
int selected = 0;

// Temporary solution. Reconsider when cleaning code/changing way that particles and constraints are handled

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
void model::loadPredefinedModel(std::string model, ParticleData &particles, ConstraintData &constraints, modelConfig config)
{
	if (model == "Box")
	{
		Box::makeBox(particles, constraints, config);
	}
}

void loadMesh(std::string filename, std::vector<glm::vec4> &vertices, std::vector<GLushort> &elements)
{
    std::ifstream in(MODEL_FOLDER + filename + ".obj", std::ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << ".obj" << std::endl;
    }

    std::string line;
    while (getline(in, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0, 2) == "f ")
        {
            std::istringstream s(line.substr(2));
            GLushort a, b, c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
            elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        //else if (line[0] == '#') {/* ignoring this line */}
        //else {/* ignoring this line */}
    }
}


void model::calculateNormals(std::vector<glm::vec3> &normals, std::vector<glm::vec4> &vertices, std::vector<GLushort> &elements) {
    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i += 3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i + 1];
        GLushort ic = elements[i + 2];
        glm::vec3 normal = glm::normalize(glm::cross(
            glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
            glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
}

// For more information on what *max, origin and spacing is, refer to https://github.com/christopherbatty/SDFGen
// Explanation is found in main.cpp
void model::loadModel(std::string model, ParticleData &particles, ConstraintData &constraints, modelConfig config)
{

	std::vector<Particle>::size_type start = particles.cardinality;

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
			p.radius = d * min(config.scale.x, min(config.scale.y, config.scale.z))/ 2;

			addParticle(p,particles);
		}
	}

	float maxDist = glm::length(d*config.scale);

    std::vector<vec3> &position = particles.position;
    std::vector<int> &numBoundConstraints = particles.numBoundConstraints;

    for (std::vector<Particle>::size_type i = start; i < particles.cardinality; i++) 
    {

        for (std::vector<Particle>::size_type j = i+1; j < particles.cardinality; j++)
	    {
			    if (glm::distance(position[i], position[j]) <= maxDist )
			    {
                    DistanceConstraint constraint;
                    constraint.firstParticleIndex = i;
                    constraint.secondParticleIndex = j;
                    constraint.stiffness = config.stiffness;
                    constraint.distance = glm::distance(position[i], position[j]);
                    constraint.threshold = config.distanceThreshold;
                    constraint.equality = true;

				    addConstraint(constraints.distanceConstraints, constraint);

				    numBoundConstraints[i]++;
				    numBoundConstraints[j]++;
			    }
	    }
    }


    // Load Mesh
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLushort> elements;
    loadMesh(model, vertices, elements);
    printf("Size of vertices is: %i", vertices.size());

    // Find three closest particles and calculate barycentric coordinates for all vertices
    std::vector<float[3]> bcCoords(vertices.size());
    std::vector<int[3]> closestParticles(vertices.size());

    for (int i = 0; i < vertices.size(); i++)
    {
        closestParticles[i][0] = 0;
        closestParticles[i][1] = 1;
        closestParticles[i][2] = 2;

        // Sort wrt. distance from vertex. Looks messy, but should be the fastest way? Max 3 comparisons
        float distances[3];
        distances[0] = distance(vec3(vertices[i]), position[0]);
        float newDist = distance(vec3(vertices[i]), position[1]);
        if (newDist >= distances[0]) { distances[1] = newDist; }
        else
        {
            distances[1] = distances[0];
            distances[0] = newDist;
            closestParticles[i][0] = 1;
            closestParticles[i][1] = 0;
        }
        newDist = distance(vec3(vertices[i]), position[2]);
        if (newDist >= distances[1]) { distances[2] = newDist; }
        else
        {
            distances[2] = distances[1];
            closestParticles[i][2] = closestParticles[i][1];
            if (newDist >= distances[0])
            {
                distances[1] = newDist;
                closestParticles[i][1] = 2;
            }
            else
            {
                distances[1] = distances[0];
                distances[0] = newDist;
                closestParticles[i][1] = closestParticles[i][0];
                closestParticles[i][0] = 2;
            }
        }


        for (std::vector<Particle>::size_type j = (start + 3); j < particles.cardinality; j++)
        {
            newDist = distance(position[j], vec3(vertices[i]));
            if (newDist < distances[2])
            {
                if (newDist < distances[1])
                {
                    distances[2] = distances[1];
                    closestParticles[i][2] = closestParticles[i][1];
                    if (newDist < distances[0])
                    {
                        distances[1] = distances[0];
                        distances[0] = newDist;
                        closestParticles[i][1] = closestParticles[i][0];
                        closestParticles[i][0] = j;
                    }
                    else
                    {
                        distances[1] = newDist;
                        closestParticles[i][1] = j;
                    }
                }
                else
                {
                    distances[2] = newDist;
                    closestParticles[i][2] = j;
                }
            }
        }
        //printf("Distances: %f, %f, %f\n", distances[0], distances[1], distances[2]);

        // Calculate Barycentric coordinates

        // Adams BCoords
        vec3 vertex = vec3(vertices[i]);
        vec3 CA = position[closestParticles[i][2]] - position[closestParticles[i][0]];
        vec3 BA = position[closestParticles[i][1]] - position[closestParticles[i][0]];
        vec3 normal = normalize(cross(CA, BA));
        bcCoords[i][2] = dot(vertex - position[closestParticles[i][0]], normal); // distance offset along normal
        vec3 projetedPosition = vertex - bcCoords[i][2] * normal;

        // Baycentric check

        // particle position relative to v0
        vec3 pPrim = projetedPosition - vertex;

        // Compute dot products
        float dotCACA = dot(CA, CA);
        float dotCABA = dot(CA, BA);
        float dotBABA = dot(BA, BA);
        float dotCApPrim = dot(CA, pPrim);
        float dotBApPrim = dot(BA, pPrim);

        // Compute barycentric coordinates
        float invDenom = 1 / (dotCACA * dotBABA - dotCABA * dotCABA);
        bcCoords[i][0] = (dotBABA * dotCApPrim - dotCABA * dotBApPrim) * invDenom; // u
        bcCoords[i][1] = (dotCACA * dotBApPrim - dotCABA * dotCApPrim) * invDenom; // v

        printf("BCC for v[%i]: %f, %f, %f, sum: %f\n", i, bcCoords[i][0], bcCoords[i][1], bcCoords[i][2], (bcCoords[i][0] + bcCoords[i][1] + bcCoords[i][2]));
    }

    calculateNormals(normals, vertices, elements);

}



void model::gui(bool *show, ParticleData &particles, ConstraintData &constraints)
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
	ImGui::Combo("Choose model", &selected, [](void *a, int b, const char **c) -> bool { *c = ((std::vector<std::string>*)a)->at(b).c_str(); return true; }, &models, models.size());
	
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
