#include "TestDiffuseMap.h"

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"
#include "Texture.h"
#include "VertexArray.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"


const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

namespace test {
	
	glm::vec3 lightPos(1.2f, 1.0f, 0.0f);

	float lightPosX = 1.2f;
	float lightPosY = 1.0f;
	float lightPosZ = 0.0f;

	inline void processInput(GLFWwindow* window);
	inline void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	Camera camera2;

	inline void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * camera2.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera2.cameraPos += cameraSpeed * camera2.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera2.cameraPos -= cameraSpeed * camera2.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera2.cameraPos -= glm::normalize(glm::cross(camera2.cameraFront, camera2.cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera2.cameraPos += glm::normalize(glm::cross(camera2.cameraFront, camera2.cameraUp)) * cameraSpeed;
	}

	inline void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (camera2.firstMouse)
		{
			camera2.lastX = xpos;
			camera2.lastY = ypos;
			camera2.firstMouse = false;
		}

		float xoffset = xpos - camera2.lastX;
		float yoffset = camera2.lastY - ypos; // reversed since y-coordinates go from bottom to top
		camera2.lastX = xpos;
		camera2.lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera2.yaw += xoffset;
		camera2.pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera2.pitch > 89.0f)
			camera2.pitch = 89.0f;
		if (camera2.pitch < -89.0f)
			camera2.pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera2.yaw)) * cos(glm::radians(camera2.pitch));
		front.y = sin(glm::radians(camera2.pitch));
		front.z = sin(glm::radians(camera2.yaw)) * cos(glm::radians(camera2.pitch));
		camera2.cameraFront = glm::normalize(front);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera2.fov -= (float)yoffset;
		if (camera2.fov < 1.0f)
			camera2.fov = 1.0f;
		if (camera2.fov > 45.0f)
			camera2.fov = 45.0f;
	}


	TestDiffuseMap::TestDiffuseMap()
		:m_Proj(glm::mat4(1.0f)),
		m_View(glm::mat4(1.0f)),
		model(glm::mat4(1.0f))
	{
		float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
		};
		
		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(vertices, 8 * 36 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);

		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_lightVAO = std::make_unique<VertexArray>();

		m_lightVAO->AddBuffer(*m_VertexBuffer, layout);


		glfwSetCursorPosCallback(glfwGetCurrentContext(), mouse_callback);
		glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);

		// tell GLFW to capture our mouse
		//glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		GLCall(glEnable(GL_DEPTH_TEST));

		m_lightingShader = std::make_unique<Shader>("res/shaders/DiffuseMap.shader");
		m_DiffuseMap = std::make_unique<Texture>("res/textures/container2.png");
		m_SpecularMap = std::make_unique<Texture>("res/textures/container2_specular.png");
		m_lightingCubeShader = std::make_unique<Shader>("res/shaders/lightingCube.shader");


	}

	TestDiffuseMap::~TestDiffuseMap()
	{
	}

	void TestDiffuseMap::OnUpdate(float deltaTime)
	{
	}

	void TestDiffuseMap::OnRender()
	{
		GLFWwindow* currentWindow = glfwGetCurrentContext();

		float currentFrame = static_cast<float>(glfwGetTime());
		camera2.deltaTime = currentFrame - camera2.lastFrame;
		camera2.lastFrame = currentFrame;

		processInput(currentWindow);

		GLCall(glClearColor(0.1f,0.1f,0.1f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		lightPos= glm::vec3(lightPosX,lightPosY,lightPosZ);

		m_DiffuseMap->Bind(0);
		m_SpecularMap->Bind(1);

		m_lightingShader->Bind();
		m_lightingShader->SetUniform1i("material.diffuse", 0);
		m_lightingShader->SetUniform1i("material.specular", 1);
		m_lightingShader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
		m_lightingShader->SetUniform3f("light.position", lightPos);
		m_lightingShader->SetUniform3f("viewPos", camera2.cameraPos);

		m_lightingShader->SetUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
		m_lightingShader->SetUniform1f("material.shininess", 64.0f);
		m_lightingShader->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
		m_lightingShader->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f);
		m_lightingShader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

		Renderer renderer;
		
		model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera2.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		m_View = glm::lookAt(camera2.cameraPos, camera2.cameraPos + camera2.cameraFront, camera2.cameraUp);
		m_lightingShader->SetUniformMat4("projection", projection);
		m_lightingShader->SetUniformMat4("view", m_View);
		m_lightingShader->SetUniformMat4("model", model);

		m_VAO->Bind();
		renderer.Draw(*m_VAO, *m_lightingShader, 36);
		
		m_lightingCubeShader->Bind();
		m_lightingCubeShader->SetUniformMat4("projection", projection);
		m_lightingCubeShader->SetUniformMat4("view", m_View);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.5f));
		m_lightingCubeShader->SetUniformMat4("model", model);
		m_lightVAO->Bind();
		renderer.Draw(*m_lightVAO,  *m_lightingCubeShader,36);
	}

	void TestDiffuseMap::OnImGuiRender()
	{
  		ImGui::SliderFloat(" lightPos X", &lightPosX, -3.0f,3.0f);
  		ImGui::SliderFloat(" lightPos Y", &lightPosY, -3.0f,3.0f);
  		ImGui::SliderFloat(" lightPos Z", &lightPosZ, -3.0f,3.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	
	

}

