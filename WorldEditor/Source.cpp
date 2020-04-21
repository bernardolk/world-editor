

//#define NOMINMAX
//#include <Windows.h>
//#include <Shobjidl.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>

#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WGL
//#define GLFW_EXPOSE_NATIVE_WIN32 
//#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/ext/vector_float2.hpp> // vec2
#include <glm/ext/vector_float3.hpp> // vec3
#include <glm/ext/matrix_float4x4.hpp> // mat4x4
#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale, identity
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <stdio.h>

#include <Shader.h>
#include <Mesh.h>
#include <Model.h>
#include <Camera.h>
#include <Editor.h>
#include <Entities.h>
#include <Game.h>


#define glCheckError() glCheckError_(__FILE__, __LINE__) 

using namespace std;
using namespace glm;


const float PI = 3.141592;
const string textures_path = "assets/textures/";
const string models_path = "assets/models/";
const string fonts_path = "assets/fonts/";
const float viewportWidth = 1200;
const float viewportHeight = 675;

//______________ ..:;Camera;:.. ______________
Camera camera;

double lastXMouseCoord;
double lastYMouseCoord;
mat4 model, view, projection;

//______________ ..:;Shader settings;:.. ______________
float global_shininess = 32.0f;

// OpenGL objects
unsigned int texture, texture_specular;


unsigned int bounding_box_indices[] = {
	//Front face
	0,1,11,
	11,9,0,
	//back-left
	1,4,13,
	1,11,13,
	//back-right
	4,13,6,
	6,15,13,
	//right
	0,9,6,
	6,15,9,
	//top
	11,13,9,
	9,13,15,
	//bottom
	0,1,4,
	0,4,6
};

float quad[] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

vector<Vertex> quad_vertex_vec = {
	Vertex{vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 0.0f, 1.0f),vec2(0.0f, 0.0f)},
	Vertex{vec3(1.0f, 0.0f, 0.0f),vec3(0.0f, 0.0f, 1.0f),vec2(1.0f, 0.0f)},
	Vertex{vec3(1.0f, 1.0f, 0.0f),vec3(0.0f, 0.0f, 1.0f),vec2(1.0f, 1.0f)},
	Vertex{vec3(0.0f, 1.0f, 0.0f),vec3(0.0f, 0.0f, 1.0f),vec2(0.0f, 1.0f)}
};


unsigned int quad_indices[] = {
	0,1,2,
	2,3,0
};

vector<unsigned int> quad_vertex_indices = { 0,1,2,2,3,0 };

unsigned int quad_vao, quad_vbo, quad_ebo, quad_lightbulb_texture;
Shader quad_shader, model_shader;

//______________ ..:;Global state;:.. ______________

vec3 pickray_collision_point;
vec3 pickray_direction;
bool render_pickray = false;
unsigned int buf, vao, vbo, entity_counter = 0;

//______________ ..:;Method prototypes;:.. ______________
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
//unsigned int loadAndBindTextureData(const char* filename);
void onMouseMove(GLFWwindow* window, double xpos, double ypos);
void load_text_textures(string font, int size);
//void render_cubes(Shader shader, glm::vec3 lightPos[], glm::vec3 lightRgb[]);
void render_model(Entity ent, glm::vec3 lightPos[], glm::vec3 lightRgb[]);
void render_light(Shader shader, glm::vec3 lightPos[], glm::vec3 lightRgb[]);
void editor_create_grid(int gridx, int gridy, float gridl);
void setup_window(bool debug);
void onMouseBtn(GLFWwindow* window, int button, int action, int mods);
void render_ray();
void editor_setup_textures();
float ray_triangle_intersection(glm::vec3 ray_origin, glm::vec3 ray_dir, glm::vec3 A, glm::vec3 B, glm::vec3 C, bool check_both_sides = false);
bool check_collision(Entity entity);
float check_box_collision(int entityIndex);
void render_scene();
void check_pickray_collision();
unsigned int gen_vertex_buffer(float buffer_array[]);
void show_entity_controls(int entityId);
void update_scene_objects();
void update_entity(Entity* entity);
void render_scene_lights();
void show_light_controls(int lightId);
GLenum glCheckError_(const char* file, int line);
void cast_pickray();



int main() {
	//______________ ..:;Initial GLFW and GLAD setups;:.. ______________

	setup_window(true);
	editor_initialize();

	// Setup mouse and camera initial settings
	lastXMouseCoord = viewportWidth / 2;
	lastYMouseCoord = viewportHeight / 2;

	//______________ ..:;Shaders;:.. ______________
	glEnable(GL_DEPTH_TEST);

	// Main shaders
	//Shader model_shader("shaders/vertex_model.shd", "shaders/fragment_model.shd");
	model_shader = Shader("shaders/vertex_model.shd", "shaders/fragment_multiple_lights.shd");
	//Shader cube_shader("shaders/vertex_main.shd", "shaders/fragment_main.shd");
	Shader obj_shader("shaders/vertex_color_cube.shd", "shaders/fragment_multiple_lights.shd");
	Shader light_shader("shaders/vertex_color_cube.shd", "shaders/fragment_light.shd");
	quad_shader = Shader("shaders/quad_vertex.shd", "shaders/textured_quad_fragment.shd");


	// Text shaders (GUI)
	Shader text_shader = initialize_text_shader();
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

	//______________ ..:;d'autres;:.. ______________
	// Point
	float point_buffer[]{ 0.0f, 0.0f, 0.0f };

	// Pickray render objects	
	float empty[6];
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(empty), 0, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Quad
	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);
	glGenBuffers(1, &quad_ebo);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	quad_lightbulb_texture = load_texture_from_file("lightbulb.png", textures_path);

	// initializate and loads textures for gui controls of editor mode
	editor_setup_textures();


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
		camera_update(camera, viewportWidth, viewportHeight);
		update_scene_objects();


		//				..,;:Render phase:;,..
		glClearColor(bg_color.x, bg_color.y, bg_color.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_scene();

		if (!moveMode)
			render_scene_lights();

		if (render_pickray)
			render_ray();

		if (pickray_collision_test)
			check_pickray_collision();

		if (entity_controls.selected_entity > -1 && !moveMode) {
			render_bounding_box(entity_controls.selected_entity);
			show_entity_controls(entity_controls.selected_entity);
		}
		if (entity_controls.selected_light > -1 && !moveMode) {
			show_light_controls(entity_controls.selected_light);
		}
		

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

// ..;: Setups :;..

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, onMouseMove);
	glfwSetScrollCallback(window, onMouseScroll);
	glfwSetMouseButtonCallback(window, onMouseBtn);

	if (debug) {
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	}

}

// ..;: Render calls :;..


unsigned int gen_vertex_buffer(float buffer_array[]) {
	unsigned int vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_array), 0, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return vao;
}

void render_scene_lights() {
	auto it = active_scene->pointLights.begin();
	auto end = active_scene->pointLights.end();
	for (it; it != end; it++) {
		quad_shader.use();
		quad_shader.setMatrix4("view", view);
		quad_shader.setMatrix4("projection", projection);

		vec3 light_norm = camera.Position - it->position;
		float angle_pitch = atan(light_norm.x / light_norm.z);

		glm::mat4 model_m = glm::translate(mat4identity, it->position);
		model_m = rotate(model_m, angle_pitch, camera.Up);
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
	ent.shader->setMatrix4("view", view);
	ent.shader->setMatrix4("projection", projection);

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
	ent.shader->setFloat3("spotLights[0].direction", camera.Front);
	ent.shader->setFloat3("spotLights[0].position", camera.Position);
	ent.shader->setFloat3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
	ent.shader->setFloat3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
	ent.shader->setFloat3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);

	ent.shader->setFloat3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
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
		entity_ptr->shader->setMatrix4("view", view);
		entity_ptr->shader->setMatrix4("projection", projection);
		entity_ptr->shader->setFloat("shininess", global_shininess);
		entity_ptr->shader->setFloat3("viewPos", camera.Position);
		mat4 model_matrix = scale(mat4identity, vec3(0.01,0.01,0.01));
		entity_ptr->shader->setMatrix4("model", model_matrix);
		entity_ptr->model3d->Draw(*entity_ptr->shader);
	}
}

void render_light(Shader shader, glm::vec3 lightPos[], glm::vec3 lightRgb[]) {
	glBindVertexArray(main_VAO);
	shader.use();
	shader.setMatrix4("view", view);
	shader.setMatrix4("projection", projection);

	for (int i = 0; i < 4; i++) {
		shader.setFloat3("color", lightRgb[i]);
		model = glm::translate(mat4identity, lightPos[i]);
		model = glm::scale(model, glm::vec3(0.05f));
		shader.setMatrix4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glCheckError();
		glGetError();
	}
}


// ..;: GUI and Controls :;..


// ..;: Collision :;..

// Returns the direction in world coordinates of a ray cast from a click on screen
void cast_pickray() {

	float screenX_normalized = (currentMouseX - viewportWidth / 2) / (viewportWidth / 2);
	float screenY_normalized = -1 * (currentMouseY - viewportHeight / 2) / (viewportHeight / 2);

	glm::vec4 ray_clip(screenX_normalized, screenY_normalized, -1.0, 1.0);
	glm::mat4 inv_view = glm::inverse(view);
	glm::mat4 inv_proj = glm::inverse(projection);
	glm::vec3 ray_eye_3 = (inv_proj * ray_clip);
	glm::vec4 ray_eye(ray_eye_3.x, ray_eye_3.y, -1.0, 0.0);
	glm::vec3 ray_world = glm::normalize(inv_view * ray_eye);

	pickray_direction = ray_world;
}

float check_box_collision(int entityIndex) {
	Entity entity = active_scene->entities[entityIndex];
	vector<float> bb = entity.model3d->boundingBox;
	int stride = 3;
	float shortest_distance = numeric_limits<float>::max();

	//@FixMe: I dont know how to optimize this yet, but right now i am checking every face of an object regardless.
	// Isn't there a better way? Maybe a simple filter for some faces, or something. The problem is that sometimes
	// a ray might intercept two faces of the object (or more) and we got to only consider the closest face.

	for (int i = 11; i >= 0; i--) {
		int bbi = i * stride;

		vec3 A = entity.matModel * vec4(bb[bounding_box_indices[bbi + 0] * stride],
			bb[bounding_box_indices[bbi + 0] * stride + 1],
			bb[bounding_box_indices[bbi + 0] * stride + 2], 1.0f);
		vec3 B = entity.matModel * vec4(bb[bounding_box_indices[bbi + 1] * stride],
			bb[bounding_box_indices[bbi + 1] * stride + 1],
			bb[bounding_box_indices[bbi + 1] * stride + 2], 1.0f);
		vec3 C = entity.matModel * vec4(bb[bounding_box_indices[bbi + 2] * stride],
			bb[bounding_box_indices[bbi + 2] * stride + 1],
			bb[bounding_box_indices[bbi + 2] * stride + 2], 1.0f);

		float dist = ray_triangle_intersection(active_camera.Position, pickray_direction, A, B, C, true);

		if (dist > 0 && dist < shortest_distance) {
			shortest_distance = dist;
		}
	}

	if (shortest_distance != numeric_limits<float>::max()) {
		pickray_collision_point = active_camera.Position + shortest_distance * pickray_direction;
		return shortest_distance;
	}
	else {
		return -1;
	}
}

float check_light_collision(int lightIndex) {
	PointLight point_light = active_scene->pointLights[lightIndex];

	//@FixMe: This should be inside the light struct or something similar and not be calculated in the render call nor here!
	vec3 light_norm = camera.Position - point_light.position;
	float angle_pitch = atan(light_norm.x / light_norm.z);
	glm::mat4 model_m = glm::translate(mat4identity, point_light.position);
	model_m = rotate(model_m, angle_pitch, camera.Up);
	model_m = glm::scale(model_m, vec3(light_icons_scaling, light_icons_scaling * 1.5f, light_icons_scaling));


	//@FixMe: This sucks so much i dont even know where to begin
	// This down here checks for a quad collision

	vec3 A1 = model_m * vec4(quad[0], quad[1], quad[2], 1.0f);
	vec3 B1 = model_m * vec4(quad[5], quad[6], quad[7], 1.0f);
	vec3 C1 = model_m * vec4(quad[10], quad[11], quad[12], 1.0f);
	float dist = ray_triangle_intersection(active_camera.Position, pickray_direction, A1, B1, C1, true);

	if (dist > 0) {
		pickray_collision_point = active_camera.Position + dist * pickray_direction;
		return dist;
	}

	vec3 A2 = model_m * vec4(quad[10], quad[11], quad[12], 1.0f);
	vec3 B2 = model_m * vec4(quad[15], quad[16], quad[17], 1.0f);
	vec3 C2 = model_m * vec4(quad[0], quad[1], quad[2], 1.0f);
	float dist = ray_triangle_intersection(active_camera.Position, pickray_direction, A2, B2, C2, true);

	if (dist > 0) {
		pickray_collision_point = active_camera.Position + dist * pickray_direction;
		return dist;
	}

	return -1;
}

bool check_collision(Entity entity) {

	//@optimization: can calculate on mesh load it's center and test ray-center distance so to try avoiding
	// testing collision again far away meshes inside model
	vector<Mesh>::iterator mesh = entity.model3d->meshes.begin();
	for (mesh; mesh != entity.model3d->meshes.end(); mesh++) {

		Mesh mesh_v = *mesh;

		glm::vec3 A;
		glm::vec3 B;
		glm::vec3 C;
		int counter = 0;
		vector<unsigned int>::iterator index = mesh_v.indices.begin();

		//@Attention: this wont work if the model is rotated in world space

		for (index; index != mesh_v.indices.end(); index++) {
			unsigned int i = *index;
			if (counter == 0) {
				A = mesh_v.vertices[i].Position;
				A = glm::translate(mat4identity, entity.position) * glm::vec4(A.x, A.y, A.z, 1.0);
				counter++;
			}
			else if (counter == 1) {
				B = mesh_v.vertices[i].Position;
				B = glm::translate(mat4identity, entity.position) * glm::vec4(A.x, A.y, A.z, 1.0);
				counter++;
			}
			else if (counter == 2) {
				C = mesh_v.vertices[i].Position;
				C = glm::translate(mat4identity, entity.position) * glm::vec4(A.x, A.y, A.z, 1.0);
				int test = ray_triangle_intersection(active_camera.Position, pickray_direction, A, B, C);
				if (test > 0)
					return true;
				counter = 0;
			}
		}
	}
	return false;
}

void check_pickray_collision() {
	auto entity_ptr = active_scene->entities.begin();
	int collided_entity = -1;	// point to first entity just to initialize it
	int collided_light = -1;
	float entity_closer_distance = numeric_limits<float>::max();
	for (entity_ptr; entity_ptr != active_scene->entities.end(); entity_ptr++) {
		float dist = check_box_collision(entity_ptr->index);
		if (entity_closer_distance > dist && dist > 0) {
			collided_entity = entity_ptr->id;
			entity_closer_distance = dist;
		}
	}

	//point light pointer
	auto pl_ptr = active_scene->pointLights.begin();
	auto pl_end = active_scene->pointLights.end();
	for (pl_ptr; pl_ptr != pl_end; pl_ptr++) {
		//@FixMe: passing index to collision call. This is a general problem, do i pass index or Ids?
		// Adapt entity structs and methods to what is more inteligent
		float dist = check_light_collision(pl_ptr - active_scene->pointLights.begin());
		if (entity_closer_distance > dist && dist > 0) {
			collided_light = pl_ptr->id;
			entity_closer_distance = dist;
		}
	}

	if (collided_light != -1) {
		if (collided_light == entity_controls.selected_light)
			entity_controls.is_dragging_entity = true;
		else
			entity_controls.selected_light = collided_light;

		entity_controls.selected_entity = -1;
	}
	else if (collided_entity != -1) {
		if (collided_entity == entity_controls.selected_entity)
			entity_controls.is_dragging_entity = true;
		else
			entity_controls.selected_entity = collided_entity;

		entity_controls.selected_light = -1;
	}
	else {
		entity_controls.selected_entity = -1;
		entity_controls.selected_light = -1;
	}

	pickray_collision_test = false;
}

float ray_triangle_intersection(glm::vec3 ray_origin, glm::vec3 ray_dir, glm::vec3 A, glm::vec3 B, glm::vec3 C, bool check_both_sides) {
	glm::vec3 E1 = B - A;
	glm::vec3 E2 = C - A;
	glm::vec3 N = glm::cross(E1, E2);
	float det = -glm::dot(ray_dir, N);
	float invdet = 1.0 / det;
	glm::vec3 AO = ray_origin - A;
	glm::vec3 DAO = glm::cross(AO, ray_dir);
	float u = glm::dot(E2, DAO) * invdet;
	float v = -glm::dot(E1, DAO) * invdet;
	float t = glm::dot(AO, N) * invdet;
	bool test1 = (det >= 1e-6 && t >= 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0);
	if (test1)
		return t;

	if (check_both_sides) {
		E1 = B - A;
		E2 = C - A;
		N = glm::cross(E2, E1);
		det = -glm::dot(ray_dir, N);
		invdet = 1.0 / det;
		AO = ray_origin - A;
		DAO = glm::cross(ray_dir, AO);
		u = glm::dot(E2, DAO) * invdet;
		v = -glm::dot(E1, DAO) * invdet;
		float t2 = glm::dot(AO, N) * invdet;
		bool test2 = (det >= 1e-6 && t2 >= 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0);
		if (test2)
			return t2;
	}

	return -1;
}


// ..;: Utils :;..

void load_text_textures(string font, int size) {
	// Load font
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	FT_Face face;

	string filepath = fonts_path + font;
	if (FT_New_Face(ft, filepath.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, size);

	//Sets opengl to require just 1 byte per pixel in textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//we will store all characters inside the Characters map
	for (GLubyte c = 0; c < 128; c++)
	{
		//Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
			continue;
		}

		GLuint gylphTexture;
		glGenTextures(1, &gylphTexture);
		glBindTexture(GL_TEXTURE_2D, gylphTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
			face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = { gylphTexture, glm::ivec2(face->glyph->bitmap.width,
			face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), face->glyph->advance.x };
		Characters.insert(std::pair<GLchar, Character>(c, character));
		//std::cout << "c: " << (GLchar)c << " sizeInfo: " << character.Size.x << " (x) " << character.Size.y << " (y)" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}


// ..;: Input callbacks :;..

void processInput(GLFWwindow* window)
{
	float cameraSpeed = deltaTime * camera.Acceleration;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			cameraSpeed = deltaTime * camera.Acceleration * 2;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.Position += cameraSpeed * camera.Front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.Position -= cameraSpeed * camera.Front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.Position -= cameraSpeed * glm::normalize(glm::cross(camera.Front, camera.Up));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.Position += cameraSpeed * glm::normalize(glm::cross(camera.Front, camera.Up));
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.Position -= cameraSpeed * camera.Up;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			camera.Position += cameraSpeed * camera.Up;
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			camera_look_at(camera, glm::vec3(0.0f, 0.0f, 0.0f), true);
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
	if (moveMode || (editor_controls.is_mouse_drag && !entity_controls.is_dragging_entity)) {
		// 'teleports' stored coordinates to current mouse coordinates
		if (resetMouseCoords) {
			lastXMouseCoord = xpos;
			lastYMouseCoord = ypos;
			resetMouseCoords = false;
		}

		// calculates offsets
		float xoffset = xpos - lastXMouseCoord;
		float yoffset = lastYMouseCoord - ypos;
		lastXMouseCoord = xpos;
		lastYMouseCoord = ypos;

		xoffset *= camera.Sensitivity;
		yoffset *= camera.Sensitivity;

		camera_change_direction(camera, xoffset, yoffset);

		// Unallows camera to perform a flip
		if (camera.Pitch > 89.0f)
			camera.Pitch = 89.0f;
		if (camera.Pitch < -89.0f)
			camera.Pitch = -89.0f;

		// Make sure we don't overflow floats when camera is spinning indefinetely
		if (camera.Yaw > 360.0f) 
			camera.Yaw = camera.Yaw - 360.0f;
		if (camera.Yaw < -360.0f)
			camera.Yaw = camera.Yaw + 360.0f;

	}
	else if (entity_controls.is_dragging_entity) {
		// search for entity in scene (shouldn't be done every frame)
		// @attention
		if (entity_controls.selected_entity != -1) {
			int entityId = entity_controls.selected_entity;
			auto it = active_scene->entities.begin();
			auto end = active_scene->entities.end();
			auto entity = find_if(it, end, [entityId](const Entity& entity) {
				if (entity.id == entityId)
					return true;
				else return false;
				});
			if (it != end) {
				cast_pickray();
				float t_y = (entity->position.y - camera.Position.y) / pickray_dir.y;
				entity->position.x = camera.Position.x + t_y * pickray_dir.x;
				entity->position.z = camera.Position.z + t_y * pickray_dir.z;
			}
		}
		else if (entity_controls.selected_light != -1) {
			int entityId = entity_controls.selected_light;
			auto it = active_scene->pointLights.begin();
			auto end = active_scene->pointLights.end();
			auto entity = find_if(it, end, [entityId](const PointLight& entity) {
				if (entity.id == entityId)
					return true;
				else return false;
				});
			if (it != end) {
				cast_pickray();
				float t_y = (entity->position.y - camera.Position.y) / pickray_dir.y;
				entity->position.x = camera.Position.x + t_y * pickray_dir.x;
				entity->position.z = camera.Position.z + t_y * pickray_dir.z;
			}
		}
	}


	currentMouseX = xpos;
	currentMouseY = ypos;

	// mouse dragging controls
	if (editor_controls.is_mouse_left_btn_press
		&& (abs(editor_controls.mouse_btn_down_x - currentMouseX) > 2
			|| abs(editor_controls.mouse_btn_down_y - currentMouseY) > 2)) {
		editor_controls.is_mouse_drag = true;
	}
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
			if (camera.FOVy <= 45.0f && camera.FOVy >= 1.0f)
				camera.FOVy -= yoffset * 3;
			if (camera.FOVy < 1.0f)
				camera.FOVy = 1.0f;
			if (camera.FOVy > 45.0f)
				camera.FOVy = 45.0f;
		}
		else {
			camera.Position += (float)(3 * yoffset) * camera.Front;
		}
	}
}

void onMouseBtn(GLFWwindow* window, int button, int action, int mods) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

			if (editor_mode) {
				editor_controls.is_mouse_left_btn_press = true;
				if (editor_controls.press_release_toggle != GLFW_PRESS) {
					// captures mouse coordinates for mouse dragging checkage 
					editor_controls.mouse_btn_down_x = currentMouseX;
					editor_controls.mouse_btn_down_y = currentMouseY;

					cast_pickray();

					pickray_collision_test = true;

					editor_controls.press_release_toggle = GLFW_PRESS;
				}
				// might want to drag camera when click, got to reset to make it work
				resetMouseCoords = true;
			}
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			cout << "left_btn_release" << endl;
			editor_controls.is_mouse_left_btn_press = false;
			cout << editor_controls.is_mouse_drag << endl;
			if (!editor_controls.is_mouse_drag && !moveMode) {


			}
			editor_controls.press_release_toggle = GLFW_RELEASE;
			editor_controls.is_mouse_drag = false;
			entity_controls.is_dragging_entity = false;
		}
		// toggle move mode
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			if (moveMode) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				moveMode = false;
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				moveMode = true;
				resetMouseCoords = true;
			}
		}
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
