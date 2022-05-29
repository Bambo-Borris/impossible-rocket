#include "BaseState.hpp"
#include <cassert>

BaseState::BaseState(sf::RenderWindow &window)
	: m_window(window)
{
}

auto BaseState::isStateCompleted() -> bool const
{
	return m_stateCompleted;
}