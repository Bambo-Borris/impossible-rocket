#include "PauseMenu.hpp"
#include "AssetHolder.hpp"
#include "GameplayBlackboard.hpp"
#include "InputHandler.hpp"
#include "SFUtility.hpp"

PauseMenu::PauseMenu(sf::RenderWindow& window)
    : m_window(window)
{
    auto const font { AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf") };
    auto const hoverSfx { AssetHolder::get().getSoundBuffer("bin/sounds/menu_hover.wav") };

    // Pause menu dimmer shape
    m_pauseMenuDim.setSize(sf::Vector2f { m_window.getSize() });
    m_pauseMenuDim.setFillColor({ 90, 90, 90, 100 });

    // Pause menu text
    m_uiPauseTitle.setFont(*font);
    m_uiPauseTitle.setString("Paused");
    m_uiPauseTitle.setCharacterSize(bb::TITLE_FONT_SIZE);
    m_uiPauseTitle.setStyle(sf::Text::Style::Bold);
    CentreTextOrigin(m_uiPauseTitle);
    m_uiPauseTitle.setPosition(
        { static_cast<float>(m_window.getSize().x) / 2.0f, (m_uiPauseTitle.getGlobalBounds().height / 2.0f) + 150.0f });

    // Pause UI buttons
    m_uiResumeButton.setFont(*font);
    m_uiResumeButton.setString("Resume");
    m_uiResumeButton.setCharacterSize(bb::BUTTON_FONT_SIZE);
    CentreTextOrigin(m_uiResumeButton);
    m_uiResumeButton.setPosition(
        m_uiPauseTitle.getPosition()
        + sf::Vector2f { 0.0f, GetHalfBounds(m_uiPauseTitle.getGlobalBounds()).y + bb::BUTTON_SPACING });

    m_uiOptionsButton.setFont(*font);
    m_uiOptionsButton.setString("Options");
    m_uiOptionsButton.setCharacterSize(bb::BUTTON_FONT_SIZE);
    CentreTextOrigin(m_uiOptionsButton);
    m_uiOptionsButton.setPosition(
        m_uiResumeButton.getPosition()
        + sf::Vector2f { 0.0f, GetHalfBounds(m_uiOptionsButton.getGlobalBounds()).y + bb::BUTTON_SPACING });

    m_uiQuitButton.setFont(*font);
    m_uiQuitButton.setString("Quit");
    m_uiQuitButton.setCharacterSize(bb::BUTTON_FONT_SIZE);
    CentreTextOrigin(m_uiQuitButton);
    m_uiQuitButton.setPosition(
        m_uiOptionsButton.getPosition()
        + sf::Vector2f { 0.0f, GetHalfBounds(m_uiQuitButton.getGlobalBounds()).y + bb::BUTTON_SPACING });

    m_pauseButtonHoverSfx.setBuffer(*hoverSfx);
}

void PauseMenu::update(const sf::Time& dt)
{
    (void)dt;
    auto& ih = InputHandler::get();
    const auto mousePos = ih.getMousePosition();

    static sf::Int32 lastHovered = -1;
    const bool hoveredResume = m_uiResumeButton.getGlobalBounds().contains(mousePos);
    const bool hoveredOptions = m_uiOptionsButton.getGlobalBounds().contains(mousePos);
    const bool hoveredQuit = m_uiQuitButton.getGlobalBounds().contains(mousePos);

    if (hoveredResume) {
        m_uiResumeButton.setFillColor(sf::Color::Yellow);
        if (m_pauseButtonHoverSfx.getStatus() != sf::Sound::Status::Playing && lastHovered != 0) {
            m_pauseButtonHoverSfx.play();
            lastHovered = 0;
        }

        if (ih.leftClickPressed()) {
            m_returnToPlaying = true;
        }
    } else {
        m_uiResumeButton.setFillColor(sf::Color::White);
    }

    if (hoveredOptions) {
        m_uiOptionsButton.setFillColor(sf::Color::Yellow);
        if (m_pauseButtonHoverSfx.getStatus() != sf::Sound::Status::Playing && lastHovered != 1) {
            m_pauseButtonHoverSfx.play();
            lastHovered = 1;
        }

        if (ih.leftClickPressed()) { }
    } else {
        m_uiOptionsButton.setFillColor(sf::Color::White);
    }

    if (hoveredQuit) {
        m_uiQuitButton.setFillColor(sf::Color::Yellow);
        if (m_pauseButtonHoverSfx.getStatus() != sf::Sound::Status::Playing && lastHovered != 2) {
            m_pauseButtonHoverSfx.play();
            lastHovered = 2;
        }

        if (ih.leftClickPressed()) {
            m_window.close();
        }
    } else {
        m_uiQuitButton.setFillColor(sf::Color::White);
    }

    if (!hoveredResume && !hoveredOptions && !hoveredQuit) {
        lastHovered = -1;
    }
}

auto PauseMenu::returnToPlaying() const -> bool { return m_returnToPlaying; }

void PauseMenu::reset() { m_returnToPlaying = false; }

void PauseMenu::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    target.draw(m_pauseMenuDim, states);
    target.draw(m_uiPauseTitle, states);
    target.draw(m_uiResumeButton, states);
    target.draw(m_uiOptionsButton, states);
    target.draw(m_uiQuitButton, states);
}
