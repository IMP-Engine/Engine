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
    /*
    // Cube setup
    unsigned short cubeIndices[] = {
        // front
        2, 1, 0,
        2, 0, 3,
        // top
        2, 5, 1,
        2, 6, 5,
        // back
        4, 5, 6,
        4, 6, 7,
        // bottom
        4, 7, 3,
        4, 3, 0,
        // left
        4, 0, 1,
        4, 1, 5,
        // right
        2, 3, 7,
        2, 7, 6
    };

    float cubeVertices[] = {
        -10.0f, -10.0f,  10.0f,
        10.0f, -10.0f,  10.0f,
        10.0f,  10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,

        -10.0f, -10.0f, -10.0f,
        10.0f, -10.0f, -10.0f,
        10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,
    };

    float normalComponents[] = {
        -0.5773f, -0.5773f,  0.5773f,
         0.5773f, -0.5773f,  0.5773f,
         0.5773f,  0.5773f,  0.5773f,
        -0.5773f,  0.5773f,  0.5773f,

        -0.5773f, -0.5773f, -0.5773f,
         0.5773f, -0.5773f, -0.5773f,
         0.5773f,  0.5773f, -0.5773f,
        -0.5773f,  0.5773f, -0.5773f,
    };

    for (int i = 0; i < 24; i++)
    {
        //vertices.push_back(cubeVertices[i]);
        a.push_back(cubeVertices[i]);
    }
    for (int i = 0; i < 36; i++)
    {
        //indices.push_back(cubeIndices[i]);
        b.push_back(cubeIndices[i]);
    }
    for (int i = 0; i < 24; i++)
    {
        normals.push_back(normalComponents[i]);
    }

    for (unsigned int i = 0; i < b.size();i += 3) {
        Triangle t = Triangle();
        t.v0 = vec3(a[3 * b[i]],
                    a[3 * b[i] + 1],
                    a[3 * b[i] + 2]);

        t.v1 = vec3(a[3 * b[i + 1]],
                    a[3 * b[i + 1] + 1],
                    a[3 * b[i + 1] + 2]);

        t.v2 = vec3(a[3 * b[i + 2]],
                    a[3 * b[i + 2] + 1],
                    a[3 * b[i + 2] + 2]);

        t.u = t.v1 - t.v0;
        t.v = t.v2 - t.v0;
        t.normal = normalize(cross(t.u, t.v));
        triangles.push_back(t);
    }
*/

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
        // Fill vertices
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line = line.substr(2));
            glm::vec3 vertex;
            s >> vertex.x; s >> vertex.y; s >> vertex.z;
            vertices.push_back(vertex);
        }

        // Fill indices
        if (line.substr(0, 1) == "f")
        {
            glm::vec3 index;
            line = line.substr(2);
            for (int xyz = 0; xyz < 3; xyz++) {
                std::istringstream s(line.substr(0, line.find("//")));
                s >> index[xyz];
                line = line.substr(line.find(" ") + 1);
            }
            indices.push_back(index);
            //std::cout << index.x << ", " << index.y << ", " << index.z << std::endl;
        }

        // Fill normals
        /*
           for (int i = 0; i < numIndices / 3; i++)
           {
           normals.push_back(normalComponents[i]);
           }
           */
        
    }

    for (unsigned int i = 0; i < indices.size(); i++)
    {
        std::cout << "indices.size() = "<< indices.size() << std::endl;
        std::cout << "vertices.size() = "<< vertices.size() << std::endl;
        Triangle t = Triangle();
        /*
        t.v0 = vec3(vertices[indices[i]],
                    vertices[indices[i + 1]],
                    vertices[indices[i + 2]]);

        t.v1 = vec3(vertices[indices[i]],
                    vertices[indices[i + 1]],
                    vertices[indices[i + 2]]);

        t.v2 = vec3(vertices[indices[i]],
                    vertices[indices[i + 1]],
                    vertices[indices[i + 2]]);
                    */
        //std::cout << "vertices " << indices[i].x << ", " << indices[i].y << ", " << indices[i].z << std::endl;
        t.v0 = vertices[indices[i].x - 1];
        t.v1 = vertices[indices[i].y - 1];
        t.v2 = vertices[indices[i].z - 1];

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

    // Dropdown of all scenes the scenes folder
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
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0); //sizeof(GLushort),

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::vec3) * indices.size(), &(indices[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
