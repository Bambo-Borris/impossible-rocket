#include "PauseMenu.hpp"
#include "AssetHolder.hpp"
#include "GameplayBlackboard.hpp"
#include "InputHandler.hpp"
#include "SFUtility.hpp"

#include <cassert>
#include <spdlog/fmt/fmt.h>

PauseMenu::PauseMenu(sf::RenderWindow& window, SoundCentral& soundCentral)
    : m_window(window)
    , m_soundCentral(&soundCentral)
{
    setupUIText();
    // Pause menu dimmer shape
    m_pauseMenuDim.setSize(sf::Vector2f { m_window.getSize() });
    m_pauseMenuDim.setFillColor({ 90, 90, 90, 100 });

    const auto volumeButtonRadius = static_cast<float>(m_uiMasterVolumeIndicator.getGlobalBounds().height) / 2.0f;
    m_uiUpVolume.setPointCount(3);
    m_uiUpVolume.setRadius(volumeButtonRadius);
    m_uiUpVolume.setOrigin({ volumeButtonRadius, volumeButtonRadius });
    m_uiUpVolume.setRotation(sf::degrees(90.0f));

    const auto size = m_uiMasterVolumeIndicator.getGlobalBounds().getSize();
    m_uiUpVolume.setPosition(m_uiMasterVolumeIndicator.getPosition()
                             + sf::Vector2f { (size.x / 2.0f) + volumeButtonRadius + bb::VOLUME_BUTTON_SPACING, 0.0f });

    m_uiDownVolume.setPointCount(3);
    m_uiDownVolume.setRadius(volumeButtonRadius);
    m_uiDownVolume.setOrigin({ volumeButtonRadius, volumeButtonRadius });
    m_uiDownVolume.setRotation(sf::degrees(-90.0f));
    m_uiDownVolume.setPosition(
        m_uiMasterVolumeIndicator.getPosition()
        - sf::Vector2f { (size.x / 2.0f) + volumeButtonRadius + bb::VOLUME_BUTTON_SPACING, 0.0f });
}

void PauseMenu::update(const sf::Time& dt)
{
    switch (m_stage) {
    default:
    case PauseMenu::SubMenuStage::Default:
        updateDefault(dt);
        break;
    case PauseMenu::SubMenuStage::Options:
        updateOptions(dt);
        break;
    }
}

auto PauseMenu::returnToPlaying() const -> bool { return m_returnToPlaying; }

void PauseMenu::reset()
{
    m_returnToPlaying = false;
    m_stage = SubMenuStage::Default;
}

void PauseMenu::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    target.draw(m_pauseMenuDim, states);
    target.draw(m_uiPauseTitle, states);

    switch (m_stage) {
    case PauseMenu::SubMenuStage::Default:
        target.draw(m_uiResumeButton, states);
        target.draw(m_uiResumeButton, states);
        target.draw(m_uiOptionsButton, states);
        target.draw(m_uiQuitButton, states);
        break;
    case PauseMenu::SubMenuStage::Options:
        target.draw(m_uiMasterVolumeTitle, states);
        target.draw(m_uiMasterVolumeIndicator, states);
        target.draw(m_uiUpVolume, states);
        target.draw(m_uiDownVolume, states);
        break;
    default:
        assert(false);
        break;
    }
}

void PauseMenu::setupUIText()
{
    auto const font { AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf") };

    setupTextProperty(m_uiPauseTitle, font, "Paused", bb::TITLE_FONT_SIZE);
    m_uiPauseTitle.setStyle(sf::Text::Style::Bold);
    m_uiPauseTitle.setPosition(
        { static_cast<float>(m_window.getSize().x) / 2.0f, (m_uiPauseTitle.getGlobalBounds().height / 2.0f) + 150.0f });

    // Pause UI buttons
    setupTextProperty(m_uiResumeButton, font, "Resume", bb::BUTTON_FONT_SIZE);
    m_uiResumeButton.setPosition(getSpacedLocation(m_uiPauseTitle));

    setupTextProperty(m_uiOptionsButton, font, "Options", bb::BUTTON_FONT_SIZE);
    m_uiOptionsButton.setPosition(getSpacedLocation(m_uiResumeButton));

    setupTextProperty(m_uiQuitButton, font, "Quit", bb::BUTTON_FONT_SIZE);
    m_uiQuitButton.setPosition(getSpacedLocation(m_uiOptionsButton));

    setupTextProperty(m_uiMasterVolumeTitle, font, "Master Volume", bb::BUTTON_FONT_SIZE);
    m_uiMasterVolumeTitle.setPosition(getSpacedLocation(m_uiPauseTitle));

    setupTextProperty(m_uiMasterVolumeIndicator, font, "100%", bb::BUTTON_FONT_SIZE);
    m_uiMasterVolumeIndicator.setPosition(getSpacedLocation(m_uiMasterVolumeTitle));
}

void PauseMenu::setupTextProperty(sf::Text& text,
                                  sf::Font* font,
                                  const sf::String& initString,
                                  sf::Uint32 charSize) const
{
    text.setFont(*font);
    text.setString(initString);
    text.setCharacterSize(charSize);
    CentreTextOrigin(text);
}

sf::Vector2f PauseMenu::getSpacedLocation(const sf::Text& previousNode) const
{
    return previousNode.getPosition()
        + sf::Vector2f { 0.0f, GetHalfBounds(previousNode.getGlobalBounds()).y + bb::BUTTON_SPACING };
}

void PauseMenu::updateDefault(const sf::Time& dt)
{
    (void)dt;
    auto& ih = InputHandler::get();

    if (updateHoveredStatus(m_uiResumeButton)) {
        if (ih.leftClickPressed())
            m_returnToPlaying = true;
    } else if (updateHoveredStatus(m_uiOptionsButton)) {
        if (ih.leftClickPressed())
            m_stage = PauseMenu::SubMenuStage::Options;
    } else if (updateHoveredStatus(m_uiQuitButton)) {
        if (ih.leftClickPressed())
            m_window.close();
    } else {
        m_lastHoveredShape = nullptr;
        m_lastHoveredText = nullptr;
    }
}

void PauseMenu::updateOptions(const sf::Time& dt)
{
    (void)dt;
    auto masterVol = static_cast<sf::Uint32>(m_soundCentral->getMasterVolume());

    if (updateHoveredStatus(m_uiUpVolume)) {
        if (InputHandler::get().leftClickHeld() && masterVol < 100)
            masterVol += 1;
    }

    if (updateHoveredStatus(m_uiDownVolume)) {
        if (InputHandler::get().leftClickHeld() && masterVol > 0)
            masterVol -= 1;
    }

    m_soundCentral->setMasterVolume(static_cast<float>(masterVol));
    updateVolumeUIPositions();
}

bool PauseMenu::updateHoveredStatus(sf::Shape& shape)
{
    const auto mousePosition = InputHandler::get().getMousePosition();
    const auto containsResult = shape.getGlobalBounds().contains(mousePosition);

    if (containsResult) {
        if (m_soundCentral->getSoundStatus(SoundCentral::SoundEffectTypes::MenuItemHover) != sf::Sound::Status::Playing
            && m_lastHoveredShape != &shape) {
            m_soundCentral->playSoundEffect(SoundCentral::SoundEffectTypes::MenuItemHover);
            m_lastHoveredShape = &shape;
        }
        shape.setFillColor(sf::Color::Yellow);
    } else
        shape.setFillColor(sf::Color::White);

    return containsResult;
}

bool PauseMenu::updateHoveredStatus(sf::Text& text)
{
    const auto mousePosition = InputHandler::get().getMousePosition();
    const auto containsResult = text.getGlobalBounds().contains(mousePosition);
    if (containsResult) {
        if (m_soundCentral->getSoundStatus(SoundCentral::SoundEffectTypes::MenuItemHover) != sf::Sound::Status::Playing
            && m_lastHoveredText != &text) {
            m_soundCentral->playSoundEffect(SoundCentral::SoundEffectTypes::MenuItemHover);
            m_lastHoveredText = &text;
        }
        text.setFillColor(sf::Color::Yellow);
    } else
        text.setFillColor(sf::Color::White);

    return containsResult;
}

void PauseMenu::updateVolumeUIPositions()
{
    const auto volumeButtonRadius = m_uiUpVolume.getRadius();
    const auto masterVolume = m_soundCentral->getMasterVolume();

    m_uiMasterVolumeIndicator.setString(fmt::format("{}%", masterVolume));
    CentreTextOrigin(m_uiMasterVolumeIndicator);
    m_uiMasterVolumeIndicator.setPosition(getSpacedLocation(m_uiMasterVolumeTitle));

    const auto size = m_uiMasterVolumeIndicator.getGlobalBounds().getSize();
    m_uiUpVolume.setPosition(m_uiMasterVolumeIndicator.getPosition()
                             + sf::Vector2f { (size.x / 2.0f) + volumeButtonRadius + bb::VOLUME_BUTTON_SPACING, 0.0f });

    m_uiDownVolume.setPosition(
        m_uiMasterVolumeIndicator.getPosition()
        - sf::Vector2f { (size.x / 2.0f) + volumeButtonRadius + bb::VOLUME_BUTTON_SPACING, 0.0f });
}
