#include "PlayState.hpp"
#include "InputHandler.hpp"
#include "AssetHolder.hpp"

#include <string>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-sfml/imgui-SFML.h>

constexpr auto MAX_OOB_TIME = 5;
constexpr auto FIXED_TIME_STEP = sf::seconds(1.0f / 120.0f);

PlayState::PlayState(sf::RenderWindow &window)
	: BaseState(window),
	  m_rocket(m_physicsWorld, m_gameLevel),
	  m_backgroundTexture(AssetHolder::get().getTexture("bin/textures/background_resized.png")),
	  m_mainFont(AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf"))
{
	m_gameLevel.loadLevel(GameLevel::Levels::One);

	m_backgroundTexture->setRepeated(true);

	m_backgroundSprite.setSize(sf::Vector2f(m_window.getSize()));
	m_backgroundSprite.setTexture(m_backgroundTexture);
	m_backgroundSprite.setTextureRect({{0, 0}, {600, 400}});

	m_uiPadType.setFont(*m_mainFont);
	m_uiPadType.setString("Xbox");
	const auto bounds = m_uiPadType.getGlobalBounds();
	m_uiPadType.setPosition({800.0f - bounds.width, 0.0f});

	m_uiAttempts.setFont(*m_mainFont);
	m_uiAttempts.setString("Attempts: 1");

	m_uiOOB.setFont(*m_mainFont);
}

void PlayState::update(const sf::Time &dt)
{
	// TODO: Reimplement skip through input handler
	auto &input = InputHandler::get();
	const bool skipLevel = input.debugSkipPressed();
	static auto padType = input.getPadType();
	static sf::Uint32 attempts = m_gameLevel.getAttemptTotal();

	// Update core gameplay & ImGui
	m_physicsWorld.step(FIXED_TIME_STEP, dt);
	m_gameLevel.update(dt);
	m_rocket.update(dt);
	
	for (auto &e : m_particleEffects)
	{
		e->update(dt);
	}

	if (input.wasResetPressed())
	{
		m_gameLevel.resetLevel();
		m_rocket.levelStart();
	}

	if (!m_rocket.isInBounds(m_window) && !m_isOutOfBounds)
	{
		m_oobTimer.restart();
		m_isOutOfBounds = true;
	}
	else if (m_rocket.isInBounds(m_window) && m_isOutOfBounds)
	{
		m_isOutOfBounds = false;
	}

	// If we're out of bounds we need to handle the GUI for the oob timer
	// and reset the level once OOB for too long
	if (m_isOutOfBounds)
	{
		const auto seconds = static_cast<sf::Int32>(m_oobTimer.getElapsedTime().asSeconds());
		auto remaining = std::max(MAX_OOB_TIME - seconds, 0);
		m_uiOOB.setString(fmt::format("Out of bounds!\nReset in.. {}", remaining));

		// I hate centring text in sfml because it produces this horrible mess
		// but honestly there's so little text in this game it's not worth
		// creating a utility function just to do this one off bs
		const sf::Vector2f localOffset{m_uiOOB.getLocalBounds().left, m_uiOOB.getLocalBounds().top};
		const sf::Vector2f globalOrigin = {m_uiOOB.getGlobalBounds().width / 2.0f, m_uiOOB.getGlobalBounds().height};
		m_uiOOB.setOrigin(localOffset + globalOrigin);
		const auto windowSize = sf::Vector2f{m_window.getSize()};
		m_uiOOB.setPosition(windowSize * 0.5f);

		if (remaining == 0)
		{
			m_gameLevel.resetLevel();
			m_rocket.levelStart();
			m_isOutOfBounds = false;
		}
	}

	// If the player collides with a planet then
	// we should play a particle effect & then
	// once the effect is complete, we'll reset
	const auto collisionInfo = m_rocket.getCollisionInfo();
	if (collisionInfo)
	{
		auto result = std::find_if(m_particleEffects.begin(), m_particleEffects.end(), [](const auto &effect) -> bool
								   { return effect->getEffectType() == ParticleEffect::Type::Planet_Collision; });
		// If we don't find an effect for the collision
		// then we'll add it
		if (result == m_particleEffects.end())
		{
			// Add particle effect
			m_particleEffects.push_back(std::make_unique<ParticleEffect>(
				ParticleEffect::Type::Planet_Collision,
				collisionInfo.value().point,
				collisionInfo.value().normal));
		}
		else
		{
			if (!(*result)->isPlaying())
			{
				m_rocket.levelStart();
				m_gameLevel.resetLevel();
				m_particleEffects.erase(
					std::remove(m_particleEffects.begin(),
								m_particleEffects.end(),
								(*result)));
			}
		}
	}

	// Roll over to next level
	if (m_gameLevel.isLevelComplete() || skipLevel)
	{
		const auto current = static_cast<sf::Uint32>(m_gameLevel.getCurrentLevel());
		if (current + 1 >= static_cast<sf::Uint32>(GameLevel::Levels::MAX_LEVEL))
		{
			// Do game completion here.
			spdlog::debug("All Levels Complete");
		}
		else
		{
			m_gameLevel.loadLevel(static_cast<GameLevel::Levels>(current + 1));
			m_rocket.levelStart();
		}
	}

	// Update pad type UI
	if (input.getPadType() != padType)
	{
		padType = input.getPadType();
		switch (padType)
		{
		case InputHandler::PadType::Xbox_Pad:
			m_uiPadType.setString("Xbox");
			break;

		case InputHandler::PadType::DS4_Pad:
			m_uiPadType.setString("DS4");
			break;
		default:
			break;
		}
		const auto bounds = m_uiPadType.getGlobalBounds();
		m_uiPadType.setPosition({800 - bounds.width, 0.0f});
	}

	if (attempts != m_gameLevel.getAttemptTotal())
	{
		attempts = m_gameLevel.getAttemptTotal();
		m_uiAttempts.setString(fmt::format("Attempts: {}", attempts));
	}
}

void PlayState::enter()
{
	m_rocket.levelStart();
}

void PlayState::draw() const
{
	m_window.draw(m_backgroundSprite);
	m_window.draw(m_gameLevel);
	m_window.draw(m_rocket);
	for (const auto &pe : m_particleEffects)
	{
		m_window.draw(*pe);
	}

	m_window.draw(m_uiPadType);
	m_window.draw(m_uiAttempts);
	if (m_isOutOfBounds)
	{
		m_window.draw(m_uiOOB);
	}
}
