#pragma once

#include <SFML/Graphics/Text.hpp>

inline void CentreTextOrigin(sf::Text &text)
{
	const auto localBounds = sf::Vector2f(text.getLocalBounds().left, text.getLocalBounds().top);
	const auto globalOrigin = sf::Vector2f(text.getGlobalBounds().width / 2.0f, text.getGlobalBounds().height / 2.0f);

	text.setOrigin(localBounds + globalOrigin);
}