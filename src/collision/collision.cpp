#include "collision.h"

collision::collisionDetectionType currentType = collision::BruteForce;
bool showTreeDiagnostics = false;
bool ignorePhase = false;
float smallestVolume = 0.1;
int minParticles = 1;

Octree *octree;

int largestLeaf(Octree::Node * n) {
    if (n->children[0] == nullptr)
        return n->particles.size();
    int i = 0;
    for (int j = 0; j < 8; j++) {
        int k = largestLeaf(n->children[j]);
        i = i > k ? i : k;
    }
    return i;
}

int particlesInTree(Octree::Node * n) {
    if (n->children[0] == nullptr)
        return n->particles.size();
    int i = 0;
    for (int j = 0; j < 8; j++) {
        i += particlesInTree(n->children[j]);

    }
    return i;
}

int numberOfNodes(Octree::Node * n) {
    if (n->children[0] == nullptr)
        return 1;
    int i = 0;
    for (int j = 0; j < 8; j++) {
        i += particlesInTree(n->children[j]);

    }
    return i + 1;
}

void collision::gui(bool * show)
{
    if (!*show)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(350, 560), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Collision detection", show))
    {
        ImGui::End();
        return;
    }
    ImGui::Checkbox("Ignore phase", &ignorePhase);
    ImGui::Text("Collision detection type.");
    ImGui::RadioButton("Brute force", reinterpret_cast<int*>(&currentType), static_cast<int>(BruteForce)); ImGui::SameLine();
    ImGui::RadioButton("Octree", reinterpret_cast<int*>(&currentType), static_cast<int>(Octree));

    switch (currentType)
    {
    case BruteForce:
        break;
    case Octree: 
        ImGui::SliderInt("min particles", &minParticles, 1, 500);
        ImGui::SliderFloat("min volume", &smallestVolume, 0.001, 100);
        ImGui::Checkbox("Show tree diagnostics", &showTreeDiagnostics);
        if (showTreeDiagnostics && octree) {
            ImGui::Text("Largest leaf: "); ImGui::SameLine(); ImGui::Text(std::to_string(largestLeaf(octree->getRoot())).c_str());
            ImGui::Text("Total particles in tree: "); ImGui::SameLine(); ImGui::Text(std::to_string(particlesInTree(octree->getRoot())).c_str());
            ImGui::Text("Total nodes in tree: "); ImGui::SameLine(); ImGui::Text(std::to_string(numberOfNodes(octree->getRoot())).c_str());
        }
        break;
    }

    ImGui::End();
}

void collision::createCollisionConstraints(ParticleData & particles, DistanceConstraintData & constraints)
{
    switch(currentType) 
    {
    case BruteForce:
        bruteForce(particles, constraints, ignorePhase);
        break;
    case Octree:
        if (octree)
            delete octree;
        octree = new ::Octree();
        octree->construct(particles, BoundingVolume(vec3(-10, -10, -10), 20.f), minParticles, smallestVolume, ignorePhase);
        octree->findCollisions(particles, constraints);
        break;
    }
}
