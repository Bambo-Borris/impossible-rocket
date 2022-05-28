#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Time.hpp>
#include <random>

class ParticleEffect : public sf::Drawable
{
public:
	enum class Type
	{
		Planet_Collision,
		Objective_Collected
	};

	ParticleEffect(Type type, sf::Vector2f position, sf::Vector2f normal = sf::Vector2f{});
	void update(const sf::Time &dt);

	auto isPlaying() const -> bool;
	auto getEffectType() const -> Type;

protected:
	virtual void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;

private:
	Type m_effectType;
	sf::Vector2f m_position;
	sf::Vector2f m_normal;
	sf::VertexArray m_vertices;
	std::vector<sf::Vector2f> m_velocities;
	std::size_t m_emittedIndex{0};

	sf::Time m_duration;
	sf::Time m_aliveTime;

	static std::random_device m_randomDevice;
	std::default_random_engine m_randomEngine;
	bool m_isPlaying{true};
};
