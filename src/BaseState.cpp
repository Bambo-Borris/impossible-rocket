#include "BaseState.hpp"
#include <cassert>

BaseState::BaseState(sf::RenderWindow &window)
	: m_window(window)
{
}

auto BaseState::isStateCompleted() const -> bool
{
	return m_stateCompleted;
}