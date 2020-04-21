#pragma once

#include <vector>
#include <Entities.h>
#include <glfw/glfw3.h>

struct Scene {
	int id;
	std::vector<Entity> entities;
	std::vector<SpotLight> spotLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	//vector<LightEntity> lights;
};

GLFWwindow* window;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int frameCounter = 0;
float current_fps;
bool editor_mode = true;
double currentMouseX;
double currentMouseY;
Scene* active_scene;
Camera active_camera;
