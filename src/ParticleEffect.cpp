#include "ParticleEffect.hpp"

#include <cassert>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <spdlog/spdlog.h>
#include <array>

std::random_device ParticleEffect::m_randomDevice;

constexpr std::size_t PLANET_COLLISION_PARTICLE_COUNT{100};
constexpr sf::Vector2f PLANET_COLLISION_PARTICLE_SIZE{8.0f, 8.0f};
constexpr auto PLANET_COLLISION_PARTICLE_MAX_SPEED{200.0f};
constexpr auto PLANET_COLLISION_PARTICLE_MIN_SPEED{120.0f};
constexpr auto PLANET_COLLISION_PARTICLE_COLOUR_START{sf::Color{252, 119, 3, 255}};
constexpr auto PLANET_COLLISION_PARTICLE_COLOUR_END{sf::Color{50, 50, 50, 50}};

constexpr std::array<sf::Vector2f, 4> PLANET_COLLISION_QUAD_OFFSETS = {
	sf::Vector2f(-PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, -PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f),
	sf::Vector2f(PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, -PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f),
	sf::Vector2f(PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f),
	sf::Vector2f(-PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f)};

float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

sf::Color lerp_colour(const sf::Color &start, const sf::Color &end, float t)
{
	const auto start_red = start.r / 255.0f;
	const auto start_green = start.g / 255.0f;
	const auto start_blue = start.b / 255.0f;
	const auto start_alpha = start.a / 255.0f;

	const auto end_red = end.r / 255.0f;
	const auto end_green = end.g / 255.0f;
	const auto end_blue = end.b / 255.0f;
	const auto end_alpha = end.a / 255.0f;

	const auto outputRed = lerp(start_red, end_red, t);
	const auto outputGreen = lerp(start_green, end_green, t);
	const auto outputBlue = lerp(start_blue, end_blue, t);
	const auto outputAlpha = lerp(start_alpha, end_alpha, t);

	return {static_cast<sf::Uint8>(outputRed * 255.0f), static_cast<sf::Uint8>(outputGreen * 255.0f),
			static_cast<sf::Uint8>(outputBlue * 255.0f), static_cast<sf::Uint8>(outputAlpha * 255.0f)};
}

ParticleEffect::ParticleEffect(Type type, sf::Vector2f position, sf::Vector2f normal)
	: m_effectType(type),
	  m_position(position),
	  m_normal(normal),
	  m_randomEngine(m_randomDevice())
{
	// TODO: It's now reached a stage where we are loading assets
	// on a class that may be constructed constantly mid frame
	// we should probably opt to instead load these assets
	// via a different class & allow this class to query
	// that one...
	if (!m_planetCollisionTexture.loadFromFile("bin/textures/explosion.png"))
		throw std::runtime_error("Unable to load planet collision texture for particle effect!");

	switch (m_effectType)
	{
	case Type::Planet_Collision:
	{
		m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
		m_duration = sf::seconds(0.5f);
		m_vertices.resize(PLANET_COLLISION_PARTICLE_COUNT * 6);
		m_particles.resize(PLANET_COLLISION_PARTICLE_COUNT);
		const auto normalAngle = m_normal != sf::Vector2f{} ? m_normal.angle().asDegrees() : 0.0f;
		const auto min = normalAngle - 45.0f;
		const auto max = normalAngle + 45.0f;
		const auto texSize = sf::Vector2f(m_planetCollisionTexture.getSize());
		std::uniform_real_distribution<float> angleDist(min, max);
		std::uniform_real_distribution<float> speedDist(PLANET_COLLISION_PARTICLE_MIN_SPEED, PLANET_COLLISION_PARTICLE_MAX_SPEED);

		for (std::size_t i = 0; i < PLANET_COLLISION_PARTICLE_COUNT; ++i)
		{
			// m_vertices[i].position = position;
			// m_vertices[i].color = PLANET_COLLISION_PARTICLE_COLOUR_START;
			const auto angle = sf::degrees(angleDist(m_randomEngine));
			m_particles[i].velocity = sf::Vector2f{speedDist(m_randomEngine), angle};
			m_particles[i].position = position;

			sf::Vertex *v = &m_vertices[i * 6];
			for (std::size_t j = 0; j < 6; ++j)
			{
				v[j].color = PLANET_COLLISION_PARTICLE_COLOUR_START;
			}

			v[0].position = position + PLANET_COLLISION_QUAD_OFFSETS[0];
			v[0].texCoords = sf::Vector2f{0.0f, 0.0f};

			v[1].position = position + PLANET_COLLISION_QUAD_OFFSETS[1];
			v[1].texCoords = sf::Vector2f{texSize.x, 0.0f};

			v[2].position = position + PLANET_COLLISION_QUAD_OFFSETS[2];
			v[2].texCoords = texSize;

			v[3].position = position + PLANET_COLLISION_QUAD_OFFSETS[2];
			v[3].texCoords = texSize;

			v[4].position = position + PLANET_COLLISION_QUAD_OFFSETS[3];
			v[4].texCoords = sf::Vector2f(0.0f, texSize.y);

			v[5].position = position + PLANET_COLLISION_QUAD_OFFSETS[0];
			v[5].texCoords = sf::Vector2f();
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
	if (m_aliveTime >= m_duration)
	{
		m_isPlaying = false;
		return;
	}
	switch (m_effectType)
	{
	case Type::Planet_Collision:
	{
		const auto t = std::min(m_aliveTime.asSeconds() / m_duration.asSeconds(), 1.0f);
		const auto newColour = lerp_colour(PLANET_COLLISION_PARTICLE_COLOUR_START, PLANET_COLLISION_PARTICLE_COLOUR_END, t);
		for (std::size_t i = 0; i < PLANET_COLLISION_PARTICLE_COUNT; ++i)
		{
			m_particles[i].position += m_particles[i].velocity * dt.asSeconds();

			sf::Vertex *v = &m_vertices[i * 6];
			for (std::size_t j = 0; j < 6; ++j)
				v[j].color = newColour;

			v[0].position = m_particles[i].position + PLANET_COLLISION_QUAD_OFFSETS[0];
			v[1].position = m_particles[i].position + PLANET_COLLISION_QUAD_OFFSETS[1];
			v[2].position = m_particles[i].position + PLANET_COLLISION_QUAD_OFFSETS[2];

			v[3].position = m_particles[i].position + PLANET_COLLISION_QUAD_OFFSETS[2];
			v[4].position = m_particles[i].position + PLANET_COLLISION_QUAD_OFFSETS[3];
			v[5].position = m_particles[i].position + PLANET_COLLISION_QUAD_OFFSETS[0];
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
	auto statesCopy = states;
	statesCopy.texture = &m_planetCollisionTexture;
	target.draw(m_vertices, statesCopy);
}
