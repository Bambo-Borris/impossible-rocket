#include "PlayerRocket.hpp"
#include "AssetHolder.hpp"
#include "InputHandler.hpp"

#include "GameplayBlackboard.hpp"
#include <array>
#include <cassert>
#include <imgui-SFML.h>
#include <imgui.h>

PlayerRocket::PlayerRocket(PhysicsWorld& world, GameLevel& levelGeometry, SoundCentral& soundCentral)
    : m_body(world.addBody())
    , m_gameLevel(levelGeometry)
    , m_soundCentral(&soundCentral)
{
    m_shape.setOrigin(bb::ROCKET_SIZE * 0.5f);
    m_body->inertia = 1.0e3f;
    m_body->mass = 1.0e5f;
    m_body->transformable = &m_shape;

    auto texture { AssetHolder::get().getTexture("bin/textures/ship.png") };
    // auto sbLevelReset { AssetHolder::get().getSoundBuffer("bin/sounds/level_reset.wav") };
    // auto sbCollide { AssetHolder::get().getSoundBuffer("bin/sounds/planet_collide.wav") };

    // m_resetLevelSfx.setBuffer(*sbLevelReset);
    // m_planetCollideSfx.setBuffer(*sbCollide);

    m_shape.setTexture(texture);
    m_shape.setSize(sf::Vector2f { texture->getSize() });
}

void PlayerRocket::update(const sf::Time& dt)
{
    (void)dt;
    const auto state = InputHandler::get().getInputState();
    const auto direction = sf::Vector2f(1.0f, m_shape.getRotation());

    if (state.linear_thrust != 0.0f) {
        m_body->force += direction * (bb::THRUST_FORCE * state.linear_thrust);
    }

    if (state.angular_thrust != 0.0f) {
        m_body->torque += bb::TORQUE_MAG * state.angular_thrust;
    }

    auto result = m_gameLevel.doesCollideWithPlanet(m_shape.getPosition(), bb::ROCKET_SIZE.x / 2.0f);
    if (result) {
        m_body->isActive = false;
        m_collisionInfo = result;
        if (m_soundCentral->getSoundStatus(SoundCentral::SoundEffectTypes::PlanetCollision)
            != sf::Sound::Status::Playing)
            m_soundCentral->playSoundEffect(SoundCentral::SoundEffectTypes::PlanetCollision);
    }

    m_gameLevel.handleObjectiveIntersections(m_shape.getPosition(), bb::ROCKET_SIZE.x / 2.0f);

    m_body->force += m_gameLevel.getSummedForce(m_shape.getPosition(), m_body->mass);

#if defined(IMPOSSIBLE_ROCKET_DEBUG)
    if (InputHandler::get().wasHaltKeyPressed()) {
        m_body->force = {};
        m_body->linearVelocity = {};
        m_body->angularVelocity = 0.0f;
        m_body->torque = 0.0f;
    }
#endif

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

    m_soundCentral->playSoundEffect(SoundCentral::SoundEffectTypes::LevelStart);
}

auto PlayerRocket::isInBounds(const sf::RenderWindow& window) const -> bool
{
    const auto& view = window.getView();
    const sf::FloatRect bounds { { 0.0f, 0.0f }, view.getSize() };
    return bounds.findIntersection(m_shape.getGlobalBounds()).has_value();
}

auto PlayerRocket::getCollisionInfo() const -> std::optional<GameLevel::PlanetCollisionInfo> { return m_collisionInfo; }

auto PlayerRocket::getPosition() const -> sf::Vector2f { return m_shape.getPosition(); }

auto PlayerRocket::getExhaustPoint() const -> sf::Vector2f
{
    return m_shape.getTransform().transformPoint({ 2.0f, 16.0f });
}

auto PlayerRocket::getExhaustDirection() const -> sf::Vector2f
{
    const auto angle = m_shape.getRotation();
    const auto thrust = InputHandler::get().getInputState().linear_thrust
        / std::abs(InputHandler::get().getInputState().linear_thrust);
    return { sf::Vector2f { 1.0f, angle } * -thrust };
}

auto PlayerRocket::isPlayerApplyingForce() const -> bool
{
    const auto state = InputHandler::get().getInputState();
    return state.linear_thrust != 0.0f;
}

auto PlayerRocket::getRotation() const -> sf::Angle { return m_shape.getRotation(); }

void PlayerRocket::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    target.draw(m_shape, states);
}