#include "MenuState.hpp"
#include "AssetHolder.hpp"
#include "InputHandler.hpp"

MenuState::MenuState(sf::RenderWindow &window)
	: BaseState(window)
{
	auto const font = AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf");
	auto const backgroundTex = AssetHolder::get().getTexture("bin/textures/background_resized.png");

	m_playText.setFont(*font);
	m_playText.setString("PLAY");

	m_titleText.setFont(*font);
	m_titleText.setCharacterSize(48);
	m_titleText.setString("IMPOSSIBLE ROCKET!");

	m_backgroundSprite.setSize({m_window.getSize()});
	m_backgroundSprite.setTexture(backgroundTex);

	m_backgroundSprite.setTextureRect({{0, 0}, {600, 400}});
}

void MenuState::enter()
{
	{ // Play Text
		const auto localBounds = sf::Vector2f(m_playText.getLocalBounds().left, m_playText.getLocalBounds().top);
		const auto globalOrigin = sf::Vector2f(m_playText.getGlobalBounds().width / 2.0f, m_playText.getGlobalBounds().height / 2.0f);

		m_playText.setOrigin(localBounds + globalOrigin);
		m_playText.setPosition({400, 300});
	}

	{ // Title Text
		const auto localBounds = sf::Vector2f(m_titleText.getLocalBounds().left, m_titleText.getLocalBounds().top);
		const auto globalOrigin = sf::Vector2f(m_titleText.getGlobalBounds().width / 2.0f, m_titleText.getGlobalBounds().height / 2.0f);

		m_titleText.setOrigin(localBounds + globalOrigin);
		m_titleText.setPosition({400, (m_titleText.getGlobalBounds().height / 2.0f) + 150.0f});
	}
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
	m_window.draw(m_backgroundSprite);
	m_window.draw(m_playText);
	m_window.draw(m_titleText);
}
