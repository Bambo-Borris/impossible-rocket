#include "PlayerRocket.hpp"
#include "InputHandler.hpp"
#include "AssetHolder.hpp"

#include <array>
#include <cassert>
#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>
#include "GameplayBlackboard.hpp"

PlayerRocket::PlayerRocket(PhysicsWorld &world, GameLevel &levelGeometry)
	: m_body(world.addBody()),
	  m_gameLevel(levelGeometry),
	  m_shape({32.0f, 32.0f}),
	  m_texture(AssetHolder::get().getTexture("bin/textures/ship.png")),
	  m_sbResetLevel(AssetHolder::get().getSoundBuffer("bin/sounds/level_reset.wav")),
	  m_sbPlanetCollide(AssetHolder::get().getSoundBuffer("bin/sounds/planet_collide.wav"))
{
	m_shape.setOrigin(bb::ROCKET_SIZE * 0.5f);
	m_body->inertia = 1.0e3f;
	m_body->mass = 1.0e5f;
	m_body->transformable = &m_shape;

	m_resetLevelSfx.setBuffer(*m_sbResetLevel);
	m_planetCollideSfx.setBuffer(*m_sbPlanetCollide);

	m_shape.setTexture(m_texture);
	m_trailVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
}

void PlayerRocket::update(const sf::Time &dt)
{
	(void)dt;
	const auto state = InputHandler::get().getInputState();
	const auto direction = sf::Vector2f(1.0f, m_shape.getRotation());

	if (state.linear_thrust != 0.0f)
	{
		m_body->force += direction * (bb::THRUST_FORCE * state.linear_thrust);
	}

	if (state.angular_thrust != 0.0f)
	{
		m_body->torque += bb::TORQUE_MAG * state.angular_thrust;
	}

	auto result = m_gameLevel.doesCollideWithPlanet(m_shape.getPosition(), bb::ROCKET_SIZE.x / 2.0f);
	if (result)
	{
		m_body->isActive = false;
		m_collisionInfo = result;
		if (m_planetCollideSfx.getStatus() != sf::Sound::Status::Playing)
			m_planetCollideSfx.play();
	}

	m_gameLevel.handleObjectiveIntersections(m_shape.getPosition(), bb::ROCKET_SIZE.x / 2.0f);

	m_body->force += m_gameLevel.getSummedForce(m_shape.getPosition(), m_body->mass);
	emitParticle();

	ImGui::Begin("Debug");
	ImGui::Text("Linear Velocity {%f - %f}", m_body->linearVelocity.x, m_body->linearVelocity.y);
	ImGui::Text("Angular Velocity {%f}", m_body->angularVelocity);
	ImGui::Text("Speed {%f}", m_body->linearVelocity.length());
	ImGui::End();
}

void PlayerRocket::levelStart()
{
	m_collisionInfo.reset();

	// Shape & physics reset
	m_shape.setPosition(m_gameLevel.getPlayerStart());
	m_shape.setRotation(sf::degrees(0.0f));

	m_body->angularVelocity = 0.0f;
	m_body->linearVelocity = {};
	m_body->force = {};
	m_body->torque = 0.0f;
	m_body->isActive = true;

	// Trail reset
	m_trailVertices.clear();
	m_particleIndex = 0;
	m_emitCounter = 0;

	const sf::Vector2f offscreen = {-150.0f, -150.0f};
	std::array<sf::Vertex, 4> quad;

	quad[0].color = sf::Color::White;
	quad[1].color = sf::Color::White;
	quad[2].color = sf::Color::White;
	quad[3].color = sf::Color::White;

	quad[0].position = offscreen;
	quad[1].position = offscreen;
	quad[2].position = offscreen;
	quad[3].position = offscreen;

	m_trailVertices.resize(static_cast<std::size_t>(bb::TRAIL_PARTICLE_COUNT * 6));

	for (std::size_t i = 0; i < bb::TRAIL_PARTICLE_COUNT; ++i)
	{
		sf::Vertex *v = &m_trailVertices[i * 6];
		v[0] = quad[0];
		v[1] = quad[1];
		v[2] = quad[2];

		v[3] = quad[2];
		v[4] = quad[3];
		v[5] = quad[0];
	}

	m_resetLevelSfx.play();
}

auto PlayerRocket::isInBounds(const sf::RenderWindow &window) const -> bool
{
	const auto &view = window.getView();
	const sf::FloatRect bounds{{0.0f, 0.0f}, view.getSize()};
	return bounds.findIntersection(m_shape.getGlobalBounds()).has_value();
}

auto PlayerRocket::getCollisionInfo() const -> std::optional<GameLevel::PlanetCollisionInfo>
{
	return m_collisionInfo;
}

auto PlayerRocket::getPosition() const -> sf::Vector2f
{
	return m_shape.getPosition();
}

void PlayerRocket::draw(sf::RenderTarget &target, const sf::RenderStates &states) const
{
	target.draw(m_trailVertices, states);
	target.draw(m_shape, states);
}

void PlayerRocket::emitParticle()
{
	if (m_emitCounter < 10)
	{
		++m_emitCounter;
		return;
	}

	constexpr sf::Vector2f offsets[] =
		{
			{-bb::PARTICLE_SIZE.x / 2.0f, -bb::PARTICLE_SIZE.y / 2.0f},
			{bb::PARTICLE_SIZE.x / 2.0f, -bb::PARTICLE_SIZE.y / 2.0f},
			{bb::PARTICLE_SIZE.x / 2.0f, bb::PARTICLE_SIZE.y / 2.0f},
			{-bb::PARTICLE_SIZE.x / 2.0f, bb::PARTICLE_SIZE.y / 2.0f},
		};

	std::array<sf::Vertex, 4> quad;
	const auto currentPosition = m_shape.getPosition();

	quad[0].position = currentPosition + offsets[0];
	quad[1].position = currentPosition + offsets[1];
	quad[2].position = currentPosition + offsets[2];
	quad[3].position = currentPosition + offsets[3];

	assert(m_particleIndex * 6 < m_trailVertices.getVertexCount());

	sf::Vertex *v = &m_trailVertices[m_particleIndex * 6];
	v[0].position = quad[0].position;
	v[1].position = quad[1].position;
	v[2].position = quad[2].position;

	v[3].position = quad[2].position;
	v[4].position = quad[3].position;
	v[5].position = quad[0].position;
	m_emitCounter = 0;

	m_particleIndex = (m_particleIndex + 1) >= bb::TRAIL_PARTICLE_COUNT ? 0 : (m_particleIndex + 1);
}
