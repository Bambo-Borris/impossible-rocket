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

	virtual void enter();
	virtual void update(const sf::Time &dt) override;
	virtual void draw() const override;

private:
	PhysicsWorld m_physicsWorld;
	GameLevel m_gameLevel;
	PlayerRocket m_rocket;

	sf::Texture *m_backgroundTexture; // TODO: remove and swap to local instance which is passed to .setTexture() call
	sf::RectangleShape m_backgroundSprite;
	sf::Font *m_mainFont;
	sf::Text m_uiPadType;
	sf::Text m_uiAttempts;
	sf::Text m_uiOOB;
	sf::Clock m_oobTimer; // out of bounds timer

	std::vector<std::unique_ptr<ParticleEffect>> m_particleEffects;
	bool m_isOutOfBounds{false};
};
