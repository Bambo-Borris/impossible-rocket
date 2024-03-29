#include "GameLevel.hpp"
#include "AssetHolder.hpp"
#include "GameplayBlackboard.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <cassert>
#include <fstream>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <string>

std::optional<GameLevel::PlanetCollisionInfo>
circle_vs_circle(const sf::Vector2f& position_a, float radius_a, const sf::Vector2f& position_b, float radius_b)
{
    const float radii_sum_sq = std::pow((radius_a + radius_b), 2.0f);
    sf::Vector2f difference = position_a - position_b;
    const float length_squared = difference.lengthSq();

    if (length_squared > radii_sum_sq)
        return {};

    const auto normal = (position_a - position_b).normalized();
    const auto point = position_b + (radius_b * normal);
    return { { normal, point } };
}

GameLevel::GameLevel()
{
    auto sbCollect { AssetHolder::get().getSoundBuffer("bin/sounds/objective_collect.wav") };
    m_collectObjective.setBuffer(*sbCollect);
}

void GameLevel::loadLevel(Levels level)
{
    std::string levelPath;
    switch (level) {
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
    if (levelFile.fail()) {
        throw std::runtime_error(fmt::format("Unable to load {} level", levelPath));
    }

    m_currentLevel = level;
    m_planets.clear();
    m_objectives.clear();
    auto objectiveTexture { AssetHolder::get().getTexture("bin/textures/objective_ring.png") };

    while (!levelFile.eof()) {
        std::string line;
        levelFile >> line;
        // Skip commented out lines
        if (line == "#") {
            line.resize(250);
            levelFile.getline(line.data(), 250);
            continue;
        }

        // Load start position
        if (line[0] == 's') {
            levelFile >> m_playerStart.x >> m_playerStart.y;
        } else if (line[0] == 'p') // Load planets
        {
            m_planets.emplace_back();
            float radius { 0.0f };
            sf::Vector2f position;

            levelFile >> radius >> position.x >> position.y >> m_planets.back().mass;
            m_planets.back().shape.setSize({ radius * 2.f, radius * 2.f });
            m_planets.back().shape.setOrigin({ radius, radius });
            m_planets.back().shape.setPosition(position);
            auto tex = AssetHolder::get().getTexture("bin/textures/planet.png");
            if (!tex->generateMipmap())
                throw std::runtime_error("Unable to generate mip maps");
                
            m_planets.back().shape.setTexture(tex);

        } else if (line[0] == 'o') // Load objectives
        {
            sf::Vector2f pos;
            levelFile >> pos.x >> pos.y;

            Objective o { sf::RectangleShape { bb::OBJECTIVE_SIZE }, true };

            o.shape.setOrigin(bb::OBJECTIVE_SIZE * 0.5f);
            o.shape.setTexture(objectiveTexture);
            o.shape.setPosition(pos);
            m_objectives.push_back(o);
        }
    }

    levelFile.close();
    m_levelAttempts = 1;
}

void GameLevel::update(const sf::Time& dt)
{
    for (auto& o : m_objectives) {
        if (!o.isActive)
            continue;

        o.shape.rotate(dt.asSeconds() * sf::degrees(bb::OBJECTIVE_ROTATION_SPEED));
    }
}

sf::Vector2f GameLevel::getSummedForce(const sf::Vector2f& pos, float mass) const
{
    sf::Vector2f sum;
    for (auto& p : m_planets) {
        const auto delta = p.shape.getPosition() - pos;
        const float radiusSq = delta.lengthSq();
        const float forceMag = bb::BIG_G * p.mass * mass / radiusSq;

        sum += (delta / std::sqrt(radiusSq)) * forceMag;
    }
    return sum;
}

std::optional<GameLevel::PlanetCollisionInfo> GameLevel::doesCollideWithPlanet(const sf::Vector2f& pos,
                                                                               float radius) const
{
    for (auto& p : m_planets) {
        const auto result
            = circle_vs_circle(pos, radius, p.shape.getPosition(), p.shape.getGlobalBounds().width / 2.0f);
        if (result)
            return result;
    }
    return {};
}

void GameLevel::handleObjectiveIntersections(const sf::Vector2f& pos, float radius)
{
    for (auto& o : m_objectives) {
        if (!o.isActive)
            continue;

        const auto result = circle_vs_circle(pos, radius, o.shape.getPosition(), bb::OBJECTIVE_SIZE.x / 2.0f);
        if (result) {
            if (m_collectObjective.getStatus() != sf::Sound::Status::Playing)
                m_collectObjective.play();

            o.isActive = false;
        }
    }
}

void GameLevel::resetLevel()
{
    for (auto& o : m_objectives) {
        o.isActive = true;
        o.shape.setRotation(sf::degrees(0.0f));
    }
    ++m_levelAttempts;
}

auto GameLevel::isLevelComplete() const -> bool
{
    for (const auto& o : m_objectives)
        if (o.isActive)
            return false;
    return true;
}

auto GameLevel::getCurrentLevel() const -> Levels { return m_currentLevel; }

auto GameLevel::getAttemptTotal() const -> std::uint32_t { return m_levelAttempts; }

void GameLevel::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    for (const auto& p : m_planets) {
        target.draw(p.shape, states);
    }

    for (const auto& o : m_objectives) {
        if (!o.isActive)
            continue;
        target.draw(o.shape, states);
    }
}
