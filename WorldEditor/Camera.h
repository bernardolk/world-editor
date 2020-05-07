#pragma once

#include<glm/glm.hpp>


struct Camera {
	glm::vec3 Position = glm::vec3(0.0f, 3.0f, 0.0f);
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	float Acceleration = 10.0f;
	float FOVy = 45.0f;
	float FarPlane = 300.0f;
	float NearPlane = 0.1f;
	float Sensitivity = 0.1f;
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	glm::mat4 View4x4;
	glm::mat4 Projection4x4;
};

void camera_update(Camera& camera, float viewportWidth, float viewportHeight);
void camera_change_direction(Camera& camera, float yawOffset, float pitchOffset);
// Make camera look at a place in world coordinates to look at. If isPosition is set to true, then
// a position is expected, if else, then a direction is expected.
void camera_look_at(Camera& camera, glm::vec3 position, bool isPosition);