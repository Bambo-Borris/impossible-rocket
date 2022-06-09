#pragma once

#include "PhysicsWorld.hpp"
#include "PlayerRocket.hpp"
#include "GameLevel.hpp"
#include "BaseState.hpp"
#include "ParticleEffect.hpp"

#include <memory>

class PlayState
	: public BaseState
{
public:
	PlayState(sf::RenderWindow &window);
	~PlayState() = default;

	virtual void enter() override;
	virtual void update(const sf::Time &dt) override;
	virtual void draw() const override;

private:
	enum class Status
	{
		Playing,
		Paused,
		End_Of_Level
	};

	void updatePlaying(const sf::Time &dt);
	void updatePaused(const sf::Time &dt);
	void updateEndOfLevel(const sf::Time &dt);
	void particleEffectUpdate();
	void outOfBoundsUpdate();

	PhysicsWorld m_physicsWorld;
	GameLevel m_gameLevel;
	PlayerRocket m_rocket;

	sf::RectangleShape m_backgroundSprite;
	sf::RectangleShape m_oobDirectionIndicator;
	sf::RectangleShape m_pauseMenuDim;

	sf::Text m_uiPadType;
	sf::Text m_uiAttempts;
	sf::Text m_uiOOB;
	sf::Text m_uiPauseTitle;
	sf::Text m_uiResumeButton;
	sf::Text m_uiOptionsButton;
	sf::Text m_uiQuitButton;
	sf::Clock m_oobTimer; // out of bounds timer
	sf::Sound m_pauseButtonHoverSfx;

	std::vector<std::unique_ptr<ParticleEffect>> m_particleEffects;
	bool m_isOutOfBounds{false};
	PlayState::Status m_status{PlayState::Status::Playing};
};
