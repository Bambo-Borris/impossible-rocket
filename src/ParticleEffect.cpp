#include "ParticleEffect.hpp"

#include <cassert>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <spdlog/spdlog.h>

ParticleEffect::ParticleEffect(Type type, sf::Vector2f position, sf::Vector2f normal)
	: m_effectType(type),
	  m_position(position),
	  m_normal(normal)
{
	m_vertices.setPrimitiveType(sf::PrimitiveType::Points);

	switch (m_effectType)
	{
	case Type::Planet_Collision:
		m_duration = sf::seconds(0.5f);
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
