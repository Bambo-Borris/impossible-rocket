#include "PlayState.hpp"
#include "AssetHolder.hpp"
#include "GameplayBlackboard.hpp"
#include "InputHandler.hpp"
#include "SFUtility.hpp"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <string>

constexpr auto MAX_OOB_TIME = 5;
constexpr auto FIXED_TIME_STEP = sf::seconds(1.0f / 120.0f);

PlayState::PlayState(sf::RenderWindow& window)
    : BaseState(window)
    , m_rocket(m_physicsWorld, m_gameLevel)
{
    // First we grab our asset pointers
    auto const bgTexture { AssetHolder::get().getTexture("bin/textures/background_resized.png") };
    auto const oobArrowTexture(AssetHolder::get().getTexture("bin/textures/oob_arrow.png"));
    auto const font { AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf") };
    auto const hoverSfx { AssetHolder::get().getSoundBuffer("bin/sounds/menu_hover.wav") };

    m_gameLevel.loadLevel(GameLevel::Levels::One);
    // Static background shape & texture setup
    bgTexture->setRepeated(true);
    m_backgroundSprite.setSize(sf::Vector2f(m_window.getSize()));
    m_backgroundSprite.setTexture(bgTexture);
    m_backgroundSprite.setTextureRect({ { 0, 0 }, { 600, 400 } });

    // In game UI
    m_uiAttempts.setFont(*font);
    m_uiAttempts.setString("Attempts: 1");

    m_uiOOB.setFont(*font);
    m_uiOOB.setFillColor(sf::Color::Yellow); // Make it catch the eye!

    // Out of bounds arrow
    m_oobDirectionIndicator.setSize({ 32.0f, 32.0f });
    m_oobDirectionIndicator.setTexture(oobArrowTexture);
    m_oobDirectionIndicator.setOrigin({ 16.0f, 16.0f });

    // Pause menu dimmer shape
    m_pauseMenuDim.setSize(sf::Vector2f { m_window.getSize() });
    m_pauseMenuDim.setFillColor({ 90, 90, 90, 100 });

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

void PlayState::update(const sf::Time& dt)
{
    if (InputHandler::get().pauseUnpausePressed()) {
        switch (m_status) {
        case PlayState::Status::Playing:
            m_status = PlayState::Status::Paused;
            break;
        case PlayState::Status::Paused:
            m_status = PlayState::Status::Playing;
            break;
        default:
            // Do nothing I guess
            break;
        }
    }

    switch (m_status) {
    case PlayState::Status::Playing:
        updatePlaying(dt);
        break;
    case PlayState::Status::Paused:
        updatePaused(dt);
        break;
    case PlayState::Status::End_Of_Level:
        updateEndOfLevel(dt);
        break;
    default:
        assert(false);
        break;
    }
}

void PlayState::enter() { m_rocket.levelStart(); }

void PlayState::draw() const
{
    // Gameplay oriented
    m_window.draw(m_backgroundSprite);
    m_window.draw(m_gameLevel);
    if (m_isOutOfBounds) {
        m_window.draw(m_oobDirectionIndicator);
    }
    m_window.draw(m_rocket);
    for (const auto& pe : m_particleEffects) {
        m_window.draw(*pe);
    }

    m_window.draw(m_uiAttempts);
    if (m_isOutOfBounds) {
        m_window.draw(m_uiOOB);
    }

    if (m_status == PlayState::Status::Paused) {
        m_window.draw(m_pauseMenuDim);
        m_window.draw(m_uiPauseTitle);
        m_window.draw(m_uiResumeButton);
        m_window.draw(m_uiOptionsButton);
        m_window.draw(m_uiQuitButton);
    }
}

void PlayState::updatePlaying(const sf::Time& dt)
{
    auto& input = InputHandler::get();
    const bool skipLevel = input.debugSkipPressed();
    static sf::Uint32 attempts = m_gameLevel.getAttemptTotal();

    // Update core gameplay & ImGui
    m_physicsWorld.step(FIXED_TIME_STEP, dt);
    m_gameLevel.update(dt);
    m_rocket.update(dt);

    for (auto& e : m_particleEffects) {
        e->update(dt);
    }

    if (input.wasResetPressed()) {
        m_gameLevel.resetLevel();
        m_rocket.levelStart();
    }

    outOfBoundsUpdate();
    particleEffectUpdate();

    // Roll over to next level
    if (m_gameLevel.isLevelComplete() || skipLevel) {
        const auto current = static_cast<sf::Uint32>(m_gameLevel.getCurrentLevel());
        if (current + 1 >= static_cast<sf::Uint32>(GameLevel::Levels::MAX_LEVEL)) {
            // Do game completion here.
            spdlog::debug("All Levels Complete");
        } else {
            m_gameLevel.loadLevel(static_cast<GameLevel::Levels>(current + 1));
            m_rocket.levelStart();
        }
    }

    if (attempts != m_gameLevel.getAttemptTotal()) {
        attempts = m_gameLevel.getAttemptTotal();
        m_uiAttempts.setString(fmt::format("Attempts: {}", attempts));
    }
}

void PlayState::updatePaused(const sf::Time& dt)
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
            m_status = PlayState::Status::Playing;
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

void PlayState::updateEndOfLevel(const sf::Time& dt) { (void)dt; }

void PlayState::particleEffectUpdate()
{
    // If the player collides with a planet then
    // we should play a particle effect & then
    // once the effect is complete, we'll reset
    const auto collisionInfo = m_rocket.getCollisionInfo();
    if (collisionInfo) {
        auto result = std::find_if(m_particleEffects.begin(), m_particleEffects.end(), [](const auto& effect) -> bool {
            return effect->getEffectType() == ParticleEffect::Type::Planet_Collision;
        });
        // If we don't find an effect for the collision
        // then we'll add it
        if (result == m_particleEffects.end()) {
            // Add particle effect
            m_particleEffects.push_back(std::make_unique<ParticleEffect>(
                ParticleEffect::Type::Planet_Collision, collisionInfo.value().point, collisionInfo.value().normal));
        } else {
            if (!(*result)->isPlaying()) {
                m_rocket.levelStart();
                m_gameLevel.resetLevel();
            }
        }
    }

    // If the player is moving, lets add a particle effect for the exhaust
    // once they stop we'll remove it. Really we should opt for never
    // removing this effect, and only stop it.
    // TODO: Don't remove this effect every time, add a pause mechanism for it
    // instead
    auto exhaustParticleEffect
        = std::find_if(m_particleEffects.begin(), m_particleEffects.end(), [](const auto& effect) -> bool {
              return effect->getEffectType() == ParticleEffect::Type::Rocket_Exhaust;
          });
    if (m_rocket.isPlayerApplyingForce()) {
        if (exhaustParticleEffect == m_particleEffects.end()) {
            m_particleEffects.push_back(std::make_unique<ParticleEffect>(
                ParticleEffect::Type::Rocket_Exhaust, m_rocket.getExhaustPoint(), m_rocket.getExhaustDirection()));
        } else {
            // Update the effect position
            (*exhaustParticleEffect)->setPosition(m_rocket.getExhaustPoint());
            (*exhaustParticleEffect)->setNormal(m_rocket.getExhaustDirection());
        }
    } else {
        if (exhaustParticleEffect != m_particleEffects.end()) {
            if ((*exhaustParticleEffect)->isPlaying())
                (*exhaustParticleEffect)->stop();
        }
    }

    // Remove completed particle effects
    if (!m_particleEffects.empty()) {
        m_particleEffects.erase(
            std::remove_if(m_particleEffects.begin(),
                           m_particleEffects.end(),
                           [](std::unique_ptr<ParticleEffect>& pe) -> bool { return !pe->isPlaying(); }),
            m_particleEffects.end());
    }
}

void PlayState::outOfBoundsUpdate()
{
    if (!m_rocket.isInBounds(m_window) && !m_isOutOfBounds) {
        m_oobTimer.restart();
        m_isOutOfBounds = true;
    } else if (m_rocket.isInBounds(m_window) && m_isOutOfBounds) {
        m_isOutOfBounds = false;
    }

    // If we're out of bounds we need to handle the GUI for the oob timer
    // and reset the level once OOB for too long
    if (m_isOutOfBounds) {
        // Update our ui text
        const auto seconds = static_cast<sf::Int32>(m_oobTimer.getElapsedTime().asSeconds());
        auto remaining = std::max(MAX_OOB_TIME - seconds, 0);
        m_uiOOB.setString(fmt::format("Out of bounds!\nReset in.. {}", remaining));
        CentreTextOrigin(m_uiOOB);
        const auto windowSize = sf::Vector2f { m_window.getSize() };
        m_uiOOB.setPosition(windowSize * 0.5f);

        // Clamp the position of the OOB indicator to the edges of the screen
        // and update its orientation to match the players
        // TODO: animate the oob indicator..
        m_oobDirectionIndicator.setRotation(m_rocket.getRotation());
        const auto rocketPosition = m_rocket.getPosition();

        sf::Vector2f clampedPosition;
        clampedPosition.x
            = std::clamp(rocketPosition.x,
                         m_oobDirectionIndicator.getSize().x / 2.0f,
                         static_cast<float>(m_window.getSize().x) - m_oobDirectionIndicator.getSize().x / 2.0f);

        clampedPosition.y
            = std::clamp(rocketPosition.y,
                         m_oobDirectionIndicator.getSize().y / 2.0f,
                         static_cast<float>(m_window.getSize().y) - m_oobDirectionIndicator.getSize().y / 2.0f);

        m_oobDirectionIndicator.setPosition(clampedPosition);
        if (remaining == 0) {
            m_gameLevel.resetLevel();
            m_rocket.levelStart();
            m_isOutOfBounds = false;
        }
    }
}
