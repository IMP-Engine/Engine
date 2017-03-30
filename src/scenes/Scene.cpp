#include "Scene.h"

#ifdef _WIN32
#define SCENE_FOLDER "../../src/scenes/"
#elif __unix__
#define SCENE_FOLDER "../src/scenes/"
#endif

std::vector<std::string> predefinedScenes;
std::vector<std::string> scenes;
int selectedScene = 0;

Scene::Scene()
{
    Scene::loadSceneNames();
    Scene::loadScene("cube");
}

void Scene::loadSceneNames() {
    scenes.clear();
    scenes.insert(scenes.end(), predefinedScenes.begin(), predefinedScenes.end());
    DIR *dir = opendir(SCENE_FOLDER);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL)
    {
        std::string filename(ent->d_name);
        if (filename.size() > 4 && filename.substr(filename.size() - 4) == std::string(".obj"))
            scenes.push_back(filename.substr(0, filename.size() - 4));
    }
    closedir(dir);
}

void Scene::loadScene(std::string scene) {
    std::ifstream file(SCENE_FOLDER + scene + ".obj");
    if (!file)
    {
        std::cerr << "Error opening file " << scene << ".obj" << std::endl;
    }

    std::string line;

    while (std::getline(file, line))
    {
        // Load vertices
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line = line.substr(2));
            glm::vec3 vertex;
            s >> vertex.x; s >> vertex.y; s >> vertex.z;
            vertices.push_back(vertex);
        }

        // Load normals
        if (line.substr(0, 2) == "vn")
        {
            std::istringstream s(line = line.substr(2));
            glm::vec3 normal;
            s >> normal.x; s >> normal.y; s >> normal.z;
            normals.push_back(normal);
        }

        // Load indices and normal indices
        if (line.substr(0, 1) == "f")
        {
            glm::ivec3 index;
            int normalIndex;
            
            // Normal indices
            std::istringstream s(line.substr(line.find("//") + 2, line.find(" ") + 1));
            s >> normalIndex;
            normalIndices.push_back(normalIndex);

            // Indices
            line = line.substr(2);
            for (int xyz = 0; xyz < 3; ++xyz) {
                std::istringstream s(line.substr(0, line.find("//")));
                s >> index[xyz];
                index[xyz] -= 1; // -1 to account for 0-indexing in C++ vs 1-indexing from Blender
                line = line.substr(line.find(" ") + 1);
            }
            indices.push_back(index);
        }

        /*
           for (int i = 0; i < numIndices / 3; i++)
           {
           normals.push_back(normalComponents[i]);
           }
           */
        
    }

    for (unsigned int i = 0; i < indices.size(); i++)
    {
        Triangle t = Triangle();
        t.v0 = vertices[indices[i].x];
        t.v1 = vertices[indices[i].y];
        t.v2 = vertices[indices[i].z];

        t.u = t.v1 - t.v0;
        t.v = t.v2 - t.v0;
        t.normal = normalize(cross(t.u, t.v));
        triangles.push_back(t);
    }
}

void Scene::gui(bool *show)
{
    if (!*show)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(450, 300),  ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Scene Selection", show))
    {
        ImGui::End();
        return;
    }

    // Dropdown of all scenes in the scenes folder
    ImGui::Combo("Choose scene", &selectedScene, [](void *a, int b, const char **c) -> bool { *c = ((std::vector<std::string>*)a)->at(b).c_str(); return true; }, &scenes, scenes.size());

    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        vertices.clear();
        indices.clear();

        if ((unsigned int)selectedScene >= predefinedScenes.size()) {
            loadScene(scenes[selectedScene]);
            std::cout << "at least tried to load " << scenes[selectedScene] << std::endl;
        }

        Scene::init();
    }

    ImGui::End();
}

void Scene::render(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix) {

    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    // Send uniforms to shader
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjectionMatrix"), 1, false, &modelViewProjectionMatrix[0].x);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewMatrix"), 1, false, &modelViewMatrix[0].x);

    // Draw cube
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(vao);
   
    int size;
    glDisable(GL_CULL_FACE);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLint), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Scene::init() {

    // Shader setup
    shader = glHelper::loadShader(VERT_SHADER_PATH, FRAG_SHADER_PATH);
    GLuint mvp_location = glGetUniformLocation(shader, "MVP");
    GLuint vpos_location = glGetAttribLocation(shader, "vPos");

    // Scene setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &(vertices[0]), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), &(indices[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
