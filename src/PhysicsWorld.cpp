#include "PhysicsWorld.hpp"

constexpr auto MAX_SPEED{310.0f};
constexpr auto MAX_ANGULAR_SPEED{15.0f};

PhysicsWorld::PhysicsWorld()
{
}

void PhysicsWorld::step(const sf::Time &timeStep, const sf::Time &dt)
{
	m_accumulator += dt;
	while (m_accumulator >= timeStep)
	{
		integrate(timeStep);
		m_accumulator -= dt;
	}
}

std::shared_ptr<PhysicsBody> PhysicsWorld::addBody()
{
	auto body = std::make_shared<PhysicsBody>();
	m_bodies.push_back(body);
	return body;
}

void PhysicsWorld::integrate(const sf::Time &timeStep)
{
	const auto stepAsSeconds = timeStep.asSeconds();
	for (auto &body : m_bodies)
	{
		// Immovable!
		if (body->mass == 0.0f)
		{
			continue;
		}

		const float invMass = 1.0f / body->mass;
		const float invInertia = 1.0f / body->inertia;

		// Linear
		const sf::Vector2f acceleration = body->force * invMass;

		body->linearVelocity += acceleration * stepAsSeconds;
		const auto speed = std::min(body->linearVelocity.length(), MAX_SPEED);
		if (body->linearVelocity != sf::Vector2f{})
			body->linearVelocity = body->linearVelocity.normalized() * speed;

		body->transformable->move(body->linearVelocity * stepAsSeconds);

		// Oriented
		body->angularVelocity += body->torque * invInertia * stepAsSeconds;
		body->angularVelocity = std::min(body->angularVelocity, MAX_ANGULAR_SPEED);

		body->transformable->rotate(sf::radians(body->angularVelocity * stepAsSeconds));

		// Clear force & torque
		body->force = {};
		body->torque = 0.0f;
	}
}
