#include <fstream>
#include <sstream>
#include <random>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>

#include "SceneBuilder.h"
#include "Vector.h"

#include "scene/Scene.h"

#include "shape/Object.h"
#include "shape/Triangle.h"
#include "shape/Mesh.h"
#include "shape/Sphere.h"

using std::cout;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::endl;

SceneBuilder::SceneBuilder() {
}

SceneBuilder::SceneBuilder(Scene s) {
    scene = s;
}

SceneBuilder::SceneBuilder(char* filename) {
    importScene(filename);
}

SceneBuilder::~SceneBuilder() {
    for(auto it = scene.objects.begin(); it != scene.objects.end(); ++it) {
        delete* it;
    }
}

void SceneBuilder::importScene(char* filename) {
    // Open XML
    tinyxml2::XMLDocument xmlDoc;
    if (xmlDoc.LoadFile(filename) != tinyxml2::XML_SUCCESS) {
        std::string fl = filename;
        throw std::runtime_error("Error opening XML file: " + fl);
    }
    
    // Call parseScene to handle all
    parseScene(xmlDoc);
}

void SceneBuilder::setAntiAliasing(int n) {
    anti_aliasing = n;
}

int SceneBuilder::getAntiAliasing() {
    return anti_aliasing;
}

RGB convert(Color c) {
    return RGB(static_cast<short>(c.x() * 255), static_cast<short>(c.y() * 255), static_cast<short>(c.z() * 255));
}

double generate_random_double() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

void renderChunk(SceneBuilder* builder, const Camera& camera, int start, int end, std::vector<RGB>& buffer, std::mutex& buffer_mutex, std::atomic<int>& completed_scanlines, int total_scanlines, std::mutex& cerr_mutex) {
    Vector w = camera.gaze;
    Vector u = (camera.up * w).normalize();
    Vector v = w * u;

    double aspect_ratio = static_cast<double>(camera.h_res) / camera.v_res;
    double image_plane_width = camera.right - camera.left;
    double image_plane_height = (camera.top - camera.bottom) / aspect_ratio;

    for(int j = start; j < end; ++j) {
        for(int i = 0; i < camera.h_res; ++i) {
            RGB color(0, 0, 0);
            // Anti aliasing
            for(int k = 0; k < builder->anti_aliasing; ++k) {
                // Create a ray from camera to pixel
                double u_offset = ((double)i + generate_random_double()) * image_plane_width / camera.h_res;
                double v_offset = ((double)j + generate_random_double()) * image_plane_height / camera.v_res;
                Point pixel_pos = camera.position + (w * camera.near_distance) + (u * (camera.left + u_offset)) + (v * (camera.bottom + v_offset));

                Ray ray(camera.position, (pixel_pos - camera.position).normalize());
                // call trace for ray
                color = color + builder->trace(ray, 0);
            }
            color = color / builder->anti_aliasing;

            // Store the result in the buffer
            std::lock_guard<std::mutex> guard(buffer_mutex);
            buffer[j * camera.h_res + i] = color;
        }

        // Update progress
        completed_scanlines++;
        {
            std::lock_guard<std::mutex> guard(cerr_mutex);
            int progress = static_cast<int>(100.0 * completed_scanlines / total_scanlines);
            std::cerr << "\rProgress: " << progress << "%" << std::flush;
        }
    }
}

void SceneBuilder::exportScene() {
    std::ofstream out;
    const int num_threads = std::thread::hardware_concurrency(); // Get the number of available threads

    // For every camera, output an image
    for (auto camera : scene.cameras) {
        out.open(camera.image_name, std::ios::binary | std::ios::ate | std::ios::out);

        // ppm header
        out << "P3" << "\n";
        out << camera.h_res << " " << camera.v_res << "\n";
        out << "255" << "\n";

        std::vector<RGB> buffer(camera.h_res * camera.v_res);
        std::mutex buffer_mutex;
        std::mutex cerr_mutex;
        std::atomic<int> completed_scanlines(0);
        int total_scanlines = camera.v_res;

        // Determine the chunk size for each thread
        int chunk_size = std::ceil(static_cast<double>(camera.v_res) / num_threads);

        // Launch threads
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            int start = t * chunk_size;
            int end = std::min(start + chunk_size, static_cast<int>(camera.v_res));
            threads.emplace_back(renderChunk, this, std::ref(camera), start, end, std::ref(buffer), std::ref(buffer_mutex), std::ref(completed_scanlines), total_scanlines, std::ref(cerr_mutex));
        }

        // Wait for all threads to finish
        for (auto& thread : threads) {
            thread.join();
        }

        // Write the buffer to the output file
        for (const auto& color : buffer) {
            out << color;
        }

        out.close();
        std::cerr << std::endl;
    }
}

void SceneBuilder::printScene() {
    cout << "\nMax Ray Trace Depth: " << scene.max_raytracedepth << "\n";
    cout << "Background Color: " << scene.background_color << "\n";
    
    for(int i = 0; i < scene.cameras.size(); ++i) {
        cout << "Camera[" << i << "]:\n";
        cout << "\tID: " << scene.cameras[i].id << "\n";
        cout << "\tPosition: " << scene.cameras[i].position << "\n";
        cout << "\tGaze: " << scene.cameras[i].gaze << "\n";
        cout << "\tUp: " << scene.cameras[i].up << "\n";
        cout << "\tLeft Right Bottom Top: " << scene.cameras[i].left << " "
                            << scene.cameras[i].right << " "
                            << scene.cameras[i].bottom << " "
                            << scene.cameras[i].top << "\n";
        cout << "\tNear Distance: " << scene.cameras[i].near_distance << "\n";
        cout << "\tH-res V-res: " << scene.cameras[i].h_res << " "
                            << scene.cameras[i].v_res << "\n";
        cout << "\tImage Name: " << scene.cameras[i].image_name << "\n\n";
    }

    cout << "Ambient Light: " << scene.ambient_light << "\n";
    for(int i = 0; i < scene.lights.size(); ++i) {
        cout << "Light[" << i << "]:\n";
        cout << "\tID: " << scene.lights[i].id << "\n";
        cout << "\tPosition: " << scene.lights[i].position << "\n";
        cout << "\tIntensity: " << scene.lights[i].intensity << "\n\n";
    }

    for(int i = 0; i < scene.materials.size(); ++i) {
        cout << "Material[" << i << "]:\n";
        cout << "\tID: " << scene.materials[i].id << "\n";
        cout << "\tAmbient: " << scene.materials[i].ambient << "\n";
        cout << "\tDiffuse: " << scene.materials[i].diffuse << "\n";
        cout << "\tSpecular: " << scene.materials[i].specular << "\n";
        cout << "\tMirror: " << scene.materials[i].mirror_reflectance << "\n";
        cout << "\tPhong: " << scene.materials[i].phong_exponent << "\n\n";
    }

    cout << "VertexData: \n";
    for(int i = 0; i < scene.vertexdata.size(); ++i) {
        cout << "\t" << scene.vertexdata[i] << "\n";
    }

    cout << "\nObjects: \n";
    for(auto object : scene.objects) {
        cout << "\tType: " << object->getType() << std::endl;
        cout << "\tID: " << object->id << std::endl;
        cout << "\tMaterial: \n";
        cout << "\t\tAmbient: " << object->material.ambient << "\n";
        cout << "\t\tDiffuse: " << object->material.diffuse << "\n";
        cout << "\t\tSpecular: " << object->material.specular << "\n";
        cout << "\t\tMirror: " << object->material.mirror_reflectance << "\n";
        cout << "\t\tPhong: " << object->material.phong_exponent << " \n";

        if(object->getType() == "Mesh") {
            Mesh* mesh = dynamic_cast<Mesh*>(object);
            cout << "\tFaces: \n";
            for(int i = 0; i < mesh->faces.size(); ++i) {
                cout << "\t" << mesh->faces[i][0] << " " << mesh->faces[i][1] << " " << mesh->faces[i][2] << "\n";
            }
        }
        else if(object->getType() == "Triangle") {
            Triangle* triangle = dynamic_cast<Triangle*>(object);
            cout << "\tFace: " 
                    << triangle->coords[0] << " " << triangle->coords[1] << " " << triangle->coords[2] << "\n";
        }
        else if(object->getType() == "Sphere") {
            Sphere* sphere = dynamic_cast<Sphere*>(object);
            cout << "\tCenter: " << sphere->center << "\n";
            cout << "\tRadius: " << sphere->radius << "\n";
        }
        cout << "\n";
    }
}

/////////////////////
// Private methods //
/////////////////////

RGB SceneBuilder::trace(const Ray &ray, int depth)
{
    Hit hit;
    bool hit_found = false;
    // Loop all objects in the scene
    for(auto obj : scene.objects) {
        Hit obj_hit = obj->intersect(ray);
        // If hit found
        if(obj_hit.is_hit() && (!hit_found || obj_hit.t < hit.t)) {
            // Closest hit
            hit = obj_hit;
            hit_found = true;
        }
    }

    if(hit_found) {
        RGB total_color(0, 0, 0);
        for(auto& light : scene.lights) {
            RGB light_color = shade(ray, hit, light, depth);
            total_color = total_color + light_color;
        }
        return total_color;
    }
    else {
        return scene.background_color;
    }
}

RGB SceneBuilder::shade(const Ray& ray, const Hit &hit, const PointLight &light, int depth) {
    if(depth > scene.max_raytracedepth) {
        return RGB(0, 0, 0);
    }
    
    Color curr_light(hit.material.ambient.e[0] * scene.ambient_light.r,
                    hit.material.ambient.e[1] * scene.ambient_light.g,
                    hit.material.ambient.e[2] * scene.ambient_light.b);

    Vector light_direction = light.position - hit.hit_point;
    light_direction = light_direction.normalize();
    double distance_to_light = light_direction.length();

    // Shadow check
    Ray shadow_ray(hit.hit_point, light_direction);
    bool in_shadow = false;
    for(auto obj : scene.objects) {
        Hit shadow_hit = obj->intersect(shadow_ray);
        if(shadow_hit.is_hit() && shadow_hit.t < distance_to_light) {
            in_shadow = true;
            break;
        }
    }

    if(!in_shadow) {
        // Diffuse reflectance
        double diffuse_factor = std::max(0.0, light_direction.dot(hit.normal));
        double cosine = diffuse_factor / (hit.normal.length() * light_direction.length());
        curr_light += (light.intensity / (distance_to_light * distance_to_light)) * cosine * hit.material.diffuse;
        
        // Specular reflectance
        Vector reflection = (light_direction * -1.0) - hit.normal * (light_direction.dot(hit.normal)) * 2;
        Vector view_direction = ray.direction() * -1.0;
        double specular_factor = std::pow(std::max(0.0, reflection.dot(view_direction)), hit.material.phong_exponent);

        curr_light += (light.intensity / (distance_to_light * distance_to_light)) * specular_factor * hit.material.specular;

        // Mirror reflectance
        if(depth < scene.max_raytracedepth && (hit.material.mirror_reflectance.x() > 0 || hit.material.mirror_reflectance.y() > 0 || hit.material.mirror_reflectance.z() > 0)) {
            Vector reflect_dir = ray.direction() - hit.normal * 2.0 * ray.direction().dot(hit.normal);
            reflect_dir = reflect_dir.normalize();
            Ray reflect_ray(hit.hit_point + reflect_dir * 1e-4, reflect_dir);
            RGB reflect_color = trace(reflect_ray, depth + 1);

            curr_light += Color(reflect_color.r / 255.0, reflect_color.g / 255.0, reflect_color.b / 255.0) * hit.material.mirror_reflectance;
        }
        
    }
    
    for(int i = 0; i < 3; ++i) {
        if(curr_light.e[i] > 255) {
            curr_light.e[i] = 255;
        }
        else if(curr_light.e[i] < 0) {
            curr_light.e[i] = 0;
        }
    }

    return RGB(static_cast<short>(curr_light.e[0]), 
                static_cast<short>(curr_light.e[1]), 
                static_cast<short>(curr_light.e[2]));
}


void SceneBuilder::parseScene(tinyxml2::XMLDocument &xmlDoc)
{
    tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Scene");

    parseMaxRayTraceDepth(root);
    parseBackgroundColor(root);
    parseCameras(root);
    parseLights(root);
    parseMaterials(root);
    parseVertexData(root);
    parseObjects(root);
}

void SceneBuilder::parseMaxRayTraceDepth(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* max_depth_element = root->FirstChildElement("maxraytracedepth");
    if (max_depth_element) {
        int max_ray_trace_depth = 0;
        if (max_depth_element->QueryIntText(&max_ray_trace_depth) == tinyxml2::XML_SUCCESS) {
            this->scene.max_raytracedepth = max_ray_trace_depth;
        }
    }
}
void SceneBuilder::parseBackgroundColor(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* RGBElement = root->FirstChildElement("BackgroundColor");
    if (RGBElement) {
        const char* RGBValue = RGBElement->GetText();
        if (RGBValue) {
            std::istringstream ss(RGBValue);
            short r, g, b;
            if (ss >> r >> g >> b) {
                this->scene.background_color = RGB(r, g, b);
            }
        }
    }
}

void SceneBuilder::parseCameras(tinyxml2::XMLElement* root) {
    //All cameras
    tinyxml2::XMLElement* camerasElement = root->FirstChildElement("Cameras");
    //First camera
    tinyxml2::XMLElement* cameraElement = camerasElement->FirstChildElement("Camera");
    while (cameraElement != nullptr) {
        parseCamera(cameraElement);
        //Next camera
        cameraElement = cameraElement->NextSiblingElement("Camera");
    }
}

void SceneBuilder::parseCamera(tinyxml2::XMLElement* camera_element) {
    Camera curr_camera;
    //id
    curr_camera.id = camera_element->IntAttribute("id");

    //Position
    tinyxml2::XMLElement* pos_element = camera_element->FirstChildElement("Position");
    std::istringstream iss(pos_element->GetText());
    iss >> curr_camera.position.e[0] >> curr_camera.position.e[1] >> curr_camera.position.e[2];

    //Gaze
    tinyxml2::XMLElement* gaze_element = camera_element->FirstChildElement("Gaze");
    iss.clear();
    iss.str(gaze_element->GetText());
    iss >> curr_camera.gaze.e[0] >> curr_camera.gaze.e[1] >> curr_camera.gaze.e[2];

    //Up
    tinyxml2::XMLElement* up_element = camera_element->FirstChildElement("Up");
    iss.clear();
    iss.str(up_element->GetText());
    iss >> curr_camera.up.e[0] >> curr_camera.up.e[1] >> curr_camera.up.e[2];

    //NearPlane
    tinyxml2::XMLElement* nearp_element = camera_element->FirstChildElement("NearPlane");
    iss.clear();
    iss.str(nearp_element->GetText());
    iss >> curr_camera.left >> curr_camera.right >> curr_camera.bottom >> curr_camera.top;

    //NearDistance
    tinyxml2::XMLElement* neard_element = camera_element->FirstChildElement("NearDistance");
    curr_camera.near_distance = neard_element->DoubleText();

    //ImageResolution
    tinyxml2::XMLElement* res_element = camera_element->FirstChildElement("ImageResolution");
    iss.clear();
    iss.str(res_element->GetText());
    iss >> curr_camera.h_res >> curr_camera.v_res;

    //ImageName
    tinyxml2::XMLElement* iname_element = camera_element->FirstChildElement("ImageName");
    curr_camera.image_name = iname_element->GetText();

    scene.cameras.push_back(curr_camera);
}

void SceneBuilder::parseLights(tinyxml2::XMLElement* root) {
// From root
    tinyxml2::XMLElement* lights_element = root->FirstChildElement("Lights");

// AmbientLight
    tinyxml2::XMLElement* amblight_element = lights_element->FirstChildElement("AmbientLight");
    std::istringstream iss(amblight_element->GetText());
    iss >> scene.ambient_light.r >> scene.ambient_light.g >> scene.ambient_light.b;

// Multiple PointLights
    // First PointLight
    tinyxml2::XMLElement* pointlight_element = lights_element->FirstChildElement("PointLight");
    while (pointlight_element != nullptr) {
        parsePointLight(pointlight_element);
        // Next PointLight
        pointlight_element = pointlight_element->NextSiblingElement("PointLight");
    }
    
}

void SceneBuilder::parsePointLight(tinyxml2::XMLElement* light_element) {
    PointLight curr_pl;

    // id
    curr_pl.id = light_element->IntAttribute("id");

    // Position
    tinyxml2::XMLElement* pos_element = light_element->FirstChildElement("Position");
    std::istringstream iss(pos_element->GetText());
    iss >> curr_pl.position.e[0] >> curr_pl.position.e[1] >>  curr_pl.position.e[2];

    // Intensity
    tinyxml2::XMLElement* int_element = light_element->FirstChildElement("Intensity");
    iss.clear();
    iss.str(int_element->GetText());
    iss >> curr_pl.intensity.e[0] >> curr_pl.intensity.e[1] >>  curr_pl.intensity.e[2];
    
    scene.lights.push_back(curr_pl);
}

void SceneBuilder::parseMaterials(tinyxml2::XMLElement* root) {
    //All materials
    tinyxml2::XMLElement* materials_element = root->FirstChildElement("Materials");

    //First material
    tinyxml2::XMLElement* material_element = materials_element->FirstChildElement("Material");
    while (material_element != nullptr) {
        parseMaterial(material_element);
        //Next material
        material_element = material_element->NextSiblingElement("Material");
    }
}

void SceneBuilder::parseMaterial(tinyxml2::XMLElement* material_element) {
    Material curr_material;

    // id
    curr_material.id = material_element->IntAttribute("id");

    // AmbientReflectance
    tinyxml2::XMLElement* ambref_element = material_element->FirstChildElement("AmbientReflectance");
    std::istringstream iss(ambref_element->GetText());
    iss >> curr_material.ambient.e[0] >> curr_material.ambient.e[1] >> curr_material.ambient.e[2];

    // DiffuseReflectance
    tinyxml2::XMLElement* difref_element = material_element->FirstChildElement("DiffuseReflectance");
    iss.clear();
    iss.str(difref_element->GetText());
    iss >> curr_material.diffuse.e[0] >> curr_material.diffuse.e[1] >> curr_material.diffuse.e[2];

    // SpecularReflectance
    tinyxml2::XMLElement* specref_element = material_element->FirstChildElement("SpecularReflectance");
    iss.clear();
    iss.str(specref_element->GetText());
    iss >> curr_material.specular.e[0] >> curr_material.specular.e[1] >> curr_material.specular.e[2];

    // MirrorReflectance
    tinyxml2::XMLElement* mirref_element = material_element->FirstChildElement("MirrorReflectance");
    iss.clear();
    iss.str(mirref_element->GetText());
    iss >> curr_material.mirror_reflectance.e[0] >> curr_material.mirror_reflectance.e[1] >> curr_material.mirror_reflectance.e[2];

    // PhongExponent
    tinyxml2::XMLElement* phexp_element = material_element->FirstChildElement("PhongExponent");
    curr_material.phong_exponent = phexp_element->DoubleText();

    scene.materials.push_back(curr_material);
}

void SceneBuilder::parseVertexData(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* vertex_element = root->FirstChildElement("VertexData");
    std::istringstream iss(vertex_element->GetText());

    double x, y, z;
    while(iss >> x >> y >> z) {
        Vector temp(x, y, z);
        scene.vertexdata.push_back(temp);
    }
}

void SceneBuilder::parseObjects(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* objects_element = root->FirstChildElement("Objects");

    for (tinyxml2::XMLElement* object_element = objects_element->FirstChildElement(); object_element; object_element = object_element->NextSiblingElement()) {
        std::string object_type = object_element->Name();
        if(object_type == "Mesh") {
            parseMesh(object_element);
        }
        else if(object_type == "Triangle") {
            parseTriangle(object_element);
        }
        else if(object_type == "Sphere") {
            parseSphere(object_element);
        }
    }
}

void SceneBuilder::parseMesh(tinyxml2::XMLElement* mesh_element) {
    Mesh *curr_mesh = new Mesh();

    // id
    curr_mesh->id = mesh_element->IntAttribute("id");

    // Material
    tinyxml2::XMLElement* mat_element = mesh_element->FirstChildElement("Material");
    int material_id = mat_element->IntText();
    curr_mesh->material = scene.materials[material_id - 1];

    // Faces
    tinyxml2::XMLElement* faces_element = mesh_element->FirstChildElement("Faces");
    std::istringstream iss(faces_element->GetText());

    int x, y, z;
    while(iss >> x >> y >> z) {
        std::array<Point, 3> temp;
        temp[0] = scene.vertexdata[x - 1];
        temp[1] = scene.vertexdata[y - 1];
        temp[2] = scene.vertexdata[z - 1];
        curr_mesh->faces.push_back(temp);
    }

    scene.objects.push_back(curr_mesh);
}

void SceneBuilder::parseTriangle(tinyxml2::XMLElement* triangle_element) {
    Triangle* curr_triangle = new Triangle();

    // id
    curr_triangle->id = triangle_element->IntAttribute("id");

    // Material
    tinyxml2::XMLElement* mat_element = triangle_element->FirstChildElement("Material");
    int material_id = mat_element->IntText();
    curr_triangle->material = scene.materials[material_id - 1];

    // Indices
    tinyxml2::XMLElement* indice_element = triangle_element->FirstChildElement("Indices");
    std::istringstream iss(indice_element->GetText());
    int x, y, z;
    iss >> x >> y >> z;

    curr_triangle->coords[0] = scene.vertexdata[x - 1];
    curr_triangle->coords[1] = scene.vertexdata[y - 1];
    curr_triangle->coords[2] = scene.vertexdata[z - 1];

    scene.objects.push_back(curr_triangle);
}

void SceneBuilder::parseSphere(tinyxml2::XMLElement* sphere_element) {
    Sphere* curr_sphere = new Sphere();

    // id
    curr_sphere->id = sphere_element->IntAttribute("id");

    // Material
    tinyxml2::XMLElement* mat_element = sphere_element->FirstChildElement("Material");
    int material_id = mat_element->IntText();
    curr_sphere->material = scene.materials[material_id - 1];

    // Center
    tinyxml2::XMLElement* center_element = sphere_element->FirstChildElement("Center");
    int center_idx = center_element->IntText();
    curr_sphere->center = Point(scene.vertexdata[center_idx - 1]);

    // Radius
    tinyxml2::XMLElement* radius_element = sphere_element->FirstChildElement("Radius");
    curr_sphere->radius = radius_element->DoubleText();

    scene.objects.push_back(curr_sphere);
}
