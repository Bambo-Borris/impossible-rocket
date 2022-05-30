#pragma once

#include "BaseState.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class MenuState : public BaseState
{
public:
	MenuState(sf::RenderWindow &window);

	virtual void enter() override;
	virtual void update(const sf::Time &dt) override;
	virtual void draw() const override;

private:
	sf::Text m_playText;
	sf::Text m_titleText;
	sf::Text m_creditsText; 
	sf::RectangleShape m_backgroundSprite;

	sf::CircleShape m_animationPlanet;
	sf::RectangleShape m_animationRocket; 
	sf::Angle m_orbitAngle;
};
