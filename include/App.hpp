#pragma once

#include <SFML/Graphics.hpp>

#include "PhysicsWorld.hpp"
#include "PlayerRocket.hpp"
#include "GameLevel.hpp"

class App
{
public:
	App();
	~App();

	void run();

private:
	void logFPS(const sf::Time &dt);
	void updateGame(const sf::Time &dt, bool skipLevel);
	void drawGame();

	sf::RenderWindow m_window;
	PhysicsWorld m_physicsWorld;
	GameLevel m_gameLevel;
	PlayerRocket m_rocket;
	sf::Font m_mainFont;
	sf::Text m_uiPadType;
	sf::Text m_uiAttempts;
	sf::Text m_uiOOB;
	sf::Clock m_oobTimer; // out of bounds timer
	sf::Texture m_backgroundTexture; 
	sf::RectangleShape m_backgroundSprite; 

	bool m_isOutOfBounds{false};
};
