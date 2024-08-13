#pragma once

#include "Test.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "VertexArray.h"
#include <memory>

namespace test {

	class TestMultiLight :public Test
	{
	public:
		TestMultiLight();
		~TestMultiLight();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexArray> m_lightVAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr < IndexBuffer> m_IndexBuffer;
		std::unique_ptr < Texture > m_DiffuseMap;
		std::unique_ptr < Texture > m_SpecularMap;
		std::unique_ptr < Shader> m_lightingShader;
		std::unique_ptr < Shader> m_lightingCubeShader;

		glm::mat4 m_Proj, m_View,model;
	};

}