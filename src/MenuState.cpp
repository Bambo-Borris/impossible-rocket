#include "MenuState.hpp"
#include "AssetHolder.hpp"
#include "InputHandler.hpp"
#include "SFUtility.hpp"
#include "GameplayBlackboard.hpp"

MenuState::MenuState(sf::RenderWindow &window)
	: BaseState(window)
{
	auto const font = AssetHolder::get().getFont("bin/fonts/VCR_OSD_MONO_1.001.ttf");
	auto const backgroundTex = AssetHolder::get().getTexture("bin/textures/background_resized.png");
	auto const rocketTexture = AssetHolder::get().getTexture("bin/textures/ship.png");
	m_playText.setFont(*font);
	m_playText.setString("PLAY");

	m_titleText.setFont(*font);
	m_titleText.setCharacterSize(48);
	m_titleText.setString("IMPOSSIBLE ROCKET!");

	m_creditsText.setFont(*font);
	m_creditsText.setCharacterSize(24);
	m_creditsText.setString("Created by Bambo!");

	m_backgroundSprite.setSize({m_window.getSize()});
	m_backgroundSprite.setTexture(backgroundTex);
	m_backgroundSprite.setTextureRect({{0, 0}, {600, 400}});

	m_animationPlanet.setRadius(48.0f);
	m_animationPlanet.setOrigin({48.0f, 48.0f});
	m_animationPlanet.setFillColor({64, 124, 214});
	m_animationPlanet.setPosition(sf::Vector2f{m_window.getSize()} * 0.5f);

	m_animationRocket.setSize(bb::ROCKET_SIZE);
	m_animationRocket.setOrigin(bb::ROCKET_SIZE * 0.5f);
	m_animationRocket.setTexture(rocketTexture);
}

void MenuState::enter()
{
	CentreTextOrigin(m_playText);
	m_playText.setPosition({400, 300});

	CentreTextOrigin(m_titleText);
	m_titleText.setPosition({400, (m_titleText.getGlobalBounds().height / 2.0f) + 150.0f});

	CentreTextOrigin(m_creditsText);
	const auto bounds = m_creditsText.getGlobalBounds();
	m_creditsText.setPosition({(bounds.width / 2.0f), m_window.getSize().y - (bounds.height / 2.0f)});

	m_animationRocket.setPosition(
		m_animationPlanet.getPosition() +
		sf::Vector2f(bb::MENU_ORBIT_RADIUS, m_orbitAngle));

	m_animationRocket.setRotation(sf::degrees(90.0f));
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

	const auto rotateDelta = sf::degrees(bb::MENU_ORBIT_SPEED * dt.asSeconds());
	auto rocketAngle = m_animationRocket.getRotation();
	rocketAngle += rotateDelta;
	rocketAngle = rocketAngle.wrapSigned();
	m_animationRocket.setRotation(rocketAngle);

	m_orbitAngle += rotateDelta;
	m_orbitAngle = m_orbitAngle.wrapSigned();
	m_animationRocket.setPosition(
		m_animationPlanet.getPosition() +
		sf::Vector2f(bb::MENU_ORBIT_RADIUS, m_orbitAngle));
}

void MenuState::draw() const
{
	m_window.draw(m_backgroundSprite);

	m_window.draw(m_animationPlanet);
	m_window.draw(m_animationRocket);

	m_window.draw(m_playText);
	m_window.draw(m_creditsText);
	m_window.draw(m_titleText);
}
