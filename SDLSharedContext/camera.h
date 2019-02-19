#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(glm::vec4);
	void SetPosition(float, float, float);
	void SetRotation(glm::vec4);
	void SetRotation(float, float, float);
	void SetProjection(uint32_t, uint32_t, float, float, float);

	glm::vec4 GetPosition();
	glm::vec4 GetRotation();

	glm::mat4x4 GetViewProjectionMatrix();

private:
	void SetUp();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	glm::mat4x4 m_viewMatrix, m_projectionMatrix;
	float m_windowWidth, m_windowHeight;
	float m_fovy, m_near, m_far;
};