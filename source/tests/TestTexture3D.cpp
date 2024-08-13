#include "TestTexture3D.h"

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


	static void processInput(GLFWwindow* window);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	Camera camera0;

	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * camera0.deltaTime);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera0.cameraPos += cameraSpeed * camera0.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera0.cameraPos -= cameraSpeed * camera0.cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera0.cameraPos -= glm::normalize(glm::cross(camera0.cameraFront, camera0.cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera0.cameraPos += glm::normalize(glm::cross(camera0.cameraFront, camera0.cameraUp)) * cameraSpeed;
	}

	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (camera0.firstMouse)
		{
			camera0.lastX = xpos;
			camera0.lastY = ypos;
			camera0.firstMouse = false;
		}

		float xoffset = xpos - camera0.lastX;
		float yoffset = camera0.lastY - ypos; // reversed since y-coordinates go from bottom to top
		camera0.lastX = xpos;
		camera0.lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera0.yaw += xoffset;
		camera0.pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (camera0.pitch > 89.0f)
			camera0.pitch = 89.0f;
		if (camera0.pitch < -89.0f)
			camera0.pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(camera0.yaw)) * cos(glm::radians(camera0.pitch));
		front.y = sin(glm::radians(camera0.pitch));
		front.z = sin(glm::radians(camera0.yaw)) * cos(glm::radians(camera0.pitch));
		camera0.cameraFront = glm::normalize(front);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera0.fov -= (float)yoffset;
		if (camera0.fov < 1.0f)
			camera0.fov = 1.0f;
		if (camera0.fov > 45.0f)
			camera0.fov = 45.0f;
	}



	TestTexture3D::TestTexture3D()
		:m_Proj(glm::mat4(1.0f)),
		m_View(glm::mat4(1.0f)),
		model(glm::mat4(1.0f)),
		ModelRotateX(0.0f),ModelRotateY(0.0f),ModelRotateZ(0.0f), ProjDistance(45.0f)
	{

		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
		};

		glfwSetCursorPosCallback(glfwGetCurrentContext(), mouse_callback);
		glfwSetScrollCallback(glfwGetCurrentContext(), scroll_callback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		GLCall(glEnable(GL_DEPTH_TEST));

		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(vertices, 5 * 36 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		//m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		m_Shader = std::make_unique<Shader>("res/shaders/learnopengl.shader");
		m_Shader->Bind();
		m_Texture1 = std::make_unique<Texture>("res/textures/container.jpg");
		m_Texture2 = std::make_unique<Texture>("res/textures/awesomeface.png");
		m_Shader->Bind();
		m_Shader->SetUniform1i("texture1",0);
		m_Shader->SetUniform1i("texture2",1);

	}

	TestTexture3D::~TestTexture3D()
	{
	}

	void TestTexture3D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture3D::OnRender()
	{
		GLFWwindow* currentWindow = glfwGetCurrentContext();

		float currentFrame = static_cast<float>(glfwGetTime());
		camera0.deltaTime = currentFrame - camera0.lastFrame;
		camera0.lastFrame = currentFrame;

		processInput(currentWindow);

		GLCall(glClearColor(1.0f,1.0f,1.0f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		Renderer renderer;
		 
		m_Texture1->Bind(0);
		m_Texture2->Bind(1);

		model = glm::rotate(model, glm::radians(ModelRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(ModelRotateY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(ModelRotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
		m_View = glm::lookAt(camera0.cameraPos, camera0.cameraPos + camera0.cameraFront, camera0.cameraUp);
		m_Proj = glm::perspective(glm::radians(camera0.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 mvp = m_Proj * m_View * model;

		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_MVP", mvp);
		renderer.Draw(*m_VAO,  *m_Shader,36);
	}

	void TestTexture3D::OnImGuiRender()
	{
//  		ImGui::SliderFloat("RotateAngleX", &ModelRotateX, -180.0f, 180.0f);            // Edit 1 float using a slider from 0.0f to 1.0f   
// 		ImGui::SliderFloat("RotateAngleY", &ModelRotateY, -180.0f, 180.0f);
// 		ImGui::SliderFloat("RotateAngleZ", &ModelRotateZ, -180.0f, 180.0f);
//  		ImGui::SliderFloat("Camera Distance", &ProjDistance, -20.0f,160.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	

}
