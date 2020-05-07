#pragma once


const glm::mat4 mat4identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

struct Entity {
	unsigned int index;
	unsigned int id;
	Model* model3d;
	Shader* shader;
	glm::vec3 position;
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 matModel = mat4identity;
};

struct SpotLight {
	unsigned int id;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
	float innercone;
	float outercone;
	float intensity_constant = 0.02f;
	float intensity_linear = 1.0f;
	float intensity_quadratic = 0.032f;
};

struct PointLight {
	unsigned int id;
	glm::vec3 position = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ambient = glm::vec3(0.01f, 0.01f, 0.01f);
	float intensity_constant = 1.0f;
	float intensity_linear = 0.5f;
	float intensity_quadratic = 0.1f;
};

struct DirectionalLight {
	unsigned int id;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
};

struct Scene {
	int id;
	std::vector<Entity> entities;
	std::vector<SpotLight> spotLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	//vector<LightEntity> lights;
};

