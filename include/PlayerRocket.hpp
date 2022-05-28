#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include "PhysicsWorld.hpp"
class GameLevel;

class PlayerRocket : public sf::Drawable
{
public:
	PlayerRocket(PhysicsWorld &world, GameLevel &levelGeometry);
	void update(const sf::Time &dt);

	void levelStart();

	auto isInBounds(const sf::RenderWindow &window) const -> bool;
	auto didHitPlanet() const -> bool;
	auto getCollisionNormal() const -> sf::Vector2f;
	auto getPosition() const -> sf::Vector2f;

protected:
	virtual void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;

private:
	void emitParticle();

	std::shared_ptr<PhysicsBody> m_body;
	sf::RectangleShape m_shape;
	GameLevel &m_gameLevel;

	sf::Vector2f m_collisionNormal;
	sf::Texture m_texture;
	sf::SoundBuffer m_sbResetLevel;
	sf::Sound m_resetLevelSfx;

	sf::VertexArray m_trailVertices;

	std::size_t m_particleIndex{0};
	sf::Uint32 m_emitCounter{0};
	
	bool m_collidedWithPlanet{false};
};