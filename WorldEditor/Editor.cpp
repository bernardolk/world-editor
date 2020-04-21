
#include <Editor.h>

#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/vector_float2.hpp> // vec2
#include <glm/ext/vector_float3.hpp> // vec3
#include <glm/ext/matrix_float4x4.hpp> // mat4x4
#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale, identity
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <file_browser_modal.h>

#include <Shader.h>
#include <Camera.h>
#include <Entities.h>
#include <Game.h>

using namespace std;

float viewport_height;
float viewport_width;

GLuint text_VAO, text_VBO;

bool resetMouseCoords = true;
bool keyComboPressed = false;
bool show_GUI = false;
bool GUI_btn_down = false;
bool pickray_collision_test;
glm::vec3 bg_color(0.008f, 0.314f, 0.275f);
Shader grid_shader;
Shader bounding_box_shader;
Shader text_shader;
unsigned int editor_textures[15];
struct EditorControls {
	bool camera_align_x = false;
	bool camera_align_y = false;
	bool camera_align_z = false;
	bool is_mouse_left_btn_press = false;
	long mouse_btn_down_time;
	bool is_mouse_drag = false;
	double mouse_btn_down_x;
	double mouse_btn_down_y;
	unsigned int press_release_toggle = 0;
} editor_controls;
unsigned int pt_vao;
ImGuiStyle* imStyle;
static imgui_ext::file_browser_modal filebrowser_model("Model", "C:\\World Editor Assets\\Models", "obj");
static imgui_ext::file_browser_modal filebrowser_scene("Scene", "C:\\World Editor Assets\\Scenes");


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

void editor_initialize(float viewportWidth, float viewportHeight) {
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

	viewport_width = viewportWidth;
	viewport_height = viewportHeight;

	//load shaders
	grid_shader = Shader("shaders/editor_grid_vertex.shd", "shaders/editor_grid_fragment.shd");
	bounding_box_shader = Shader("shaders/bounding_box_vertex.shd", "shaders/bounding_box_fragment.shd");
	text_shader = Shader("shaders/vertex_text.shd", "shaders/fragment_text.shd");

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



void editor_render_gui(Camera& camera) {
	if (show_GUI) {

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

		//unsigned int bb_ebo;
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
	/*unsigned int gridVAO, gridVBO;
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

void render_point() {
	bounding_box_shader.use();
	bounding_box_shader.setMatrix4("view", active_camera.View4x4);
	bounding_box_shader.setMatrix4("projection", active_camera.Projection4x4);
	glm::mat4 model_pt = glm::translate(mat4identity, pickray_intersection_p);
	bounding_box_shader.setMatrix4("model", model_pt);

	glBindVertexArray(pt_vao);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, 1);
}



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