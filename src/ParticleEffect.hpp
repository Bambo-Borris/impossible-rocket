#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <random>

class ParticleEffect : public sf::Drawable
{
public:
	enum class Type
	{
		Planet_Collision,
		Objective_Collected, // TODO: Implement this effect
		Rocket_Exhaust
	};

	ParticleEffect(Type type, sf::Vector2f position, sf::Vector2f normal = sf::Vector2f{});
	void update(const sf::Time &dt);

	auto isPlaying() const -> bool;
	auto getEffectType() const -> Type;

	void stop();
	void setPosition(const sf::Vector2f &position);
	void setNormal(const sf::Vector2f &normal);

protected:
	virtual void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;

private:
	struct Particle
	{
		sf::Vector2f position;
		sf::Vector2f velocity;
		sf::Time particleLifetime;
		bool active{false};
	};

	Type m_effectType;
	sf::Vector2f m_position;
	sf::Vector2f m_normal;
	sf::VertexArray m_vertices;
	std::vector<Particle> m_particles;
	// std::size_t m_emittedIndex{0};

	sf::Time m_duration; // Emitter lifetime
	sf::Time m_aliveTime;
	sf::Time m_emitTimer;

	sf::Texture *m_planetCollisionTexture;

	static std::random_device m_randomDevice;
	std::default_random_engine m_randomEngine;
	bool m_isPlaying{true};
};
