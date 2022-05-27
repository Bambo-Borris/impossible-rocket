#pragma once

#include <SFML/Window/Event.hpp>

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
	void handleEvents(const sf::Event &event);

	auto getInputState() const -> InputState;
	auto wasResetPressed() const -> bool;
	auto getPadType() const -> PadType;

	void resetConsumed();

private:
	InputHandler() = default;

	void handleKeyboard(const sf::Event &event);
	void handleXbox(const sf::Event &event);
	void handleDS4(const sf::Event &event);

	PadType m_padType{PadType::Xbox_Pad};
	InputState m_state{};
	bool m_resetPressed{false};
};
