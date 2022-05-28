#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <optional>

class GameLevel
	: public sf::Drawable
{
public:
	enum class Levels
	{
		Developer = 0,
		One,
		Two,
		Three,
		Four,
		MAX_LEVEL
	};

	GameLevel();

	void loadLevel(Levels level);

	void update(const sf::Time &dt);

	sf::Vector2f getPlayerStart() const { return m_playerStart; }

	sf::Vector2f getSummedForce(const sf::Vector2f &pos, float mass) const;

	std::optional<sf::Vector2f> doesCollideWithPlanet(const sf::Vector2f &pos, float radius) const;

	void handleObjectiveIntersections(const sf::Vector2f &pos, float radius);

	void resetLevel();

	auto isLevelComplete() const -> bool;
	auto getCurrentLevel() const -> Levels;
	auto getAttemptTotal() const -> sf::Uint32;

protected:
	virtual void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;

private:
	struct Planet
	{
		sf::CircleShape shape;
		float mass{0.0f};
	};

	struct Objective
	{
		sf::RectangleShape shape;
		bool isActive{false};
	};

	std::vector<Planet> m_planets;
	std::vector<Objective> m_objectives;

	sf::Vector2f m_playerStart;

	sf::Texture m_objectiveTexture;
	sf::SoundBuffer m_sbCollectObjective;
	sf::Sound m_collectObjective;

	Levels m_currentLevel = Levels::Developer;
	sf::Uint32 m_levelAttempts{1};
};