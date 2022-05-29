#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class BaseState
{
public:
	BaseState(sf::RenderWindow &window);
	virtual ~BaseState() = default;

	virtual void enter() = 0;
	virtual void update(const sf::Time &dt) = 0;
	virtual void draw() const = 0;

	auto isStateCompleted() -> bool const;

protected:
	sf::RenderWindow &m_window;
	bool m_stateCompleted{false};
};
