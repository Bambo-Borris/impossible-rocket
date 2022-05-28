#include "App.hpp"
#include "InputHandler.hpp"
#include "PlayState.hpp"

#include <string>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>

constexpr auto WINDOW_TITLE{"Impossible Rocket - [indev]"};

App::App()
	: m_window(sf::VideoMode({800, 600}), WINDOW_TITLE, sf::Style::Default ^ sf::Style::Resize)
{
	spdlog::set_level(spdlog::level::debug);

	m_window.setFramerateLimit(60);
	m_window.setKeyRepeatEnabled(false);
	InputHandler::get(); // construct handler

	m_states.push(std::make_unique<PlayState>(m_window));

	ImGui::SFML::Init(m_window);
}

App::~App()
{
	ImGui::SFML::Shutdown(m_window);

	while (!m_states.empty())
	{
		m_states.pop();
	}
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
			}
			ImGui::SFML::ProcessEvent(m_window, event);
			InputHandler::get().handleEvents(event);
		}

		ImGui::SFML::Update(m_window, deltaTime);
		m_states.top()->update(deltaTime);

		m_window.clear();
		m_states.top()->draw();
		ImGui::SFML::Render(m_window);
		m_window.display();
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