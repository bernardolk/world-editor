#pragma once
#include<Camera.h>

#include <glm/glm.hpp>
#include <glm/ext/vector_float2.hpp> // vec2
#include <glm/ext/vector_float3.hpp> // vec3
#include <glm/gtx/compatibility.hpp>


void camera_update(Camera& camera, float viewportWidth, float viewportHeight) {
	camera.View4x4 = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
	camera.Projection4x4 = glm::perspective(glm::radians(camera.FOVy), viewportWidth / viewportHeight, camera.NearPlane, camera.FarPlane);
}


void camera_change_direction(Camera& camera, float yawOffset, float pitchOffset) {
	float newPitch = camera.Pitch += pitchOffset;
	float newYaw = camera.Yaw += yawOffset;
	camera.Front.x = cos(glm::radians(newPitch)) * cos(glm::radians(newYaw));
	camera.Front.y = sin(glm::radians(newPitch));
	camera.Front.z = cos(glm::radians(newPitch)) * sin(glm::radians(newYaw));
	camera.Front = glm::normalize(camera.Front);
}


void camera_look_at(Camera& camera, glm::vec3 position, bool isPosition) {
	glm::vec3 look_vec;
	if (isPosition)
		look_vec = glm::normalize(position - glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
	else
		look_vec = glm::normalize(position);

	float pitchRdns = glm::asin(look_vec.y);
	camera.Pitch = glm::degrees(pitchRdns);
	camera.Yaw = glm::degrees(atan2(look_vec.x, -1 * look_vec.z) - 3.141592 / 2);

	camera.Front.x = cos(glm::radians(camera.Pitch)) * cos(glm::radians(camera.Yaw));
	camera.Front.y = sin(glm::radians(camera.Pitch));
	camera.Front.z = cos(glm::radians(camera.Pitch)) * sin(glm::radians(camera.Yaw));
	camera.Front = glm::normalize(camera.Front);
}

int camera_create(glm::vec3 initialPosition, glm::vec3 lookVector) {
	Camera new_camera;
	new_camera.Position = initialPosition;
	camera_look_at(new_camera, lookVector, true);
	cameraList.push_back(new_camera);
	return cameraList.size();
}
