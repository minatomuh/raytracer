#ifndef _SCENEBUILDER_H
#define _SCENEBUILDER_H

#include <string>
#include "shape/Object.h"
#include "scene/Scene.h"
#include "Hit.h"
#include "../include/tinyxml2.h"

class SceneBuilder {
public:
    SceneBuilder();
    SceneBuilder(Scene);
    SceneBuilder(char*);
    ~SceneBuilder();

    void importScene(char*);
    void exportScene();
    void printScene();
    void setAntiAliasing(int);
    int getAntiAliasing();

private:
    Scene scene;
    int anti_aliasing;

    RGB trace(const Ray& ray, int depth);
    RGB shade(const Ray& ray, const Hit& hit, const PointLight& light, int depth);

    friend void renderChunk(SceneBuilder* builder, const Camera& camera, int start, int end, std::vector<RGB>& buffer, std::mutex& buffer_mutex, std::atomic<int>& completed_scanlines, int total_scanlines, std::mutex& cerr_mutex);

    void parseScene(tinyxml2::XMLDocument& xmlDoc);
    void parseMaxRayTraceDepth(tinyxml2::XMLElement* root);
    void parseBackgroundColor(tinyxml2::XMLElement* root);
    void parseCameras(tinyxml2::XMLElement* root);
    void parseCamera(tinyxml2::XMLElement* camera_element);
    void parseLights(tinyxml2::XMLElement* root);
    void parsePointLight(tinyxml2::XMLElement* light_element);
    void parseMaterials(tinyxml2::XMLElement* root);
    void parseMaterial(tinyxml2::XMLElement* material_element);
    void parseVertexData(tinyxml2::XMLElement* root);
    void parseObjects(tinyxml2::XMLElement* root);
    void parseMesh(tinyxml2::XMLElement* mesh_element);
    void parseTriangle(tinyxml2::XMLElement* triangle_element);
    void parseSphere(tinyxml2::XMLElement* sphere_element);
    
};

#endif