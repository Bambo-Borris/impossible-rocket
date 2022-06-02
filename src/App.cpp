#include "App.hpp"
#include "InputHandler.hpp"
#include "PlayState.hpp"
#include "MenuState.hpp"
#include "AssetHolder.hpp"

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <imgui-SFML.h>
#include <imgui.h>

constexpr auto WINDOW_TITLE{"Impossible Rocket - [indev]"};

App::App()
{
	sf::ContextSettings ctxt;
	ctxt.antialiasingLevel = 16;
	m_window.create(sf::VideoMode({800, 600}), WINDOW_TITLE, sf::Style::Default ^ sf::Style::Resize, ctxt);
	spdlog::set_level(spdlog::level::debug);
	m_window.setFramerateLimit(60);
	m_window.setKeyRepeatEnabled(false);

	// Sigleton creation;
	InputHandler::get();
	AssetHolder::get();

	m_states.push(std::make_unique<PlayState>(m_window));
	m_states.push(std::make_unique<MenuState>(m_window));

	if (!ImGui::SFML::Init(m_window))
		throw std::runtime_error("Unable to initialise ImGui SFML");
}

App::~App()
{
	ImGui::SFML::Shutdown(m_window);

	while (!m_states.empty())
	{
		m_states.pop();
	}

	delete (&InputHandler::get());
	delete (&AssetHolder::get());
}

void App::run()
{
	sf::Clock loopClock;

	m_states.top()->enter();

	while (m_window.isOpen())
	{
		auto deltaTime = loopClock.restart();
		if (deltaTime > sf::seconds(0.25f))
		{
			deltaTime = sf::seconds(0.25f);
		}

		logFPS(deltaTime);

		InputHandler::get().handleEvents(m_window);
		ImGui::SFML::Update(m_window, deltaTime);

		if (m_states.top()->isStateCompleted())
		{
			m_states.pop();
			assert(!m_states.empty());
			m_states.top()->enter();
		}

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

	if (counter < 10)
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