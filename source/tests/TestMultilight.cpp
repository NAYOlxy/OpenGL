#include "TestMultiLight.h"

#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"
#include "Texture.h"
#include "VertexArray.h"

// #include "glm/glm.hpp"
// #include "glm/gtc/matrix_transform.hpp"
#include "Positions.h"
#include "Camera.h"


const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

namespace test {
	static void processInput(GLFWwindow* window);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	Camera camera4;

	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * camera4.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera4.cameraPos += cameraSpeed * camera4.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera4.cameraPos -= cameraSpeed * camera4.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera4.cameraPos -= glm::normalize(glm::cross(camera4.cameraFront, camera4.cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera4.cameraPos += glm::normalize(glm::cross(camera4.cameraFront, camera4.cameraUp)) * cameraSpeed;
	}

	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (camera4.firstMouse)
		{
			camera4.lastX = xpos;
			camera4.lastY = ypos;
			camera4.firstMouse = false;
		}

		float xoffset = xpos - camera4.lastX;
		float yoffset = camera4.lastY - ypos; // reversed since y-coordinates go from bottom to top
		camera4.lastX = xpos;
		camera4.lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera4.yaw += xoffset;
		camera4.pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera4.pitch > 89.0f)
			camera4.pitch = 89.0f;
		if (camera4.pitch < -89.0f)
			camera4.pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera4.yaw)) * cos(glm::radians(camera4.pitch));
		front.y = sin(glm::radians(camera4.pitch));
		front.z = sin(glm::radians(camera4.yaw)) * cos(glm::radians(camera4.pitch));
		camera4.cameraFront = glm::normalize(front);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera4.fov -= (float)yoffset;
		if (camera4.fov < 1.0f)
			camera4.fov = 1.0f;
		if (camera4.fov > 45.0f)
			camera4.fov = 45.0f;
	}


	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	TestMultiLight::TestMultiLight()
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
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		GLCall(glEnable(GL_DEPTH_TEST));

		m_lightingShader = std::make_unique<Shader>("res/shaders/Multilight.shader");
		m_DiffuseMap = std::make_unique<Texture>("res/textures/container2.png");
		m_SpecularMap = std::make_unique<Texture>("res/textures/container2_specular.png");
		m_lightingCubeShader = std::make_unique<Shader>("res/shaders/lightingCube.shader");

	}

	TestMultiLight::~TestMultiLight()
	{
	}

	void TestMultiLight::OnUpdate(float deltaTime)
	{
	}

	void TestMultiLight::OnRender()
	{
		GLFWwindow* currentWindow = glfwGetCurrentContext();

		float currentFrame = static_cast<float>(glfwGetTime());
		camera4.deltaTime = currentFrame - camera4.lastFrame;
		camera4.lastFrame = currentFrame;

		processInput(currentWindow);

		GLCall(glClearColor(0.1f,0.1f,0.1f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_DiffuseMap->Bind(0);
		m_SpecularMap->Bind(1);

		m_lightingShader->Bind();
		m_lightingShader->SetUniform3f("viewPos", camera4.cameraPos);
		m_lightingShader->SetUniform1f("material.shininess", 32.0f);

		m_lightingShader->SetUniform3f("dirLight.direction", -0.2f,-1.0f,-0.3f);
		m_lightingShader->SetUniform3f("dirLight.ambient", 0.1f, 0.1f, 0.1f);
		m_lightingShader->SetUniform3f("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		m_lightingShader->SetUniform3f("dirLight.specular", 0.5f, 0.5f, 0.5f);

		m_lightingShader->SetUniform3f("pointLights[0].position", pointLightPositions[0]);
		m_lightingShader->SetUniform3f("pointLights[0].ambient", 0.05f,0.05f,0.05f);
		m_lightingShader->SetUniform3f("pointLights[0].diffuse", 0.8f,0.8f,0.8f);
		m_lightingShader->SetUniform3f("pointLights[0].specular", 1.0f,1.0f,1.0f);
		m_lightingShader->SetUniform1f("pointLights[0].constant", 1.0f);
		m_lightingShader->SetUniform1f("pointLights[0].linear", 0.09f);
		m_lightingShader->SetUniform1f("pointLights[0].quadratic", 0.032f);

		m_lightingShader->SetUniform3f("pointLights[1].position", pointLightPositions[1]);
		m_lightingShader->SetUniform3f("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		m_lightingShader->SetUniform3f("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		m_lightingShader->SetUniform3f("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		m_lightingShader->SetUniform1f("pointLights[1].constant", 1.0f);
		m_lightingShader->SetUniform1f("pointLights[1].linear", 0.09f);
		m_lightingShader->SetUniform1f("pointLights[1].quadratic", 0.032f);

		m_lightingShader->SetUniform3f("pointLights[2].position", pointLightPositions[2]);
		m_lightingShader->SetUniform3f("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		m_lightingShader->SetUniform3f("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		m_lightingShader->SetUniform3f("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		m_lightingShader->SetUniform1f("pointLights[2].constant", 1.0f);
		m_lightingShader->SetUniform1f("pointLights[2].linear", 0.09f);
		m_lightingShader->SetUniform1f("pointLights[2].quadratic", 0.032f);

		m_lightingShader->SetUniform3f("pointLights[3].position", pointLightPositions[3]);
		m_lightingShader->SetUniform3f("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		m_lightingShader->SetUniform3f("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		m_lightingShader->SetUniform3f("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		m_lightingShader->SetUniform1f("pointLights[3].constant", 1.0f);
		m_lightingShader->SetUniform1f("pointLights[3].linear", 0.09f);
		m_lightingShader->SetUniform1f("pointLights[3].quadratic", 0.032f);

		// Color of FlashLight
		m_lightingShader->SetUniform3f("spotLight.lightColor", 1.0f, 0.0f, 0.0f);
		m_lightingShader->SetUniform3f("spotLight.position", camera4.cameraPos);
		m_lightingShader->SetUniform3f("spotLight.direction", camera4.cameraFront);
		m_lightingShader->SetUniform3f("spotLight.ambient", 0.0f,0.0f,0.0f);
		m_lightingShader->SetUniform3f("spotLight.diffuse", 1.0f,1.0f,1.0f);
		m_lightingShader->SetUniform3f("spotLight.specular", 1.0f,1.0f,1.0f);
		m_lightingShader->SetUniform1f("spotLight.constant", 1.0f);
		m_lightingShader->SetUniform1f("spotLight.linear", 0.09f);
		m_lightingShader->SetUniform1f("spotLight.quadratic", 0.032f);

		m_lightingShader->SetUniform1i("material.diffuse", 0);
		m_lightingShader->SetUniform1i("material.specular", 1);

		m_lightingShader->SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		m_lightingShader->SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

		Renderer renderer;

		glm::mat4 projection = glm::perspective(glm::radians(camera4.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		m_View = glm::lookAt(camera4.cameraPos, camera4.cameraPos + camera4.cameraFront, camera4.cameraUp);
		m_lightingShader->SetUniformMat4("projection", projection);
		m_lightingShader->SetUniformMat4("view", m_View);
		
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			m_lightingShader->SetUniformMat4("model", model);

			m_VAO->Bind();

			renderer.Draw(*m_VAO, *m_lightingShader, 36);
		}
		
		m_lightingCubeShader->Bind();
		m_lightingCubeShader->SetUniformMat4("projection", projection);
		m_lightingCubeShader->SetUniformMat4("view", m_View);

		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			m_lightingCubeShader->SetUniformMat4("model", model);

			renderer.Draw(*m_lightVAO, *m_lightingCubeShader, 36);
		}
	}

	void TestMultiLight::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}

