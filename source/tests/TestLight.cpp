#include "TestLight.h"

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"
#include "Texture.h"
#include "VertexArray.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"

glm::vec3 lightPos(1.2f, 1.0f, 0.0f);

float lightPosX = 1.2f;
float lightPosY = 1.0f;
float lightPosZ = 0.0f;


const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

namespace test {
	
	// TODO:
	// 这个地方如果用inline,那么后续添加的案例会覆盖掉
	static void processInput(GLFWwindow* window);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	Camera camera1;

	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * camera1.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera1.cameraPos += cameraSpeed * camera1.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera1.cameraPos -= cameraSpeed * camera1.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera1.cameraPos -= glm::normalize(glm::cross(camera1.cameraFront, camera1.cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera1.cameraPos += glm::normalize(glm::cross(camera1.cameraFront, camera1.cameraUp)) * cameraSpeed;
	}

	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (camera1.firstMouse)
		{
			camera1.lastX = xpos;
			camera1.lastY = ypos;
			camera1.firstMouse = false;
		}

		float xoffset = xpos - camera1.lastX;
		float yoffset = camera1.lastY - ypos; // reversed since y-coordinates go from bottom to top
		camera1.lastX = xpos;
		camera1.lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera1.yaw += xoffset;
		camera1.pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera1.pitch > 89.0f)
			camera1.pitch = 89.0f;
		if (camera1.pitch < -89.0f)
			camera1.pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera1.yaw)) * cos(glm::radians(camera1.pitch));
		front.y = sin(glm::radians(camera1.pitch));
		front.z = sin(glm::radians(camera1.yaw)) * cos(glm::radians(camera1.pitch));
		camera1.cameraFront = glm::normalize(front);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera1.fov -= (float)yoffset;
		if (camera1.fov < 1.0f)
			camera1.fov = 1.0f;
		if (camera1.fov > 45.0f)
			camera1.fov = 45.0f;
	}


	TestLight::TestLight()
		:m_Proj(glm::mat4(1.0f)),
		m_View(glm::mat4(1.0f)),
		model(glm::mat4(1.0f))
	{
		float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};
		
		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(vertices, 6 * 36 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_lightVAO = std::make_unique<VertexArray>();

		m_lightVAO->AddBuffer(*m_VertexBuffer, layout);

// 		glfwSetCursorPosCallback(glfwGetCurrentContext(), mouse_callback);
// 		glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);

		// tell GLFW to capture our mouse
		//glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		GLCall(glEnable(GL_DEPTH_TEST));

		// 这里有两个着色器可以挑选，一个是Gouraud着色器，一个是Phong着色器
		// 文件名分别是：lightingGouraud.shader 和 lightingPhong.shader
		// 其中Phong着色器使用的是Blinn-Phong光照模型,也就是halfdir，而没使用reflect
		m_lightingShader = std::make_unique<Shader>("res/shaders/PhongMaterial.shader");
		m_lightingCubeShader = std::make_unique<Shader>("res/shaders/lightingCube.shader");


	}

		TestLight::~TestLight()
	{
	}

	void TestLight::OnUpdate(float deltaTime)
	{
	}

	void TestLight::OnRender()
	{
		GLFWwindow* currentWindow = glfwGetCurrentContext();

		glfwSetCursorPosCallback(glfwGetCurrentContext(), mouse_callback);
		glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);

		float currentFrame = static_cast<float>(glfwGetTime());
		camera1.deltaTime = currentFrame - camera1.lastFrame;
		camera1.lastFrame = currentFrame;

		processInput(currentWindow);

		GLCall(glClearColor(0.1f,0.1f,0.1f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		lightPos= glm::vec3(lightPosX,lightPosY,lightPosZ);

		m_lightingShader->Bind();
		//m_lightingShader->SetUniform3f("objectColor", 1.0f, 0.5f, 0.31f);
		m_lightingShader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
		m_lightingShader->SetUniform3f("light.position", lightPos);
		m_lightingShader->SetUniform3f("viewPos", camera1.cameraPos);

		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);

		glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

		m_lightingShader->SetUniform3f("material.ambient", ambientColor);
		m_lightingShader->SetUniform3f("material.diffuse", diffuseColor);
		m_lightingShader->SetUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
		m_lightingShader->SetUniform1f("material.shininess", 32.0f);
		m_lightingShader->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
		m_lightingShader->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f);
		m_lightingShader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

		Renderer renderer;
		
		model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera1.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		m_View = glm::lookAt(camera1.cameraPos, camera1.cameraPos + camera1.cameraFront, camera1.cameraUp);
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
		model = glm::scale(model, glm::vec3(0.15f));
		m_lightingCubeShader->SetUniformMat4("model", model);
		m_lightVAO->Bind();
		renderer.Draw(*m_lightVAO,  *m_lightingCubeShader,36);
	}

	void TestLight::OnImGuiRender()
	{
  		ImGui::SliderFloat(" lightPos X", &lightPosX, -3.0f,3.0f);
  		ImGui::SliderFloat(" lightPos Y", &lightPosY, -3.0f,3.0f);
  		ImGui::SliderFloat(" lightPos Z", &lightPosZ, -3.0f,3.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

