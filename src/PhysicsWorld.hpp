#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <memory>

struct PhysicsBody
{
	sf::Transformable *transformable{nullptr};
	float mass{0.0f};
	float inertia{0.0f};

	float torque{0.0f};
	float angularVelocity{0.0f};

	sf::Vector2f force;
	sf::Vector2f linearVelocity;
	bool isActive{ true };
};

class PhysicsWorld
{
public:
	PhysicsWorld();

	void step(const sf::Time &timeStep, const sf::Time &dt);
	std::shared_ptr<PhysicsBody> addBody();

private:
	void integrate(const sf::Time &timeStep);

	std::vector<std::shared_ptr<PhysicsBody>> m_bodies;
	sf::Time m_accumulator;
};