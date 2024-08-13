#include "TestFlashlight.h"

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

	Camera camera3;

	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * camera3.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera3.cameraPos += cameraSpeed * camera3.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera3.cameraPos -= cameraSpeed * camera3.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera3.cameraPos -= glm::normalize(glm::cross(camera3.cameraFront, camera3.cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera3.cameraPos += glm::normalize(glm::cross(camera3.cameraFront, camera3.cameraUp)) * cameraSpeed;
	}

	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (camera3.firstMouse)
		{
			camera3.lastX = xpos;
			camera3.lastY = ypos;
			camera3.firstMouse = false;
		}

		float xoffset = xpos - camera3.lastX;
		float yoffset = camera3.lastY - ypos; // reversed since y-coordinates go from bottom to top
		camera3.lastX = xpos;
		camera3.lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera3.yaw += xoffset;
		camera3.pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera3.pitch > 89.0f)
			camera3.pitch = 89.0f;
		if (camera3.pitch < -89.0f)
			camera3.pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera3.yaw)) * cos(glm::radians(camera3.pitch));
		front.y = sin(glm::radians(camera3.pitch));
		front.z = sin(glm::radians(camera3.yaw)) * cos(glm::radians(camera3.pitch));
		camera3.cameraFront = glm::normalize(front);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera3.fov -= (float)yoffset;
		if (camera3.fov < 1.0f)
			camera3.fov = 1.0f;
		if (camera3.fov > 45.0f)
			camera3.fov = 45.0f;
	}

// 	glm::vec3 cubePositions[] = {
// 		glm::vec3(0.0f,  0.0f,  0.0f),
// 		glm::vec3(2.0f,  5.0f, -15.0f),
// 		glm::vec3(-1.5f, -2.2f, -2.5f),
// 		glm::vec3(-3.8f, -2.0f, -12.3f),
// 		glm::vec3(2.4f, -0.4f, -3.5f),
// 		glm::vec3(-1.7f,  3.0f, -7.5f),
// 		glm::vec3(1.3f, -2.0f, -2.5f),
// 		glm::vec3(1.5f,  2.0f, -2.5f),
// 		glm::vec3(1.5f,  0.2f, -1.5f),
// 		glm::vec3(-1.3f,  1.0f, -1.5f)
// 	};

	TestFlashlight::TestFlashlight()
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

		m_lightingShader = std::make_unique<Shader>("res/shaders/Flashlight.shader");
		m_DiffuseMap = std::make_unique<Texture>("res/textures/container2.png");
		m_SpecularMap = std::make_unique<Texture>("res/textures/container2_specular.png");

	}

	TestFlashlight::~TestFlashlight()
	{
	}

	void TestFlashlight::OnUpdate(float deltaTime)
	{
	}

	void TestFlashlight::OnRender()
	{
		GLFWwindow* currentWindow = glfwGetCurrentContext();

		float currentFrame = static_cast<float>(glfwGetTime());
		camera3.deltaTime = currentFrame - camera3.lastFrame;
		camera3.lastFrame = currentFrame;

		processInput(currentWindow);

		GLCall(glClearColor(0.1f,0.1f,0.1f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_DiffuseMap->Bind(0);
		m_SpecularMap->Bind(1);

		m_lightingShader->Bind();
		m_lightingShader->SetUniform1i("material.diffuse", 0);
		m_lightingShader->SetUniform1i("material.specular", 1);
		m_lightingShader->SetUniform1f("material.shininess", 32.0f);
		//m_lightingShader->SetUniform3f("material.specular", 0.5f, 0.5f, 0.5f);

		m_lightingShader->SetUniform3f("light.position", camera3.cameraPos);
		m_lightingShader->SetUniform3f("light.direction", camera3.cameraFront);
		m_lightingShader->SetUniform1f("light.cutOff", glm::cos(glm::radians(12.5f)));
		m_lightingShader->SetUniform1f("light.outerCutOff", glm::cos(glm::radians(17.5f)));

		m_lightingShader->SetUniform3f("light.lightColor", 1.0f, 1.0f, 1.0f);

		m_lightingShader->SetUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
		m_lightingShader->SetUniform3f("light.diffuse", 0.8f, 0.8f, 0.8f);
		m_lightingShader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

		m_lightingShader->SetUniform1f("light.constant", 1.0f);
		m_lightingShader->SetUniform1f("light.linear", 0.09f);
		m_lightingShader->SetUniform1f("light.quadratic", 0.032f);

		m_lightingShader->SetUniform3f("viewPos", camera3.cameraPos);

		Renderer renderer;

		glm::mat4 projection = glm::perspective(glm::radians(camera3.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		m_View = glm::lookAt(camera3.cameraPos, camera3.cameraPos + camera3.cameraFront, camera3.cameraUp);
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
		
	}

	void TestFlashlight::OnImGuiRender()
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	
	

}

