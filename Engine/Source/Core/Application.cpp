#include "Application.hpp"

#include "Core/Logging/Logger.hpp"
#include "Events/ApplicationEvent.hpp"
#include "glad/glad.h"

#include "Core/Memory/STLAllocator.hpp"

namespace QMBT
{

	Application* Application::s_Instance = nullptr;
	Application::Application(const std::string& name)
		: m_Name(name), m_LayerStackAllocator("LayerStack Allocator")
	{
		LOG_CORE_INFO("Initialized Application {0}", m_Name);
		s_Instance = this;

		m_Window = Window::Create(WindowProperties(m_Name));

		QMBT_CORE_ASSERT(m_Window, "Window was not initialized properly!");
		m_Window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{

		while (m_Running)
		{
			Instrumentor::GetInstance().BeginFrame();
			{
				float time = m_Window->GetTime();
				TimeStep timeStep{time - m_LastFrameTime};
				m_LastFrameTime = time;

				if (!m_Minimized)
				{
					glClearColor(0, 0, 0, 1);
					glClear(GL_COLOR_BUFFER_BIT);

					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timeStep);
					}
				}

				m_Window->OnUpdate();
			}

			Instrumentor::GetInstance().EndFrame();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		PROFILE_FUNCTION(ProfileCategory::Layers);
		LOG_CORE_INFO("Pushed Layer to Application LayerStack: {0}", layer->GetName());

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		PROFILE_FUNCTION(ProfileCategory::Layers);
		LOG_CORE_INFO("Pushed Overlay to Application LayerStack: {0}", layer->GetName());

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& event)
	{
		PROFILE_FUNCTION(ProfileCategory::Application);
		//Create a new EventDispatcher with the received event
		EventDispatcher dispatcher(event);

		//If the received event is WindowCloseEvent, then OnWindowClose will be called
		//This is handled by the application only. Most other events are passed to the layers,
		//so they can do whatever they see fit
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(OnWindowClose));
		//dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			//Decrement the iterator and call its OnEvent
			(*--it)->OnEvent(event);

			//If event is handled by the current layer, do not propagate it further
			if (event.IsHandled)
				break;
		}
	}

	bool Application::OnWindowClose(const WindowCloseEvent& event)
	{
		LOG_CORE_INFO("Window Closed");
		m_Running = false;
		return true;
	}
} // namespace QMBT