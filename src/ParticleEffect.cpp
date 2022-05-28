#include "ParticleEffect.hpp"

#include <cassert>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <spdlog/spdlog.h>

std::random_device ParticleEffect::m_randomDevice;

constexpr std::size_t PLANET_COLLISION_PARTICLE_COUNT{300};
constexpr auto PLANET_COLLISION_PARTICLE_MAX_SPEED{200.0f};
constexpr auto PLANET_COLLISION_PARTICLE_MIN_SPEED{120.0f};

ParticleEffect::ParticleEffect(Type type, sf::Vector2f position, sf::Vector2f normal)
	: m_effectType(type),
	  m_position(position),
	  m_normal(normal),
	  m_randomEngine(m_randomDevice())
{
	m_vertices.setPrimitiveType(sf::PrimitiveType::Points);

	switch (m_effectType)
	{
	case Type::Planet_Collision:
	{
		m_duration = sf::seconds(0.5f);
		m_vertices.resize(PLANET_COLLISION_PARTICLE_COUNT);
		m_velocities.resize(PLANET_COLLISION_PARTICLE_COUNT);
		const auto normalAngle = m_normal != sf::Vector2f{} ? m_normal.angle().asDegrees() : 0.0f;
		const auto min = normalAngle - 45.0f;
		const auto max = normalAngle + 45.0f;
		std::uniform_real_distribution<float> angleDist(min, max);
		std::uniform_real_distribution<float> speedDist(PLANET_COLLISION_PARTICLE_MIN_SPEED, PLANET_COLLISION_PARTICLE_MAX_SPEED);

		for (std::size_t i = 0; i < PLANET_COLLISION_PARTICLE_COUNT; ++i)
		{
			m_vertices[i].position = position;
			const auto angle = sf::degrees(angleDist(m_randomEngine));
			m_velocities[i] = sf::Vector2f{speedDist(m_randomEngine), angle};
		}
	}
	break;
	case Type::Objective_Collected:
		break;
	default:
		assert(false);
		break;
	}
}

void ParticleEffect::update(const sf::Time &dt)
{
	m_aliveTime += dt;
	if (m_aliveTime > m_duration)
	{
		spdlog::debug("Effect finished playing");
		m_isPlaying = false;
		return;
	}
	switch (m_effectType)
	{
	case Type::Planet_Collision:
	{
		for (std::size_t i = 0; i < PLANET_COLLISION_PARTICLE_COUNT; ++i)
		{
			m_vertices[i].position += m_velocities[i] * dt.asSeconds();
		}
	}
	break;
	default:
		assert(false);
		break;
	}
}

auto ParticleEffect::isPlaying() const -> bool
{
	return m_isPlaying;
}

auto ParticleEffect::getEffectType() const -> Type
{
	return m_effectType;
}

void ParticleEffect::draw(sf::RenderTarget &target, const sf::RenderStates &states) const
{
	target.draw(m_vertices, states);
}
