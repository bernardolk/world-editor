#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <file_browser_modal.h>

bool moveMode = false;
float light_icons_scaling = 0.8f;


void editor_start_frame();
void editor_end_frame();
void editor_initialize(float viewportWidth, float viewportHeight);
void editor_update();
void editor_terminate();
void editor_render_gui(Camera& camera);
void editor_process_input_mouse_move(double xpos, double ypos);
void editor_process_input_mouse_btn(int button, int action);
void show_entity_controls(int entityId);
void editor_loop();
float ray_triangle_intersection(glm::vec3 ray_origin, glm::vec3 ray_dir, glm::vec3 A, glm::vec3 B, glm::vec3 C, bool check_both_sides = false);
void editor_create_grid(int gridx, int gridy, float gridl);
void show_light_controls(int lightId);
void check_pickray_collision(glm::vec3 pickray);
void show_entity_controls(int entityId);
glm::vec3 cast_pickray(Camera activeCamera);
float check_box_collision(int entityIndex, glm::vec3 pickray);
bool check_collision(Entity entity, glm::vec3 pickray);
void load_text_textures(std::string font, int size);
void editor_update();
void render_bounding_box(int entityId);



bool GUI_btn_down = false;
bool resetMouseCoords = true;
bool keyComboPressed = false;
bool show_GUI = false;


u32 buf, vao, vbo, entity_counter = 0;
u32 quad_vao, quad_vbo, quad_ebo, quad_lightbulb_texture;


using namespace std;

float viewport_height;
float viewport_width;

GLuint text_VAO, text_VBO;

// Temporary, for the editor bounding boxes
u32 bounding_box_indices[] = {
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
	Vertex{glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f),glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f),glm::vec2(1.0f, 0.0f)},
	Vertex{glm::vec3(1.0f, 1.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f),glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(0.0f, 1.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f),glm::vec2(0.0f, 1.0f)}
};


u32 quad_indices[] = {
	0,1,2,
	2,3,0
};

vector<u32> quad_vertex_indices = { 0,1,2,2,3,0 };

glm::vec3 bg_color(0.008f, 0.314f, 0.275f);
Shader grid_shader;
Shader bounding_box_shader;
Shader text_shader;
u32 editor_textures[15];
struct EditorControls {
	bool camera_align_x = false;
	bool camera_align_y = false;
	bool camera_align_z = false;
	bool is_mouse_left_btn_press = false;
	long mouse_btn_down_time;
	bool is_mouse_drag = false;
	double mouse_btn_down_x;
	double mouse_btn_down_y;
	u32 press_release_toggle = 0;
} editor_controls;
u32 pt_vao;
ImGuiStyle* imStyle;
static imgui_ext::file_browser_modal filebrowser_model("Model", "C:\\World Editor Assets\\Models", "obj");
static imgui_ext::file_browser_modal filebrowser_scene("Scene", "C:\\World Editor Assets\\Scenes");

double lastXMouseCoord;
double lastYMouseCoord;

bool render_pickray = false;

// Function declarations
string format_float_tostr(float num, int precision);
void render_text(std::string text, float x, float y, float scale, glm::vec3 color);
float check_light_collision(int lightIndex, glm::vec3 pickray);

//entity control window
struct EntityControls {
	int selected_entity = -1;
	int selected_light = -1;
	int last_selected_entity = -1;
	glm::vec3 original_position;
	bool is_dragging_entity = false;
	glm::vec3 rotation_offset = glm::vec3(0.0f, 0.0f, 0.0f);
}entity_controls;


struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

map<GLchar, Character> Characters; // GUI character set

		// --- Pickray ---
//glm::vec3 pickray_collision_point;
//glm::vec3 pickray_direction;
//bool pickray_collision_test;



void editor_start_frame() {
	ImGui_ImplOpenGL3_NewFrame(); // feed inputs to dear imgui, start new frame
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void editor_end_frame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
}

void editor_terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//  ..,;: Editor main loop :;,..

void editor_loop() {

	if (entity_controls.selected_entity > -1 && !moveMode) {
		render_bounding_box(entity_controls.selected_entity);
		show_entity_controls(entity_controls.selected_entity);
	}
	if (entity_controls.selected_light > -1 && !moveMode) {
		show_light_controls(entity_controls.selected_light);
	}

	/*if (pickray_collision_test)
		check_pickray_collision();*/

	editor_render_gui(active_camera);

}

void editor_initialize(float viewportWidth, float viewportHeight) {

	// Setup mouse and camera initial settings
	lastXMouseCoord = viewportWidth / 2;
	lastYMouseCoord = viewportHeight / 2;
	//@Attention!: there should be a system that updates mouse coordinates at every frame
	// but it should be independent of anything else, it just makes available the values to other
	// editor methods
	resetMouseCoords = true;

	viewport_width = viewportWidth;
	viewport_height = viewportHeight;

	const char* glsl_version = "#version 330";
	// Setup Dear ImGui context
	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// ImGUI Style settings
	imStyle = &ImGui::GetStyle();
	imStyle->WindowRounding = 1.0f;

	// Blend enabling
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//load shaders
	grid_shader = create_shader_program("Grid Shader", "shaders/editor_grid_vertex.shd", "shaders/editor_grid_fragment.shd");
	bounding_box_shader = create_shader_program("Bounding Box Shader", "shaders/bounding_box_vertex.shd", "shaders/bounding_box_fragment.shd");
	text_shader = create_shader_program("Text Shader", "shaders/vertex_text.shd", "shaders/fragment_text.shd");

	//generate text buffers
	glGenVertexArrays(1, &text_VAO);
	glGenBuffers(1, &text_VBO);
	glBindVertexArray(text_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//set projection matrix for GUI text
	text_shader.use();
	text_shader.setMatrix4("projection", glm::ortho(0.0f, viewportWidth, 0.0f, viewportHeight));

	//setup textures
	//btn_x
	editor_textures[0] = load_texture_from_file("btn_x.png", "Assets/textures");
	//btn_y
	editor_textures[1] = load_texture_from_file("btn_y.png", "Assets/textures");
	//btn_z
	editor_textures[2] = load_texture_from_file("btn_z.png", "Assets/textures");


	//pt_vao = gen_vertex_buffer(point_buffer);
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

}

void editor_update() {
	//@todo: actually, dear imgui has some btn toggle functionalities that i can use here 
	if (editor_controls.camera_align_x)
		camera_look_at(active_camera, glm::vec3(1, 0, 0), false);
	if (editor_controls.camera_align_y)
		camera_look_at(active_camera, glm::vec3(0, 1, 0), false);
	if (editor_controls.camera_align_z)
		camera_look_at(active_camera, glm::vec3(0, 0, 1), false);
}

void editor_process_input_mouse_move(double xpos, double ypos) {

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

		xoffset *= active_camera.Sensitivity;
		yoffset *= active_camera.Sensitivity;

		camera_change_direction(active_camera, xoffset, yoffset);

		// Unallows camera to perform a flip
		if (active_camera.Pitch > 89.0f)
			active_camera.Pitch = 89.0f;
		if (active_camera.Pitch < -89.0f)
			active_camera.Pitch = -89.0f;

		// Make sure we don't overflow floats when camera is spinning indefinetely
		if (active_camera.Yaw > 360.0f)
			active_camera.Yaw = active_camera.Yaw - 360.0f;
		if (active_camera.Yaw < -360.0f)
			active_camera.Yaw = active_camera.Yaw + 360.0f;

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
				glm::vec3 pickray = cast_pickray(active_camera);
				float t_y = (entity->position.y - active_camera.Position.y) / pickray.y;
				entity->position.x = active_camera.Position.x + t_y * pickray.x;
				entity->position.z = active_camera.Position.z + t_y * pickray.z;
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
				glm::vec3 pickray = cast_pickray(active_camera);
				float t_y = (entity->position.y - active_camera.Position.y) / pickray.y;
				entity->position.x = active_camera.Position.x + t_y * pickray.x;
				entity->position.z = active_camera.Position.z + t_y * pickray.z;
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

void editor_process_input_mouse_btn(int button, int action) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

			if (editor_mode) {
				editor_controls.is_mouse_left_btn_press = true;
				if (editor_controls.press_release_toggle != GLFW_PRESS) {
					// captures mouse coordinates for mouse dragging checkage 
					editor_controls.mouse_btn_down_x = currentMouseX;
					editor_controls.mouse_btn_down_y = currentMouseY;

					check_pickray_collision(cast_pickray(active_camera));

					//pickray_collision_test = true;

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

void editor_render_gui(Camera& camera) {
		// render GUI text
		{
			// text render
			float GUI_x = 25;
			float GUI_y = viewport_height - 60;

			string GUI_atts[]{
				format_float_tostr(camera.Position.x, 2),
				format_float_tostr(camera.Position.y,2),
				format_float_tostr(camera.Position.z,2),
				format_float_tostr(camera.Pitch,2),
				format_float_tostr(camera.Yaw,2),
			};

			string camera_stats = "x: " + GUI_atts[0] + " y:" + GUI_atts[1] + " z:" + GUI_atts[2];
			string mouse_stats = "pitch: " + GUI_atts[3] + " yaw: " + GUI_atts[4];
			string fps = to_string(current_fps);
			string fps_gui = "FPS: " + fps.substr(0, fps.find('.', 0) + 2);


			float scale = 1;
			render_text(camera_stats, GUI_x, GUI_y, scale, glm::vec3(1.0f, 1.0f, 1.0f));
			render_text(mouse_stats, GUI_x, GUI_y - 25, scale, glm::vec3(1.0f, 1.0f, 1.0f));
			render_text(fps_gui, viewport_height - 100, 25, scale, glm::vec3(1.0f, 1.0f, 1.0f));
		} // render GUI text


		// render GUI controls
		{
			ImGui::Begin("Bg Color");
			ImGui::ColorPicker3("BG", &bg_color.x);
			ImGui::End();

			ImGui::Begin("##Cam");
			editor_controls.camera_align_x = ImGui::ImageButton((void*)(intptr_t)editor_textures[0], ImVec2(16, 16));
			editor_controls.camera_align_y = ImGui::ImageButton((void*)(intptr_t)editor_textures[1], ImVec2(16, 16));
			editor_controls.camera_align_z = ImGui::ImageButton((void*)(intptr_t)editor_textures[2], ImVec2(16, 16));
			ImGui::End();

			bool isModelLoadClicked = false;
			bool isSceneLoadClicked = false;
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("Load")) {
					if (ImGui::MenuItem("Model")) {
						isModelLoadClicked = true;
					}
					if (ImGui::MenuItem("Scene")) {
						isSceneLoadClicked = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			std::string path;
			if (filebrowser_model.render(isModelLoadClicked, path)) {
				/*	Model new_model = Model(path);
					Entity new_entity = Entity{ entity_counter,++entity_counter, &new_model, &model_shader,vec3(0.0f, 0.0f,0.0f) };
					cast_pickray();
					new_entity.position = vec3(pickray_dir * 10.0f);
					active_scene->entities.push_back(new_entity);*/
			}
			if (filebrowser_scene.render(isSceneLoadClicked, path)) {

			}
		} // render GUI controls
}

void show_entity_controls(int entityId) {
	// search for entity in scene (shouldn't be done every frame)
	// @fixMe
	auto it = active_scene->entities.begin();
	auto end = active_scene->entities.end();
	auto entity = find_if(it, end, [entityId](const Entity& entity) {
		if (entity.id == entityId)
			return true;
		else return false;
		});
	if (it != end) {
		/*if (entityId != entity_controls.last_selected_entity) {
			entity_controls.original_position = entity->position;
			entity_controls.last_selected_entity = entityId;
		}*/

		//@iwonder: does dear imgui retains state through window labels? why reseting the sliders min max doesn't work?
		stringstream label;
		label << "entity "; label << to_string(entityId);
		string lbl_str = label.str();
		ImGui::Begin(lbl_str.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SliderFloat("x", &entity->position.x, entity_controls.original_position.x - 50, entity_controls.original_position.x + 50);
		ImGui::SliderFloat("y", &entity->position.y, entity_controls.original_position.x - 50, entity_controls.original_position.x + 50);
		ImGui::SliderFloat("z", &entity->position.z, entity_controls.original_position.x - 50, entity_controls.original_position.x + 50);

		ImGui::SliderFloat("rot x", &entity->rotation.x, -360, 360);
		ImGui::SliderFloat("rot y", &entity->rotation.y, -360, 360);
		ImGui::SliderFloat("rot z", &entity->rotation.z, -360, 360);
		ImGui::End();
	}

}

void render_text(std::string text, float x, float y, float scale, glm::vec3 color) {
	text_shader.use();
	text_shader.setFloat3("textColor", color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(text_VAO);

	std::string::iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
		{ xpos, ypos + h, 0.0, 0.0 },
		{ xpos, ypos, 0.0, 1.0 },
		{ xpos + w, ypos, 1.0, 1.0 },
		{ xpos, ypos + h, 0.0, 0.0 },
		{ xpos + w, ypos, 1.0, 1.0 },
		{ xpos + w, ypos + h, 1.0, 0.0 }
		};

		//std::cout << "xpos: " << xpos << ", ypos:" << ypos << ", h: " << h << ", w: " << w << std::endl;
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

string format_float_tostr(float num, int precision) {
	string temp = std::to_string(num);
	return temp.substr(0, temp.find(".") + 3);
}

void render_bounding_box(int entityId) {
	//@attention: this sould be a hashtable outside everything!
	auto it = active_scene->entities.begin();
	auto end = active_scene->entities.end();
	auto entity = find_if(it, end, [entityId](const Entity& entity) {
		if (entity.id == entityId)
			return true;
		else return false;
		});
	if (it != end) {
		//Entity entity = active_scene->entities[entityIndex];
		bounding_box_shader.use();
		bounding_box_shader.setMatrix4("projection", active_camera.Projection4x4);
		bounding_box_shader.setMatrix4("view", active_camera.View4x4);
		bounding_box_shader.setMatrix4("model", entity->matModel);

		glBindVertexArray(entity->model3d->bb_vao);

		//u32 bb_ebo;
		glLineWidth(2.0f);
		//glGenBuffers(1, &bb_ebo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bb_ebo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, 24);
		//glVertexAttribPointer()
		//glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);
	}
}

void editor_create_grid(int gridx, int gridy, float scale) {

	float quad[12] = { 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0 };

	// @insane! creating a new buffer each frame... wtf
	/*u32 gridVAO, gridVBO;
	glGenVertexArrays(1, &gridVAO);
	glGenBuffers(1, &gridVBO);
	glBindVertexArray(gridVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);*/

	grid_shader.use();
	grid_shader.setMatrix4("view", active_camera.View4x4);
	grid_shader.setMatrix4("projection", active_camera.Projection4x4);


	float midx = (gridx * scale) / 2.0f;
	for (float i = -midx; i < midx; i += scale) {
		float midy = (gridy * scale) / 2.0f;
		for (float j = -midy; j < midy; j += scale) {
			glm::mat4 model = mat4identity;
			model = glm::translate(model, glm::vec3(scale * i, 0, scale * j));
			model = glm::scale(model, glm::vec3(scale));
			grid_shader.setMatrix4("model", model);

			glLineWidth(1.0f);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		}
	}

}

//void render_point() {
//	bounding_box_shader.use();
//	bounding_box_shader.setMatrix4("view", active_camera.View4x4);
//	bounding_box_shader.setMatrix4("projection", active_camera.Projection4x4);
//	glm::mat4 model_pt = glm::translate(mat4identity, pickray_intersection_p);
//	bounding_box_shader.setMatrix4("model", model_pt);
//
//	glBindVertexArray(pt_vao);
//	glPointSize(10.0f);
//	glDrawArrays(GL_POINTS, 0, 1);
//}

void show_light_controls(int lightId) {

	// @fixMe
	auto it = active_scene->pointLights.begin();
	auto end = active_scene->pointLights.end();
	auto light_entity = find_if(it, end, [lightId](const PointLight& light) {
		if (light.id == lightId)
			return true;
		else return false;
		});
	if (it != end) {
		stringstream label;
		label << "point light "; label << to_string(lightId);
		string lbl_str = label.str();
		ImGui::Begin(lbl_str.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SliderFloat("x", &light_entity->position.x, entity_controls.original_position.x - 50, entity_controls.original_position.x + 50);
		ImGui::SliderFloat("y", &light_entity->position.y, entity_controls.original_position.y - 50, entity_controls.original_position.y + 50);
		ImGui::SliderFloat("z", &light_entity->position.z, entity_controls.original_position.z - 50, entity_controls.original_position.z + 50);

		ImGui::ColorPicker3("Diffuse", &light_entity->diffuse.x);
		ImGui::ColorPicker3("Specular", &light_entity->specular.x);

		ImGui::DragFloat("constant", &light_entity->intensity_constant, 0.005, -1.0f, 1.0f);
		ImGui::DragFloat("linear", &light_entity->intensity_linear, 0.005, -1.0f, 2.0f);
		ImGui::DragFloat("quadratic", &light_entity->intensity_quadratic, 0.0001, -1.0f, 1.0f);

		ImGui::End();
	}

}

// ..;: Collision :;..

// Returns the direction in world coordinates of a ray cast from a click on screen
glm::vec3 cast_pickray(Camera activeCamera) {

	float screenX_normalized = (currentMouseX - viewport_width / 2) / (viewport_width / 2);
	float screenY_normalized = -1 * (currentMouseY - viewport_height / 2) / (viewport_height / 2);

	glm::vec4 ray_clip(screenX_normalized, screenY_normalized, -1.0, 1.0);
	glm::mat4 inv_view = glm::inverse(activeCamera.View4x4);
	glm::mat4 inv_proj = glm::inverse(activeCamera.Projection4x4);
	glm::vec3 ray_eye_3 = (inv_proj * ray_clip);
	glm::vec4 ray_eye(ray_eye_3.x, ray_eye_3.y, -1.0, 0.0);
	return glm::normalize(inv_view * ray_eye);
}

float check_box_collision(int entityIndex, glm::vec3 pickray) {
	Entity entity = active_scene->entities[entityIndex];
	vector<float> bb = entity.model3d->boundingBox;
	int stride = 3;
	float shortest_distance = numeric_limits<float>::max();

	//@FixMe: I dont know how to optimize this yet, but right now i am checking every face of an object regardless.
	// Isn't there a better way? Maybe a simple filter for some faces, or something. The problem is that sometimes
	// a ray might intercept two faces of the object (or more) and we got to only consider the closest face.

	for (int i = 11; i >= 0; i--) {
		int bbi = i * stride;

		glm::vec3 A = entity.matModel * glm::vec4(bb[bounding_box_indices[bbi + 0] * stride],
			bb[bounding_box_indices[bbi + 0] * stride + 1],
			bb[bounding_box_indices[bbi + 0] * stride + 2], 1.0f);
		glm::vec3 B = entity.matModel * glm::vec4(bb[bounding_box_indices[bbi + 1] * stride],
			bb[bounding_box_indices[bbi + 1] * stride + 1],
			bb[bounding_box_indices[bbi + 1] * stride + 2], 1.0f);
		glm::vec3 C = entity.matModel * glm::vec4(bb[bounding_box_indices[bbi + 2] * stride],
			bb[bounding_box_indices[bbi + 2] * stride + 1],
			bb[bounding_box_indices[bbi + 2] * stride + 2], 1.0f);

		float dist = ray_triangle_intersection(active_camera.Position, pickray, A, B, C, true);

		if (dist > 0 && dist < shortest_distance) {
			shortest_distance = dist;
		}
	}

	if (shortest_distance != numeric_limits<float>::max()) {
		glm::vec3 pickray_collision_point = active_camera.Position + shortest_distance * pickray;
		return shortest_distance;
	}
	else {
		return -1;
	}
}

float check_light_collision(int lightIndex, glm::vec3 pickray) {
	PointLight point_light = active_scene->pointLights[lightIndex];

	//@FixMe: This should be inside the light struct or something similar and not be calculated in the render call nor here!
	glm::vec3 light_norm = active_camera.Position - point_light.position;
	float angle_pitch = atan(light_norm.x / light_norm.z);
	glm::mat4 model_m = glm::translate(mat4identity, point_light.position);
	model_m = rotate(model_m, angle_pitch, active_camera.Up);
	model_m = glm::scale(model_m, glm::vec3(light_icons_scaling, light_icons_scaling * 1.5f, light_icons_scaling));


	//@FixMe: This sucks so much i dont even know where to begin
	// This down here checks for a quad collision

	glm::vec3 A1 = model_m * glm::vec4(quad[0], quad[1], quad[2], 1.0f);
	glm::vec3 B1 = model_m * glm::vec4(quad[5], quad[6], quad[7], 1.0f);
	glm::vec3 C1 = model_m * glm::vec4(quad[10], quad[11], quad[12], 1.0f);
	float dist = ray_triangle_intersection(active_camera.Position, pickray, A1, B1, C1, true);

	if (dist > 0) {
		glm::vec3 pickray_collision_point = active_camera.Position + dist * pickray;
		return dist;
	}

	glm::vec3 A2 = model_m * glm::vec4(quad[10], quad[11], quad[12], 1.0f);
	glm::vec3 B2 = model_m * glm::vec4(quad[15], quad[16], quad[17], 1.0f);
	glm::vec3 C2 = model_m * glm::vec4(quad[0], quad[1], quad[2], 1.0f);
	dist = ray_triangle_intersection(active_camera.Position, pickray, A2, B2, C2, true);

	if (dist > 0) {
		glm::vec3 pickray_collision_point = active_camera.Position + dist * pickray;
		return dist;
	}

	return -1;
}

bool check_collision(Entity entity, glm::vec3 pickray) {

	//@optimization: can calculate on mesh load it's center and test ray-center distance so to try avoiding
	// testing collision again far away meshes inside model
	vector<Mesh>::iterator mesh = entity.model3d->meshes.begin();
	for (mesh; mesh != entity.model3d->meshes.end(); mesh++) {

		Mesh mesh_v = *mesh;

		glm::vec3 A;
		glm::vec3 B;
		glm::vec3 C;
		int counter = 0;
		vector<u32>::iterator index = mesh_v.indices.begin();

		//@Attention: this wont work if the model is rotated in world space

		for (index; index != mesh_v.indices.end(); index++) {
			u32 i = *index;
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
				int test = ray_triangle_intersection(active_camera.Position, pickray, A, B, C);
				if (test > 0)
					return true;
				counter = 0;
			}
		}
	}
	return false;
}

void check_pickray_collision(glm::vec3 pickray) {
	auto entity_ptr = active_scene->entities.begin();
	int collided_entity = -1;	// point to first entity just to initialize it
	int collided_light = -1;
	float entity_closer_distance = numeric_limits<float>::max();
	for (entity_ptr; entity_ptr != active_scene->entities.end(); entity_ptr++) {
		float dist = check_box_collision(entity_ptr->index, pickray);
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
		float dist = check_light_collision(pl_ptr - active_scene->pointLights.begin(), pickray);
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

	//pickray_collision_test = false;
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


