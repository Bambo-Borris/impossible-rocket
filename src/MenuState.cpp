#include "MenuState.hpp"
#include "AssetHolder.hpp"
#include "InputHandler.hpp"

MenuState::MenuState(sf::RenderWindow &window)
	: BaseState(window),
	  m_mainFont(AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf"))
{
	m_playText.setFont(*m_mainFont);
	m_playText.setString("--> PLAY <--");
}

void MenuState::enter()
{
	const auto localBounds = sf::Vector2f(m_playText.getLocalBounds().left, m_playText.getLocalBounds().top);
	const auto globalOrigin = sf::Vector2f(m_playText.getGlobalBounds().width / 2.0f, m_playText.getGlobalBounds().height / 2.0f);

	m_playText.setOrigin(localBounds + globalOrigin);
	m_playText.setPosition({400, 300});
}

void MenuState::update(const sf::Time &dt)
{
	const auto &ih = InputHandler::get();
	if (m_playText.getGlobalBounds().contains(ih.getMousePosition()))
	{
		m_playText.setFillColor(sf::Color::Yellow);
		if (ih.leftClickPressed())
		{
			m_stateCompleted = true;
		}
	}
	else
		m_playText.setFillColor(sf::Color::White);
}

void MenuState::draw() const
{
	m_window.draw(m_playText);
}
