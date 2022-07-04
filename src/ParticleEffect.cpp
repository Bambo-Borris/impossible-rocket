#include "ParticleEffect.hpp"
#include "AssetHolder.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <array>
#include <cassert>
#include <spdlog/spdlog.h>

std::random_device ParticleEffect::m_randomDevice;

// Planet Collision Explosion
constexpr std::size_t PLANET_COLLISION_PARTICLE_COUNT { 100 };
constexpr sf::Vector2f PLANET_COLLISION_PARTICLE_SIZE { 8.0f, 8.0f };
constexpr auto PLANET_COLLISION_PARTICLE_MAX_SPEED { 200.0f };
constexpr auto PLANET_COLLISION_PARTICLE_MIN_SPEED { 120.0f };
constexpr auto PLANET_COLLISION_PARTICLE_COLOUR_START { sf::Color { 252, 119, 3, 255 } };
constexpr auto PLANET_COLLISION_PARTICLE_COLOUR_END { sf::Color { 50, 50, 50, 50 } };

constexpr std::array<sf::Vector2f, 4> PLANET_COLLISION_QUAD_OFFSETS
    = { sf::Vector2f(-PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, -PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f),
        sf::Vector2f(PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, -PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f),
        sf::Vector2f(PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f),
        sf::Vector2f(-PLANET_COLLISION_PARTICLE_SIZE.x / 2.0f, PLANET_COLLISION_PARTICLE_SIZE.y / 2.0f) };

// Objective Collected Burst

// Rocket Exhaust
constexpr auto ROCKET_EXHAUST_PARTICLE_MIN_SPEED { 200.0f };
constexpr auto ROCKET_EXHAUST_PARTICLE_MAX_SPEED { 250.0f };
constexpr sf::Vector2f ROCKET_EXHAUST_PARTICLE_SIZE { 4.0f, 4.0f };
constexpr auto ROCKET_EXHAUST_PARTICLE_COLOUR_START { sf::Color { 255, 255, 255, 255 } };
constexpr auto ROCKET_EXHAUST_PARTICLE_COLOUR_END { sf::Color { 66, 135, 245, 50 } };
constexpr auto ROCKET_EXHAUST_EMIT_FREQUENCY { sf::milliseconds(1) };
constexpr auto ROCKET_EXHAUST_PARTICLE_LIFETIME { sf::seconds(0.25f) };
constexpr auto ROCKET_EXHAUST_PARTICLE_COUNT { 500 };

constexpr std::array<sf::Vector2f, 4> ROCKET_EXHAUST_QUAD_OFFSETS
    = { sf::Vector2f(-ROCKET_EXHAUST_PARTICLE_SIZE.x / 2.0f, -ROCKET_EXHAUST_PARTICLE_SIZE.y / 2.0f),
        sf::Vector2f(ROCKET_EXHAUST_PARTICLE_SIZE.x / 2.0f, -ROCKET_EXHAUST_PARTICLE_SIZE.y / 2.0f),
        sf::Vector2f(ROCKET_EXHAUST_PARTICLE_SIZE.x / 2.0f, ROCKET_EXHAUST_PARTICLE_SIZE.y / 2.0f),
        sf::Vector2f(-ROCKET_EXHAUST_PARTICLE_SIZE.x / 2.0f, ROCKET_EXHAUST_PARTICLE_SIZE.y / 2.0f) };

float lerp(float v0, float v1, float t) { return (1 - t) * v0 + t * v1; }

sf::Color lerp_colour(const sf::Color& start, const sf::Color& end, float t)
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

    return { static_cast<sf::Uint8>(outputRed * 255.0f),
             static_cast<sf::Uint8>(outputGreen * 255.0f),
             static_cast<sf::Uint8>(outputBlue * 255.0f),
             static_cast<sf::Uint8>(outputAlpha * 255.0f) };
}

ParticleEffect::ParticleEffect(Type type, sf::Vector2f position, sf::Vector2f normal)
    : m_effectType(type)
    , m_position(position)
    , m_normal(normal)
    , m_planetCollisionTexture(AssetHolder::get().getTexture("bin/textures/explosion.png"))
    , m_randomEngine(m_randomDevice())
{
    switch (m_effectType) {
    case Type::Planet_Collision: {
        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_duration = sf::seconds(0.5f);
        m_vertices.resize(PLANET_COLLISION_PARTICLE_COUNT * 6);
        m_particles.resize(PLANET_COLLISION_PARTICLE_COUNT);
        const auto normalAngle = m_normal != sf::Vector2f {} ? m_normal.angle().asDegrees() : 0.0f;
        const auto min = normalAngle - 45.0f;
        const auto max = normalAngle + 45.0f;
        const auto texSize = sf::Vector2f(m_planetCollisionTexture->getSize());
        std::uniform_real_distribution<float> angleDist(min, max);
        std::uniform_real_distribution<float> speedDist(PLANET_COLLISION_PARTICLE_MIN_SPEED,
                                                        PLANET_COLLISION_PARTICLE_MAX_SPEED);

        for (std::size_t i = 0; i < PLANET_COLLISION_PARTICLE_COUNT; ++i) {
            const auto angle = sf::degrees(angleDist(m_randomEngine));
            m_particles[i].velocity = sf::Vector2f { speedDist(m_randomEngine), angle };
            m_particles[i].position = position;
            m_particles[i].active = true;

            sf::Vertex* v = &m_vertices[i * 6];
            for (std::size_t j = 0; j < 6; ++j) {
                v[j].color = PLANET_COLLISION_PARTICLE_COLOUR_START;
            }

            updateQuadPosition(v, i);

            v[0].texCoords = sf::Vector2f { 0.0f, 0.0f };
            v[1].texCoords = sf::Vector2f { texSize.x, 0.0f };
            v[2].texCoords = texSize;
            v[3].texCoords = texSize;
            v[4].texCoords = sf::Vector2f(0.0f, texSize.y);
            v[5].texCoords = sf::Vector2f();
        }
    } break;
    case Type::Objective_Collected:
        break;
    case Type::Rocket_Exhaust: {
        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_particles.resize(ROCKET_EXHAUST_PARTICLE_COUNT);
    } break;
    default:
        assert(false);
        break;
    }
}

void ParticleEffect::update(const sf::Time& dt)
{
    m_aliveTime += dt;
    // Rocket exhaust is an infinite effect
    // so we don't care about stopping when alive time is > than some value
    if (m_aliveTime >= m_duration && m_effectType != Type::Rocket_Exhaust) {
        m_isPlaying = false;
        return;
    }

    switch (m_effectType) {
    case Type::Planet_Collision: {
        const auto t = std::min(m_aliveTime.asSeconds() / m_duration.asSeconds(), 1.0f);
        for (std::size_t i = 0; i < PLANET_COLLISION_PARTICLE_COUNT; ++i) {
            m_particles[i].position += m_particles[i].velocity * dt.asSeconds();
            sf::Vertex* v = &m_vertices[i * 6];
            lerpQuadColours(v, t);
            updateQuadPosition(v, i);
        }
    } break;
    case Type::Rocket_Exhaust: {
        sf::Uint32 emitCount = 0;
        m_emitTimer += dt;
        if (m_emitTimer >= ROCKET_EXHAUST_EMIT_FREQUENCY) {
            // m_emitTimer = sf::Time::Zero;
            emitCount = static_cast<sf::Uint32>(m_emitTimer.asMilliseconds()
                                                / ROCKET_EXHAUST_EMIT_FREQUENCY.asMilliseconds());
            m_emitTimer = m_emitTimer % ROCKET_EXHAUST_EMIT_FREQUENCY;
            // emitNewParticle = true;
        }

        m_vertices.clear();
        // for (auto &p : m_particles)
        for (std::size_t i = 0; i < m_particles.size(); ++i) {
            // If we're on an inactive particle and we aren't
            //  going to emit a new one this frame
            auto& p = m_particles[i];
            if (!p.active && emitCount == 0)
                continue;

            if (!p.active) {
                p.position = m_position;
                p.particleLifetime = sf::Time::Zero;
                p.active = true;
                auto distSpeed = std::uniform_real_distribution<float>(ROCKET_EXHAUST_PARTICLE_MIN_SPEED,
                                                                       ROCKET_EXHAUST_PARTICLE_MAX_SPEED);

                const auto normalAngle = m_normal.angle().asDegrees();
                const auto min = normalAngle - 45.0f;
                const auto max = normalAngle + 45.0f;
                auto distAngle = std::uniform_real_distribution<float>(min, max);

                auto velocity = sf::Vector2f { distSpeed(m_randomEngine), sf::degrees(distAngle(m_randomEngine)) };
                p.velocity = velocity;
                --emitCount;
            } else {
                p.particleLifetime += dt;
                if (p.particleLifetime >= ROCKET_EXHAUST_PARTICLE_LIFETIME) {
                    p.active = false;
                    continue;
                }
                p.position += p.velocity * dt.asSeconds();
            }
            const auto t
                = std::clamp(p.particleLifetime.asSeconds() / ROCKET_EXHAUST_PARTICLE_LIFETIME.asSeconds(), 0.0f, 1.0f);
            const auto startIndex = m_vertices.getVertexCount();
            m_vertices.resize(m_vertices.getVertexCount() + 6);
            updateQuadPosition(&m_vertices[startIndex], i);
            lerpQuadColours(&m_vertices[startIndex], t);
        }
    } break;
    default:
        assert(false);
        break;
    }
}

auto ParticleEffect::isPlaying() const -> bool { return m_isPlaying; }

auto ParticleEffect::getEffectType() const -> Type { return m_effectType; }

void ParticleEffect::stop() { m_isPlaying = false; }

void ParticleEffect::setPosition(const sf::Vector2f& position) { m_position = position; }

void ParticleEffect::setNormal(const sf::Vector2f& normal) { m_normal = normal; }

void ParticleEffect::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    auto statesCopy = states;
    statesCopy.texture = m_planetCollisionTexture;
    target.draw(m_vertices, statesCopy);
}

void ParticleEffect::updateQuadPosition(sf::Vertex* vertices, std::size_t particleIndex)
{
    assert(vertices);
    switch (m_effectType) {
    case ParticleEffect::Type::Planet_Collision: {
        vertices[0].position = m_particles[particleIndex].position + PLANET_COLLISION_QUAD_OFFSETS[0];
        vertices[1].position = m_particles[particleIndex].position + PLANET_COLLISION_QUAD_OFFSETS[1];
        vertices[2].position = m_particles[particleIndex].position + PLANET_COLLISION_QUAD_OFFSETS[2];

        vertices[3].position = m_particles[particleIndex].position + PLANET_COLLISION_QUAD_OFFSETS[2];
        vertices[4].position = m_particles[particleIndex].position + PLANET_COLLISION_QUAD_OFFSETS[3];
        vertices[5].position = m_particles[particleIndex].position + PLANET_COLLISION_QUAD_OFFSETS[0];
    } break;
    case ParticleEffect::Type::Rocket_Exhaust: {
        vertices[0].position = m_particles[particleIndex].position + ROCKET_EXHAUST_QUAD_OFFSETS[0];
        vertices[1].position = m_particles[particleIndex].position + ROCKET_EXHAUST_QUAD_OFFSETS[1];
        vertices[2].position = m_particles[particleIndex].position + ROCKET_EXHAUST_QUAD_OFFSETS[2];

        vertices[3].position = m_particles[particleIndex].position + ROCKET_EXHAUST_QUAD_OFFSETS[2];
        vertices[4].position = m_particles[particleIndex].position + ROCKET_EXHAUST_QUAD_OFFSETS[3];
        vertices[5].position = m_particles[particleIndex].position + ROCKET_EXHAUST_QUAD_OFFSETS[0];
    } break;
    default:
        assert(false);
    }
}

void ParticleEffect::lerpQuadColours(sf::Vertex* vertices, float t)
{
    assert(vertices);

    sf::Color newColour;
    switch (m_effectType) {
    case ParticleEffect::Type::Planet_Collision:
        newColour = lerp_colour(PLANET_COLLISION_PARTICLE_COLOUR_START, PLANET_COLLISION_PARTICLE_COLOUR_END, t);
        break;
    case ParticleEffect::Type::Rocket_Exhaust:
        newColour = lerp_colour(ROCKET_EXHAUST_PARTICLE_COLOUR_START, ROCKET_EXHAUST_PARTICLE_COLOUR_END, t);
        break;
    default:
        assert(false);
        break;
    }
    for (std::size_t i = 0; i < 6; ++i) {
        vertices[i].color = newColour;
    }
}