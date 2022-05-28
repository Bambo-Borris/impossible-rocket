#include "InputHandler.hpp"

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Event.hpp>
#include <cassert>
#include <imgui-sfml/imgui-SFML.h>

constexpr auto AXIS_DEADZONE_LOWER = 5.0f;
constexpr auto AXIS_DEADZONE_UPPER = 95.0f;

float get_normalized_axis_value(float pos)
{
	float pos_abs = fabsf(pos);
	const float sign = pos_abs / pos;
	if (pos_abs > AXIS_DEADZONE_LOWER)
	{
		pos_abs = std::min(AXIS_DEADZONE_UPPER, pos_abs);
		pos_abs += (100.0f - AXIS_DEADZONE_UPPER);
		pos_abs /= 100.0f;
		pos_abs *= sign;
		return pos_abs;
	}
	return 0.0f;
}

InputHandler &InputHandler::get()
{
	static InputHandler &handler = *new InputHandler();
	return handler;
}

void InputHandler::handleEvents(sf::RenderWindow &window)
{
	m_resetPressed = false;
	m_debugSkipPressed = false;
	sf::Event event;
	while (window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(window, event);
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Key::Escape)
				window.close();
		}
		handleKeyboard(event);
		switch (m_padType)
		{
		case PadType::Xbox_Pad:
			handleXbox(event);
			break;

		case PadType::DS4_Pad:
			handleDS4(event);
			break;
		default:
			assert(false);
			break;
		}
	}
}

auto InputHandler::getInputState() const -> InputState
{
	return m_state;
}

auto InputHandler::wasResetPressed() const -> bool
{
	return m_resetPressed;
}

auto InputHandler::getPadType() const -> PadType
{
	return m_padType;
}

auto InputHandler::debugSkipPressed() const -> bool
{
	return m_debugSkipPressed;
}

void InputHandler::handleKeyboard(const sf::Event &event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Key::W)
			m_state.linear_thrust = 1.0f;

		if (event.key.code == sf::Keyboard::Key::S)
			m_state.linear_thrust = -1.0f;

		if (event.key.code == sf::Keyboard::Key::A)
			m_state.angular_thrust = -1.0f;

		if (event.key.code == sf::Keyboard::Key::D)
			m_state.angular_thrust = 1.0f;
	}

	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code == sf::Keyboard::Key::R)
			m_resetPressed = true;

		if (event.key.code == sf::Keyboard::N)
			m_debugSkipPressed = true;

		if (event.key.code == sf::Keyboard::F3)
		{
			switch (m_padType)
			{
			case PadType::Xbox_Pad:
				m_padType = PadType::DS4_Pad;
				break;

			case PadType::DS4_Pad:
				m_padType = PadType::Xbox_Pad;
				break;
			default:
				assert(false);
				break;
			}
		}

		if (event.key.code == sf::Keyboard::Key::W)
			m_state.linear_thrust = 0.0f;

		if (event.key.code == sf::Keyboard::Key::S)
			m_state.linear_thrust = 0.0f;

		if (event.key.code == sf::Keyboard::Key::A)
			m_state.angular_thrust = 0.0f;

		if (event.key.code == sf::Keyboard::Key::D)
			m_state.angular_thrust = 0.0f;
	}
}

void InputHandler::handleXbox(const sf::Event &event)
{
	const sf::Uint32 START_BUTTON_ID = 0;

	// Button presses
	if (event.type == sf::Event::JoystickButtonPressed)
	{
		if (event.joystickButton.button == START_BUTTON_ID)
		{
			m_resetPressed = true;
		}
	}

	if (event.type == sf::Event::JoystickButtonReleased)
	{
		if (event.joystickButton.button == START_BUTTON_ID)
		{
			m_resetPressed = false;
		}
	}

	// Analogue inputs
	if (event.type == sf::Event::JoystickMoved)
	{

		// Left Joystick
		if (event.joystickMove.axis == sf::Joystick::X)
		{
			m_state.angular_thrust = get_normalized_axis_value(event.joystickMove.position);
		}

		// Left & Right trigger
		if (event.joystickMove.axis == sf::Joystick::Z)
		{
			// The triggers behave as negative values are the right trigger
			// and positive values are the left trigger. So we will invert
			// the value we get back from the normalize function
			m_state.linear_thrust = -get_normalized_axis_value(event.joystickMove.position);
		}
	}
}

void InputHandler::handleDS4(const sf::Event &event)
{
	const sf::Uint32 START_BUTTON_ID = 9;

	// Button presses
	if (event.type == sf::Event::JoystickButtonPressed)
	{
		if (event.joystickButton.button == START_BUTTON_ID)
		{
			m_resetPressed = true;
		}
	}

	if (event.type == sf::Event::JoystickButtonReleased)
	{
		if (event.joystickButton.button == START_BUTTON_ID)
		{
			m_resetPressed = false;
		}
	}

	// Analogue inputs
	if (event.type == sf::Event::JoystickMoved)
	{

		// Left Joystick
		if (event.joystickMove.axis == sf::Joystick::X)
		{
			m_state.angular_thrust = get_normalized_axis_value(event.joystickMove.position);
		}

		// Left trigger
		if (event.joystickMove.axis == sf::Joystick::U)
		{
			if (event.joystickMove.position > -AXIS_DEADZONE_UPPER)
				m_state.linear_thrust = -get_normalized_axis_value(fabsf(event.joystickMove.position));
			else
				m_state.linear_thrust = 0.0f;
		}

		// Right trigger
		if (event.joystickMove.axis == sf::Joystick::V)
		{
			if (event.joystickMove.position > -AXIS_DEADZONE_UPPER)
				m_state.linear_thrust = get_normalized_axis_value(fabsf(event.joystickMove.position));
			else
				m_state.linear_thrust = 0.0f;
		}
	}
}
