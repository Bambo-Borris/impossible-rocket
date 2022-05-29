#pragma once

#include "BaseState.hpp"

#include <SFML/Graphics/Text.hpp> 
#include <SFML/Graphics/Font.hpp> 

class MenuState : public BaseState
{
public:
	MenuState(sf::RenderWindow &window);

	virtual void enter() override;
	virtual void update(const sf::Time &dt) override;
	virtual void draw() const override;

private:
	sf::Text m_playText; 
	sf::Font* m_mainFont;
};
