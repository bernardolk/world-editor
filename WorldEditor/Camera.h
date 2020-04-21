#pragma once

struct Camera {
	glm::vec3 Position;					//camera Initial world-space coordinates
	glm::vec3 Front;					//unit vector for where the camera is facing
	glm::vec3 Up;						//y-axis @careful this is fragile as this assumes we will never roll the camera (player is always straight up)
	float Acceleration;					//distance over seconds squared to get constant velocity over variable time per frame
	float FOVy;
	float FarPlane;
	float NearPlane;
	float Sensitivity;
	float Yaw;
	float Pitch;
	glm::mat4 View4x4;
	glm::mat4 Projection4x4;
};

void camera_update(Camera& camera, float viewportWidth, float viewportHeight);
void camera_change_direction(Camera& camera, float yawOffset, float pitchOffset);
// Make camera look at a place in world coordinates to look at. If isPosition is set to true, then
// a position is expected, if else, then a direction is expected.
void camera_look_at(Camera& camera, glm::vec3 position, bool isPosition);