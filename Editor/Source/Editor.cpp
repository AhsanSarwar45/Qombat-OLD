#include <QMBT.hpp>

#include <Core/EntryPoint.hpp>

#include "ImGui/ImGuiLayer.hpp"

namespace QCreate
{
	class TestLayer : public QMBT::Layer
	{
	  public:
		TestLayer()
			: Layer("Test Layer")
		{
		}

		void OnUpdate(const QMBT::TimeStep& timeStep) override
		{
		}

		void OnEvent(QMBT::Event& event) override
		{
		}
	};
} // namespace QCreate

using namespace QCreate;

class Editor : public QMBT::Application
{
  public:
	Editor()
		: Application("QCreate")
	{
		m_TestLayer = m_LayerStackAllocator.New<TestLayer>();
		m_ImGuiLayer = m_LayerStackAllocator.New<ImGuiLayer>();
		PushLayer(m_TestLayer);
		PushLayer(m_ImGuiLayer);
	}
	~Editor()
	{
		m_LayerStackAllocator.Delete(m_TestLayer);
		m_LayerStackAllocator.Delete(m_ImGuiLayer);
	}

	TestLayer* m_TestLayer;
	ImGuiLayer* m_ImGuiLayer;
};

QMBT::Application* QMBT::CreateApplication()
{
	return new Editor();
}