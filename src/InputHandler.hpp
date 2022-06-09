#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class InputHandler
{
public:
	struct InputState
	{
		float linear_thrust{0.0f};	// +ve = forward, -ve = backwards
		float angular_thrust{0.0f}; // +ve clockwise, -ve anticlockwise
	};

	enum class PadType
	{
		Xbox_Pad,
		DS4_Pad
	};

	static InputHandler &get();
	void handleEvents(sf::RenderWindow &window);

	auto getInputState() const -> InputState;
	auto wasResetPressed() const -> bool;
	auto debugSkipPressed() const -> bool;
	auto getMousePosition() const -> sf::Vector2f;
	auto leftClickPressed() const -> bool;
	auto pauseUnpausePressed() const -> bool;

	// Completely halts all forces and velocity on the player rocket, entirely for debug
	// purposes only.
	auto wasHaltKeyPressed() const -> bool;

private:
	InputHandler() = default;

	void handleKeyboard(const sf::Event &event);
	void handleXbox(const sf::Event &event);
	void handleDS4(const sf::Event &event);

	PadType m_padType{PadType::Xbox_Pad};
	InputState m_state{};
	sf::Vector2f m_mousePosition;
	bool m_resetPressed{false};
	bool m_debugSkipPressed{false};
	bool m_leftJustPressed{false};
	bool m_haltKeyPressed{false};
	bool m_pauseUnpausePressed{false};
};
