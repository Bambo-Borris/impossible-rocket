#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

#include "GameLevel.hpp"
#include "PhysicsWorld.hpp"
#include "SoundCentral.hpp"

class PlayerRocket : public sf::Drawable {
public:
    PlayerRocket(PhysicsWorld& world, GameLevel& levelGeometry, SoundCentral& soundCentral);
    void update(const sf::Time& dt);

    void levelStart();

    auto isInBounds(const sf::RenderWindow& window) const -> bool;
    auto getCollisionInfo() const -> std::optional<GameLevel::PlanetCollisionInfo>;

    auto getPosition() const -> sf::Vector2f;
    auto getExhaustPoint() const -> sf::Vector2f;
    auto getExhaustDirection() const -> sf::Vector2f;
    auto isPlayerApplyingForce() const -> bool;
    auto getRotation() const -> sf::Angle;

protected:
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

private:
    std::shared_ptr<PhysicsBody> m_body;
    sf::RectangleShape m_shape;
    GameLevel& m_gameLevel;

    std::optional<GameLevel::PlanetCollisionInfo> m_collisionInfo;
    SoundCentral* m_soundCentral;
};