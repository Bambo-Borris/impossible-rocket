#include "GameLevel.hpp"
#include "AssetHolder.hpp"

#include <string>
#include <fstream>
#include <cassert>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <SFML/Graphics/RenderTarget.hpp>

constexpr auto BIG_G{6.67e-11f};
constexpr auto OBJECTIVE_ROTATION_SPEED{50.0f};
const sf::Vector2f OBJECTIVE_SIZE{24.0f, 24.0f};

std::optional<GameLevel::PlanetCollisionInfo> circle_vs_circle(const sf::Vector2f &position_a, float radius_a, const sf::Vector2f &position_b, float radius_b)
{
	const float radii_sum_sq = std::pow((radius_a + radius_b), 2.0f);
	sf::Vector2f difference = position_a - position_b;
	const float length_squared = difference.lengthSq();

	if (length_squared > radii_sum_sq)
		return {};

	const auto normal = (position_a - position_b).normalized();
	const auto point = position_b + (radius_b * normal);
	return {{normal, point}};
}

GameLevel::GameLevel()
	: m_objectiveTexture(AssetHolder::get().getTexture("bin/textures/objective_ring.png")),
	  m_sbCollectObjective(AssetHolder::get().getSoundBuffer("bin/sounds/objective_collect.wav"))
{
	m_collectObjective.setBuffer(*m_sbCollectObjective);
}

void GameLevel::loadLevel(Levels level)
{
	std::string levelPath;
	switch (level)
	{
	case GameLevel::Levels::Developer:
		levelPath = "bin/levels/dev_level.txt";
		break;
	case GameLevel::Levels::One:
		levelPath = "bin/levels/level_1.txt";
		break;
	case GameLevel::Levels::Two:
		levelPath = "bin/levels/level_2.txt";
		break;
	case GameLevel::Levels::Three:
		levelPath = "bin/levels/level_3.txt";
		break;
	case GameLevel::Levels::Four:
		levelPath = "bin/levels/level_4.txt";
		break;
	case GameLevel::Levels::Five:
		levelPath = "bin/levels/level_5.txt";
		break;
	case GameLevel::Levels::Six:
		levelPath = "bin/levels/level_6.txt";
		break;
	default:
		assert(false);
		break;
	}

	std::ifstream levelFile(levelPath, std::ios::in);
	if (levelFile.fail())
	{
		throw std::runtime_error(fmt::format("Unable to load {} level", levelPath));
	}

	m_currentLevel = level;
	m_planets.clear();
	m_objectives.clear();

	while (!levelFile.eof())
	{
		std::string line;
		levelFile >> line;
		// Skip commented out lines
		if (line == "#")
		{
			line.resize(250);
			levelFile.getline(line.data(), 250);
			continue;
		}

		// Load start position
		if (line[0] == 's')
		{
			levelFile >> m_playerStart.x >> m_playerStart.y;
		}
		else if (line[0] == 'p') // Load planets
		{
			m_planets.emplace_back();
			float radius{0.0f};
			sf::Vector2f position;

			levelFile >> radius >> position.x >> position.y >> m_planets.back().mass;
			m_planets.back().shape.setRadius(radius);
			m_planets.back().shape.setOrigin({radius, radius});
			m_planets.back().shape.setPosition(position);
			m_planets.back().shape.setFillColor(sf::Color::Green);
		}
		else if (line[0] == 'o') // Load objectives
		{
			sf::Vector2f pos;
			levelFile >> pos.x >> pos.y;

			Objective o{sf::RectangleShape{OBJECTIVE_SIZE}, true};

			o.shape.setOrigin(OBJECTIVE_SIZE * 0.5f);
			o.shape.setTexture(m_objectiveTexture);
			o.shape.setPosition(pos);
			m_objectives.push_back(o);
		}
	}

	levelFile.close();
	m_levelAttempts = 1;
}

void GameLevel::update(const sf::Time &dt)
{
	for (auto &o : m_objectives)
	{
		if (!o.isActive)
			continue;

		o.shape.rotate(dt.asSeconds() * sf::degrees(OBJECTIVE_ROTATION_SPEED));
	}
}

sf::Vector2f GameLevel::getSummedForce(const sf::Vector2f &pos, float mass) const
{
	sf::Vector2f sum;
	for (auto &p : m_planets)
	{
		const auto delta = p.shape.getPosition() - pos;
		const float radiusSq = delta.lengthSq();
		const float forceMag = BIG_G * p.mass * mass / radiusSq;

		sum += (delta / std::sqrt(radiusSq)) * forceMag;
	}
	return sum;
}

std::optional<GameLevel::PlanetCollisionInfo> GameLevel::doesCollideWithPlanet(const sf::Vector2f &pos, float radius) const
{
	for (auto &p : m_planets)
	{
		const auto result = circle_vs_circle(pos, radius, p.shape.getPosition(), p.shape.getRadius());
		if (result)
			return result;
	}
	return {};
}

void GameLevel::handleObjectiveIntersections(const sf::Vector2f &pos, float radius)
{
	for (auto &o : m_objectives)
	{
		if (!o.isActive)
			continue;

		const auto result = circle_vs_circle(pos, radius, o.shape.getPosition(), OBJECTIVE_SIZE.x / 2.0f);
		if (result)
		{
			if (m_collectObjective.getStatus() != sf::Sound::Status::Playing)
				m_collectObjective.play();

			o.isActive = false;
		}
	}
}

void GameLevel::resetLevel()
{
	for (auto &o : m_objectives)
	{
		o.isActive = true;
		o.shape.setRotation(sf::degrees(0.0f));
	}
	++m_levelAttempts;
}

auto GameLevel::isLevelComplete() const -> bool
{
	for (const auto &o : m_objectives)
		if (o.isActive)
			return false;
	return true;
}

auto GameLevel::getCurrentLevel() const -> Levels
{
	return m_currentLevel;
}

auto GameLevel::getAttemptTotal() const -> sf::Uint32
{
	return m_levelAttempts;
}

void GameLevel::draw(sf::RenderTarget &target, const sf::RenderStates &states) const
{
	for (const auto &p : m_planets)
	{
		target.draw(p.shape, states);
	}

	for (const auto &o : m_objectives)
	{
		if (!o.isActive)
			continue;
		target.draw(o.shape, states);
	}
}
