#pragma once

#include "Events.hpp"

#include "Input/MouseCodes.hpp"
#include "Utility/Enums.hpp"

namespace QMBT
{
	using namespace Utility;

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			//Prints it in the form: MouseMovedEvent: xPos, yPos
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		//Overriding the ToString event to print it in the form:
		//MouseSrolledEvent: xOffset, yOffset
		std::string ToString() const override
		{
			std::stringstream ss;
			//Prints it in the form: MouseMovedEvent: xPos, yPos
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline MouseCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(MouseCode button)
			: m_Button(button) {}

		MouseCode m_Button;
	};

	class MouseButtonClickedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonClickedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			//Prints it in the form: MouseMovedEvent: xPos, yPos
			ss << "MouseButtonClikedEvent: " << EnumToIntegral(m_Button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonClicked)
	};

	class MouseDoubleClickedEvent : public MouseButtonEvent
	{
	public:
		MouseDoubleClickedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseDoubleClickedEvent: " << EnumToIntegral(m_Button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonDoubleClicked)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << EnumToIntegral(m_Button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}
