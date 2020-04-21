#include<Camera.h>

#include <glm/glm.hpp>
#include <glm/ext/vector_float2.hpp> // vec2
#include <glm/ext/vector_float3.hpp> // vec3
#include <glm/ext/matrix_float4x4.hpp> // mat4x4
#include <glm/ext/matrix_transform.hpp> // translate, rotate, scale, identity
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>

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


