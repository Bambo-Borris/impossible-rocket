#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Rect.hpp>

inline void CentreTextOrigin(sf::Text &text)
{
	const auto localBounds = sf::Vector2f(text.getLocalBounds().left, text.getLocalBounds().top);
	const auto globalOrigin = sf::Vector2f(text.getGlobalBounds().width / 2.0f, text.getGlobalBounds().height / 2.0f);

	text.setOrigin(localBounds + globalOrigin);
}

inline sf::Vector2f GetHalfBounds(const sf::FloatRect &rect)
{
	return rect.getSize() * 0.5f;
}