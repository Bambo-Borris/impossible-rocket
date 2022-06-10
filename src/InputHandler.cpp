#include "InputHandler.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <cassert>
#include <imgui-SFML.h>

constexpr auto AXIS_DEADZONE_LOWER = 5.0f;
constexpr auto AXIS_DEADZONE_UPPER = 95.0f;

float get_normalized_axis_value(float pos)
{
    float pos_abs = fabsf(pos);
    const float sign = pos_abs / pos;
    if (pos_abs > AXIS_DEADZONE_LOWER) {
        pos_abs = std::min(AXIS_DEADZONE_UPPER, pos_abs);
        pos_abs += (100.0f - AXIS_DEADZONE_UPPER);
        pos_abs /= 100.0f;
        pos_abs *= sign;
        return pos_abs;
    }
    return 0.0f;
}

InputHandler& InputHandler::get()
{
    static InputHandler& handler = *new InputHandler();
    return handler;
}

void InputHandler::handleEvents(sf::RenderWindow& window)
{
    // Maybe this could be improved upon to make
    // adding things like this less painful...
    m_resetPressed = false;
    m_debugSkipPressed = false;
    m_leftJustPressed = false;
    m_haltKeyPressed = false;
    m_pauseUnpausePressed = false;

    sf::Event event;
    while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(window, event);

        if (sf::Joystick::isConnected(0)) {
            auto id = sf::Joystick::getIdentification(0);
            if (id.vendorId == 1118) {
                m_padType = PadType::Xbox_Pad;
            } else if (id.vendorId == 1356) {
                m_padType = PadType::DS4_Pad;
            }
        }

        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed)
            if (event.key.code == sf::Keyboard::Key::Escape)
                window.close();

        if (event.type == sf::Event::MouseMoved) {
            const sf::Vector2i pos { event.mouseMove.x, event.mouseMove.y };
            m_mousePosition = window.mapPixelToCoords(pos);
        }

        if (event.type == sf::Event::MouseButtonReleased)
            if (event.mouseButton.button == sf::Mouse::Left)
                m_leftJustPressed = true;

        handleKeyboard(event);
        switch (m_padType) {
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

auto InputHandler::getInputState() const -> InputState { return m_state; }

auto InputHandler::wasResetPressed() const -> bool { return m_resetPressed; }

auto InputHandler::debugSkipPressed() const -> bool { return m_debugSkipPressed; }

auto InputHandler::getMousePosition() const -> sf::Vector2f { return m_mousePosition; }

auto InputHandler::leftClickPressed() const -> bool { return m_leftJustPressed; }

auto InputHandler::pauseUnpausePressed() const -> bool { return m_pauseUnpausePressed; }

auto InputHandler::wasHaltKeyPressed() const -> bool { return m_haltKeyPressed; }

auto InputHandler::joystickActionButtonPressed() const -> bool { return m_joystickActionButtonPressed; }

void InputHandler::handleKeyboard(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::W)
            m_state.linear_thrust = 1.0f;

        if (event.key.code == sf::Keyboard::Key::S)
            m_state.linear_thrust = -1.0f;

        if (event.key.code == sf::Keyboard::Key::A)
            m_state.angular_thrust = -1.0f;

        if (event.key.code == sf::Keyboard::Key::D)
            m_state.angular_thrust = 1.0f;
    }

    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Key::R)
            m_resetPressed = true;

#if defined(IMPOSSIBLE_ROCKET_DEBUG)
        if (event.key.code == sf::Keyboard::P)
            m_pauseUnpausePressed = true;
#else
        if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape)
            m_pauseUnpausePressed = true;
#endif

        if (event.key.code == sf::Keyboard::N)
            m_debugSkipPressed = true;

        if (event.key.code == sf::Keyboard::H)
            m_haltKeyPressed = true;

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

void InputHandler::handleXbox(const sf::Event& event)
{
    constexpr sf::Uint32 START_BUTTON_ID = 7;
    constexpr sf::Uint32 ACTION_BUTTON_ID = 0;

    // Button presses
    if (event.type == sf::Event::JoystickButtonPressed) {
        if (event.joystickButton.button == START_BUTTON_ID)
            m_resetPressed = true;

        if (event.joystickButton.button == ACTION_BUTTON_ID)
            m_joystickActionButtonPressed = true;
    }

    if (event.type == sf::Event::JoystickButtonReleased) {
        if (event.joystickButton.button == START_BUTTON_ID)
            m_resetPressed = false;

        if (event.joystickButton.button == ACTION_BUTTON_ID)
            m_joystickActionButtonPressed = false;
    }

    // Analogue inputs
    if (event.type == sf::Event::JoystickMoved) {

        // Left Joystick
        if (event.joystickMove.axis == sf::Joystick::X)
            m_state.angular_thrust = get_normalized_axis_value(event.joystickMove.position);

        // Left & Right trigger
        if (event.joystickMove.axis == sf::Joystick::Z) {
            // The triggers behave as negative values are the right trigger
            // and positive values are the left trigger. So we will invert
            // the value we get back from the normalize function
            m_state.linear_thrust = -get_normalized_axis_value(event.joystickMove.position);
        }
    }
}

void InputHandler::handleDS4(const sf::Event& event)
{
    constexpr sf::Uint32 START_BUTTON_ID = 9;
    constexpr sf::Uint32 ACTION_BUTTON_ID = 1;

    // Button presses
    if (event.type == sf::Event::JoystickButtonPressed) {
        if (event.joystickButton.button == START_BUTTON_ID)
            m_resetPressed = true;
        if (event.joystickButton.button == ACTION_BUTTON_ID)
            m_joystickActionButtonPressed = true;
    }

    if (event.type == sf::Event::JoystickButtonReleased) {
        if (event.joystickButton.button == START_BUTTON_ID)
            m_resetPressed = false;
        if (event.joystickButton.button == ACTION_BUTTON_ID)
            m_joystickActionButtonPressed = false;
    }

    // Analogue inputs
    if (event.type == sf::Event::JoystickMoved) {
        // Left Joystick
        if (event.joystickMove.axis == sf::Joystick::X)
            m_state.angular_thrust = get_normalized_axis_value(event.joystickMove.position);

        // Left trigger
        if (event.joystickMove.axis == sf::Joystick::U) {
            if (event.joystickMove.position > -AXIS_DEADZONE_UPPER)
                m_state.linear_thrust = -get_normalized_axis_value(fabsf(event.joystickMove.position));
            else
                m_state.linear_thrust = 0.0f;
        }

        // Right trigger
        if (event.joystickMove.axis == sf::Joystick::V) {
            if (event.joystickMove.position > -AXIS_DEADZONE_UPPER)
                m_state.linear_thrust = get_normalized_axis_value(fabsf(event.joystickMove.position));
            else
                m_state.linear_thrust = 0.0f;
        }
    }
}
