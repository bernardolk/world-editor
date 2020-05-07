#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
//
#include <glm/ext/vector_float2.hpp> // vec2
#include <glm/ext/vector_float3.hpp> // vec3
#include <glm/ext/matrix_float4x4.hpp> // mat4x4
#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale, identity
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
//
#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//

#include <Shader.h>
#include <Mesh.h>
#include <Model.h>
#include <Camera.h>
#include <Entities.h>

//
//
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
//
using namespace std;
using namespace glm;


const float PI = 3.141592;


////______________ ..:;Shader settings;:.. ______________
float global_shininess = 32.0f;

// OpenGL objects
unsigned int texture, texture_specular;
Shader quad_shader, model_shader;

//______________ ..:;Method prototypes;:.. ______________
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void onMouseMove(GLFWwindow* window, double xpos, double ypos);
void render_model(Entity ent, glm::vec3 lightPos[], glm::vec3 lightRgb[]);
void setup_window(bool debug);
void onMouseBtn(GLFWwindow* window, int button, int action, int mods);
void render_ray();
void render_scene();
void update_scene_objects();
void update_entity(Entity* entity);
void render_scene_lights();
GLenum glCheckError_(const char* file, int line);


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
const float viewportWidth = 1200;
const float viewportHeight = 675;


const string textures_path = "assets/textures/";
const string models_path = "assets/models/";
const string fonts_path = "assets/fonts/";


#include <Editor.h>




int main() {
	//______________ ..:;Initial GLFW and GLAD setups;:.. ______________


	setup_window(true);
	editor_initialize(viewportWidth, viewportHeight);

	//______________ ..:;Shaders;:.. ______________
	glEnable(GL_DEPTH_TEST);

	// Main shaders
	Shader model_shader("shaders/vertex_model.shd", "shaders/fragment_model.shd");
	model_shader = Shader("shaders/vertex_model.shd", "shaders/fragment_multiple_lights.shd");
	//Shader cube_shader("shaders/vertex_main.shd", "shaders/fragment_main.shd");
	Shader obj_shader("shaders/vertex_color_cube.shd", "shaders/fragment_multiple_lights.shd");
	Shader light_shader("shaders/vertex_color_cube.shd", "shaders/fragment_light.shd");
	quad_shader = Shader("shaders/quad_vertex.shd", "shaders/textured_quad_fragment.shd");


	// Text shaders (GUI)
	//Shader text_shader = initialize_text_shader();
	load_text_textures("Consola.ttf", 12);

	//______________ ..:;Creates scene;:.. ______________
	Scene demo_scene;
	demo_scene.id = 1;

	Model m_sponza = Model("C:/World Editor Assets/Models/crytek-sponza/sponza_nobanner.obj");
	Entity sponza{
		entity_counter,
		++entity_counter,
		&m_sponza,
		&model_shader,
		vec3(0,0,0)
	};
	sponza.matModel = scale(mat4identity, vec3(0.01f,0.01f,0.01f));
	demo_scene.entities.push_back(sponza);


	// Quad model tests
	unsigned int brick_texture = load_texture_from_file("brickwall.jpg", "assets/textures");
	unsigned int brick_normal_texture = load_texture_from_file("brickwall_normal.jpg", "assets/textures");
	Texture quad_wall_texture{
		brick_texture,
		"texture_diffuse",
		"whatever"
	};
	Texture quad_wall_normal_texture{
		brick_normal_texture,
		"texture_normal",
		"whatever"
	};
	vector<Texture> texture_vec;
	texture_vec.push_back(quad_wall_texture);
	texture_vec.push_back(quad_wall_normal_texture);
	Mesh quad_mesh = Mesh(quad_vertex_vec, quad_vertex_indices, texture_vec);
	Model quad_model(quad_mesh);

	quad_model.textures_loaded = texture_vec;

	Entity quad_wall{
		entity_counter,
		++entity_counter,
		&quad_model,
		&model_shader,
		vec3(-10,0,-20),
		vec3(0),
		vec3(7.0f,10.0f,1.0f)
	};
	demo_scene.entities.push_back(quad_wall);

	// LightSources
	PointLight l1;
	l1.id = 1;
	l1.position = vec3(11, 10.5, 0.4);
	l1.diffuse = vec3(0.6, 0.6, 0.6);
	l1.intensity_linear = 0.01f;
	l1.intensity_quadratic = 0.001f;
	demo_scene.pointLights.push_back(l1);

	PointLight l2;
	l2.id = 2;
	l2.position = vec3(0, 2.0, -18);
	l2.diffuse = vec3(0.0, 0.3, 0.0);
	l1.intensity_linear = 0.01f;
	l1.intensity_quadratic = 0.001f;
	demo_scene.pointLights.push_back(l2);

	PointLight l3;
	l3.id = 3;
	l3.position = vec3(-7.0, 11.25, -7.7);
	l3.diffuse = vec3(0.0, 0.0, 0.7);
	l1.intensity_linear = 0.01f;
	l1.intensity_quadratic = 0.001f;
	demo_scene.pointLights.push_back(l3);

	PointLight l4;
	l4.id = 4;
	l4.position.x = -1;
	l4.position.z = -1;
	l4.position.y = 9;
	l4.diffuse = vec3(0.9, 0.9, 0.9);
	demo_scene.pointLights.push_back(l4);

	PointLight l5;
	l5.id = 5;
	l5.position.x = 5;
	l5.position.y = 9;
	l5.diffuse = vec3(0.9, 0.9, 0.9);
	demo_scene.pointLights.push_back(l5);

	active_scene = &demo_scene;


	//______________ ..,;:Main Loop:;,.. ______________
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		current_fps = 1.0f / deltaTime;

		//				..,;:Input phase:;,..
		glfwPollEvents();
		editor_start_frame();
		processInput(window);


		//				..,;:Update phase:;,..
		editor_update();
		camera_update(active_camera, viewportWidth, viewportHeight);
		update_scene_objects();


		//				..,;:Render phase:;,..
		glClearColor(0.196, 0.298, 0.3607, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_scene();

		if (!moveMode)
			render_scene_lights();

		//if (render_pickray)
		//	render_ray();

		editor_loop();

		editor_end_frame();	
		glfwSwapBuffers(window);
	}

	editor_terminate();
	glfwTerminate();
	return 0;
}


void update_scene_objects() {
	auto it = active_scene->entities.begin();
	auto end = active_scene->entities.end();
	for (it; it < end; it++) {
		Entity& entity_ref = *it;
		update_entity(&entity_ref);
	}
}

void update_entity(Entity* entity) {
	// Updates model matrix;
	mat4 model = translate(mat4identity, entity->position);
	model = rotate(model, radians(entity->rotation.x), vec3(1.0f, 0.0f, 0.0f));
	model = rotate(model, radians(entity->rotation.y), vec3(0.0f, 1.0f, 0.0f));
	model = rotate(model, radians(entity->rotation.z), vec3(0.0f, 0.0f, 1.0f));
	model = scale(model, entity->scale);
	entity->matModel = model;
}

void setup_window(bool debug) {
	// Setup the window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates the window
	window = glfwCreateWindow(viewportWidth, viewportHeight, "World Editor", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Setups openGL viewport
	glViewport(0, 0, viewportWidth, viewportHeight);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, onMouseMove);
	//glfwSetScrollCallback(window, onMouseScroll);
	//glfwSetMouseButtonCallback(window, onMouseBtn);

	if (debug) {
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	}

}

void render_scene_lights() {
	auto it = active_scene->pointLights.begin();
	auto end = active_scene->pointLights.end();
	for (it; it != end; it++) {
		quad_shader.use();
		quad_shader.setMatrix4("view", active_camera.View4x4);
		quad_shader.setMatrix4("projection", active_camera.Projection4x4);

		vec3 light_norm = active_camera.Position - it->position;
		float angle_pitch = atan(light_norm.x / light_norm.z);

		glm::mat4 model_m = glm::translate(mat4identity, it->position);
		model_m = rotate(model_m, angle_pitch, active_camera.Up);
		model_m = glm::scale(model_m, vec3(light_icons_scaling, light_icons_scaling * 1.5f, light_icons_scaling));
		quad_shader.setMatrix4("model", model_m);

		glBindVertexArray(quad_vao);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, quad_lightbulb_texture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

void render_ray() {

	//float raymesh[6] = {
	//			active_camera.Position.x, active_camera.Position.y, active_camera.Position.z,
	//			pickray_p1.x, pickray_p1.y, pickray_p1.z
	//};

	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(raymesh), raymesh, GL_STATIC_DRAW);

	//grid_shader.use();
	//grid_shader.setMatrix4("view", view);
	//grid_shader.setMatrix4("projection", projection);
	//grid_shader.setMatrix4("model", mat4identity);
	//glDrawArrays(GL_LINES, 0, 2);
}

void render_model(Entity ent, glm::vec3 lightPos[], glm::vec3 lightRgb[]) {
	ent.shader->use();
	ent.shader->setMatrix4("view", active_camera.View4x4);
	ent.shader->setMatrix4("projection", active_camera.Projection4x4);

	// point lights
	for (int i = 0; i < 4; i++) {
		glm::vec3 lightVector = lightPos[i];
		string uniform_name = "pointLights[" + to_string(i) + "]";

		ent.shader->setFloat3(uniform_name + ".position", lightVector);
		ent.shader->setFloat3(uniform_name + ".ambient", 0.002f, 0.002f, 0.002f);
		ent.shader->setFloat3(uniform_name + ".diffuse", lightRgb[i]);
		ent.shader->setFloat3(uniform_name + ".specular", 1.0f, 1.0f, 1.0f);
		ent.shader->setFloat(uniform_name + ".constant", 1.0f);
		ent.shader->setFloat(uniform_name + ".linear", 0.5f);
		ent.shader->setFloat(uniform_name + ".quadratic", 0.1f);
	}

	ent.shader->setFloat3("dirLights[0].ambient", 0.00f, 0.005f, 0.001f);
	ent.shader->setFloat3("dirLights[0].diffuse", 0.2f, 0.2f, 0.2f);
	ent.shader->setFloat3("dirLights[0].specular", 0.5f, 0.5f, 0.5f);
	ent.shader->setFloat3("dirLights[0].direction", -1.0f, -1.0f, 0.0f);

	//shader.setFloat3("dirLights[1].ambient", 0.00f, 0.001f, 0.005f);
	//shader.setFloat3("dirLights[1].diffuse", 0.0f, 0.05f, 0.2f);
	//shader.setFloat3("dirLights[1].specular", 0.0f, 0.1f, 0.4f);
	//shader.setFloat3("dirLights[1].direction", 1.0f, 1.0f, 0.0f);

	ent.shader->setFloat("spotLights[0].innercone", glm::cos(glm::radians(12.5f)));
	ent.shader->setFloat("spotLights[0].outercone", glm::cos(glm::radians(20.0f)));
	ent.shader->setFloat("spotLights[0].constant", 1.0f);
	ent.shader->setFloat("spotLights[0].linear", 0.02f);
	ent.shader->setFloat("spotLights[0].quadratic", 0.032f);
	ent.shader->setFloat3("spotLights[0].direction", active_camera.Front);
	ent.shader->setFloat3("spotLights[0].position", active_camera.Position);
	ent.shader->setFloat3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
	ent.shader->setFloat3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
	ent.shader->setFloat3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);

	ent.shader->setFloat3("viewPos", active_camera.Position.x, active_camera.Position.y, active_camera.Position.z);
	ent.shader->setFloat("shininess", global_shininess);


	// render the loaded model
	glm::mat4 model_trans = glm::translate(mat4identity, ent.position);
	ent.shader->setMatrix4("model", model_trans);
	ent.model3d->Draw(*ent.shader);

}

void render_scene() {
	vector<Entity>::iterator entity_ptr = active_scene->entities.begin();
	for (entity_ptr; entity_ptr != active_scene->entities.end(); entity_ptr++) {
		entity_ptr->shader->use();
		vector<PointLight>::iterator point_light_ptr = active_scene->pointLights.begin();
		int point_light_count = 0;
		for (point_light_ptr;point_light_ptr != active_scene->pointLights.end();point_light_ptr++) {
			PointLight point_light = *point_light_ptr;
			string uniform_name = "pointLights[" + to_string(point_light_count) + "]";
			entity_ptr->shader->setFloat3(uniform_name + ".position", point_light.position);
			entity_ptr->shader->setFloat3(uniform_name + ".diffuse", point_light.diffuse);
			entity_ptr->shader->setFloat3(uniform_name + ".specular", point_light.specular);
			entity_ptr->shader->setFloat3(uniform_name + ".ambient", point_light.ambient);
			entity_ptr->shader->setFloat(uniform_name + ".constant", point_light.intensity_constant);
			entity_ptr->shader->setFloat(uniform_name + ".linear", point_light.intensity_linear);
			entity_ptr->shader->setFloat(uniform_name + ".quadratic", point_light.intensity_quadratic);
			point_light_count++;
		}
		entity_ptr->shader->setInt("num_point_lights", point_light_count);
		entity_ptr->shader->setInt("num_directional_light", 0);
		entity_ptr->shader->setInt("num_spot_lights", 0);
		entity_ptr->shader->setMatrix4("view", active_camera.View4x4);
		entity_ptr->shader->setMatrix4("projection", active_camera.Projection4x4);
		entity_ptr->shader->setFloat("shininess", global_shininess);
		entity_ptr->shader->setFloat3("viewPos", active_camera.Position);
		mat4 model_matrix = scale(mat4identity, vec3(0.01,0.01,0.01));
		entity_ptr->shader->setMatrix4("model", model_matrix);
		entity_ptr->model3d->Draw(*entity_ptr->shader);
	}
}

void processInput(GLFWwindow* window)
{
	float cameraSpeed = deltaTime * active_camera.Acceleration;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			cameraSpeed = deltaTime * active_camera.Acceleration * 2;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			active_camera.Position += cameraSpeed * active_camera.Front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			active_camera.Position -= cameraSpeed * active_camera.Front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			active_camera.Position -= cameraSpeed * glm::normalize(glm::cross(active_camera.Front, active_camera.Up));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			active_camera.Position += cameraSpeed * glm::normalize(glm::cross(active_camera.Front, active_camera.Up));
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			active_camera.Position -= cameraSpeed * active_camera.Up;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			active_camera.Position += cameraSpeed * active_camera.Up;
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			camera_look_at(active_camera, glm::vec3(0.0f, 0.0f, 0.0f), true);
			resetMouseCoords = true;
		}
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			global_shininess -= 10 * deltaTime;
			if (global_shininess < 1)
				global_shininess = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			global_shininess += 10 * deltaTime;



		// Toggle GUI (substitute all checks for just one varible holding last key pressed (or keys)
		// then when released, set it to null or zero or something.
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !GUI_btn_down) {
			GUI_btn_down = true;
			if (show_GUI)
				show_GUI = false;
			else show_GUI = true;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE && GUI_btn_down)
			GUI_btn_down = false;

	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keyComboPressed) {
		if (moveMode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			moveMode = false;
			keyComboPressed = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			moveMode = true;
			resetMouseCoords = true;
			keyComboPressed = true;
		}
	}
	// This solution will only work while i have only one key combo implemented (i guess)
	if (keyComboPressed) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
			keyComboPressed = false;
	}

}

void onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (editor_mode) {
		editor_process_input_mouse_move(xpos, ypos);
	}
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (editor_mode) {
		if (!ImGui::GetIO().WantCaptureMouse) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
				if (active_camera.FOVy <= 45.0f && active_camera.FOVy >= 1.0f)
					active_camera.FOVy -= yoffset * 3;
				if (active_camera.FOVy < 1.0f)
					active_camera.FOVy = 1.0f;
				if (active_camera.FOVy > 45.0f)
					active_camera.FOVy = 45.0f;
			}
			else {
				active_camera.Position += (float)(3 * yoffset) * active_camera.Front;
			}
		}
	}
}

void onMouseBtn(GLFWwindow* window, int button, int action, int mods) {
	if (editor_mode) {
		editor_process_input_mouse_btn(button, action);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			//case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			//case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
