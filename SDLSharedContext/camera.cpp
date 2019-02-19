#include "camera.h"

Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(glm::vec4 newPos)
{
	m_positionX = newPos.x;
	m_positionY = newPos.y;
	m_positionZ = newPos.z;
	return;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void Camera::SetRotation(glm::vec4 newRotation)
{
	m_rotationX = newRotation.x;
	m_rotationY = newRotation.y;
	m_rotationZ = newRotation.z;
	return;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

void Camera::SetProjection(uint32_t windowWidth, uint32_t windowHeight, float fovy, float near, float far)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_fovy = fovy;
	m_near = near;
	m_far = far;
}

glm::vec4 Camera::GetPosition()
{
	return glm::vec4(m_positionX, m_positionY, m_positionZ, 1.0f);
}

glm::vec4 Camera::GetRotation()
{
	return glm::vec4(m_rotationX, m_rotationY, m_rotationZ, 1.0f);
}

void Camera::SetUp()
{
	glm::vec4 up, position, lookAt;
	float yaw, pitch, roll;
	glm::mat4x4 rotationMatrix;


	// Setup the vector that points upwards.
	up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup the position of the camera in the world.
	position = glm::vec4(m_positionX, m_positionY, m_positionZ, 1.0f);

	// Setup where the camera is looking by default.
	lookAt = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = glm::rotate(pitch, glm::vec3(1.0, 0.0, 0.0));
	rotationMatrix *= glm::rotate(yaw, glm::vec3(0.0, 1.0, 0.0));
	rotationMatrix *= glm::rotate(roll, glm::vec3(0.0, 0.0, 1.0));

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = rotationMatrix * lookAt;
	up = rotationMatrix * up;

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = glm::lookAt(glm::vec3(position), glm::vec3(lookAt), glm::vec3(up));

	float a = 4 / 3;

	m_projectionMatrix = glm::perspective(glm::radians(m_fovy), m_windowWidth / m_windowHeight, m_near, m_far);

	return;
}

glm::mat4x4 Camera::GetViewProjectionMatrix()
{
	SetUp();
	return m_projectionMatrix * m_viewMatrix;
}