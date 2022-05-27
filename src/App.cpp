#include "App.hpp"
#include "InputHandler.h"

#include <string>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>

constexpr auto WINDOW_TITLE{"Impossible Rocket - [indev]"};
const sf::Time FIXED_TIME_STEP = sf::seconds(1.0f / 120.0f);
constexpr auto MAX_OOB_TIME = 5;

App::App()
	: m_window(sf::VideoMode({800, 600}), WINDOW_TITLE, sf::Style::Default ^ sf::Style::Resize),
	  m_rocket(m_physicsWorld, m_gameLevel)
{
	spdlog::set_level(spdlog::level::debug);

	m_window.setFramerateLimit(60);
	m_window.setKeyRepeatEnabled(false);
	InputHandler::get(); // construct handler

	m_gameLevel.loadLevel(GameLevel::Levels::One);
	m_rocket.levelStart();

	if (!m_backgroundTexture.loadFromFile("bin/textures/background_resized.png"))
		throw std::runtime_error("Unable to load background texture");

	m_backgroundTexture.setRepeated(true);

	m_backgroundSprite.setSize({m_window.getSize()});
	m_backgroundSprite.setTexture(&m_backgroundTexture);
	m_backgroundSprite.setTextureRect({{0, 0}, {600, 400}});

	if (!m_mainFont.loadFromFile("bin/fonts/VCR_OSD_MONO_1.001.ttf"))
		throw std::runtime_error("Unable to load default font");

	m_uiPadType.setFont(m_mainFont);
	m_uiPadType.setString("Xbox");
	const auto bounds = m_uiPadType.getGlobalBounds();
	m_uiPadType.setPosition({800.0f - bounds.width, 0.0f});

	m_uiAttempts.setFont(m_mainFont);
	m_uiAttempts.setString("Attempts: 1");

	m_uiOOB.setFont(m_mainFont);

	ImGui::SFML::Init(m_window);
}

App::~App()
{
	ImGui::SFML::Shutdown(m_window);
}

void App::run()
{
	sf::Clock loopClock;

	while (m_window.isOpen())
	{
		bool skipLevel = false;
		auto deltaTime = loopClock.restart();
		if (deltaTime > sf::seconds(0.25f))
		{
			deltaTime = sf::seconds(0.25f);
		}

		logFPS(deltaTime);

		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				m_window.close();
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Escape)
					m_window.close();

#ifdef _DEBUG
				if (event.key.code == sf::Keyboard::N)
				{
					skipLevel = true;
				}
#endif
			}
			ImGui::SFML::ProcessEvent(m_window, event);
			InputHandler::get().handleEvents(event);
		}

		updateGame(deltaTime, skipLevel);

		drawGame();
	}
}

void App::logFPS(const sf::Time &dt)
{
	static int counter = 0;
	static auto sum = sf::Time::Zero;

	if (counter < 50)
	{
		sum += dt;
		++counter;
	}
	else
	{
		auto fps = 1.0f / (sum.asSeconds() / static_cast<float>(counter));
		const auto newTitle = fmt::format("{} - FPS {}", WINDOW_TITLE, static_cast<sf::Uint32>(fps));
		m_window.setTitle(newTitle);
		sum = sf::Time::Zero;
		counter = 0;
	}
}

void App::updateGame(const sf::Time &dt, bool skipLevel)
{
	auto &input = InputHandler::get();
	static auto padType = input.getPadType();
	static sf::Uint32 attempts = m_gameLevel.getAttemptTotal();

	// Update core gameplay & ImGui
	ImGui::SFML::Update(m_window, dt);
	m_physicsWorld.step(FIXED_TIME_STEP, dt);
	m_gameLevel.update(dt);
	m_rocket.update(dt);

	if (input.wasResetPressed())
	{
		m_gameLevel.resetLevel();
		m_rocket.levelStart();
		input.resetConsumed();
	}

	if (!m_rocket.isInBounds(m_window) && !m_isOutOfBounds)
	{
		m_oobTimer.restart();
		m_isOutOfBounds = true;
	}
	else if (m_rocket.isInBounds(m_window) && m_isOutOfBounds)
	{
		m_isOutOfBounds = false;
	}

	if (m_isOutOfBounds)
	{
		const auto seconds = static_cast<sf::Int32>(m_oobTimer.getElapsedTime().asSeconds());
		auto remaining = std::max(MAX_OOB_TIME - seconds, 0);
		m_uiOOB.setString(fmt::format("Out of bounds!\nReset in.. {}", remaining));

		// I hate centring text in sfml because it produces this horrible mess
		// but honestly there's so little text in this game it's not worth
		// creating a utility function just to do this one off bs
		const sf::Vector2f localOffset{m_uiOOB.getLocalBounds().left, m_uiOOB.getLocalBounds().top};
		const sf::Vector2f globalOrigin = {m_uiOOB.getGlobalBounds().width / 2.0f, m_uiOOB.getGlobalBounds().height};
		m_uiOOB.setOrigin(localOffset + globalOrigin);
		const auto windowSize = sf::Vector2f{m_window.getSize()};
		m_uiOOB.setPosition(windowSize * 0.5f);

		if (remaining == 0)
		{
			m_gameLevel.resetLevel();
			m_rocket.levelStart();
			m_isOutOfBounds = false;
		}
	}

	// Roll over to next level
	if (m_gameLevel.isLevelComplete() || skipLevel)
	{
		const auto current = static_cast<sf::Uint32>(m_gameLevel.getCurrentLevel());
		if (current + 1 >= static_cast<sf::Uint32>(GameLevel::Levels::MAX_LEVEL))
		{
			// Do game completion here.
			spdlog::debug("All Levels Complete");
		}
		else
		{
			m_gameLevel.loadLevel(static_cast<GameLevel::Levels>(current + 1));
			m_rocket.levelStart();
			skipLevel = false;
		}
	}

	// Update pad type UI
	if (input.getPadType() != padType)
	{
		padType = input.getPadType();
		switch (padType)
		{
		case InputHandler::PadType::Xbox_Pad:
			m_uiPadType.setString("Xbox");
			break;

		case InputHandler::PadType::DS4_Pad:
			m_uiPadType.setString("DS4");
			break;
		default:
			break;
		}
		const auto bounds = m_uiPadType.getGlobalBounds();
		m_uiPadType.setPosition({800 - bounds.width, 0.0f});
	}

	if (attempts != m_gameLevel.getAttemptTotal())
	{
		attempts = m_gameLevel.getAttemptTotal();
		m_uiAttempts.setString(fmt::format("Attempts: {}", attempts));
	}
}

void App::drawGame()
{
	m_window.clear();
	m_window.draw(m_backgroundSprite);
	m_window.draw(m_gameLevel);
	m_window.draw(m_rocket);
	m_window.draw(m_uiPadType);
	m_window.draw(m_uiAttempts);
	if (m_isOutOfBounds)
	{
		m_window.draw(m_uiOOB);
	}
	ImGui::SFML::Render(m_window);
	m_window.display();
}
