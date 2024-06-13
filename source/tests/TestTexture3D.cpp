#include "TestTexture3D.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "VertexBufferLayout.h"
#include "Texture.h"
#include "VertexArray.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

namespace test {

	TestTexture3D::TestTexture3D()
		:m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.0f))),
		ModelRotate(-55.0f), ProjDistance(45.0f)
	{

		float vertices[] = {
			// positions          // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
		};
		unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(vertices, 5 * 4 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		m_Shader = std::make_unique<Shader>("res/shaders/learnopengl.shader");
		m_Shader->Bind();
		unsigned int texture1 = 1,texture2 = 2;
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

		GLCall(glClearColor(0.0f,0.0f,0.0f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;
		 
		m_Texture1->Bind(0);
		m_Texture2->Bind(1);

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		m_View = glm::mat4(1.0f);
		m_Proj = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(ModelRotate), glm::vec3(1.0f, 0.0f, 0.0f));
		m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));
		m_Proj = glm::perspective(glm::radians(ProjDistance), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		//glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationA);
		glm::mat4 mvp = m_Proj * m_View * model;

		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_MVP", mvp);
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void TestTexture3D::OnImGuiRender()
	{
 		ImGui::SliderFloat("Model RotateAngle", &ModelRotate, -180.0f, 180.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
 		ImGui::SliderFloat("Camera Distance", &ProjDistance, -20.0f,160.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}