#pragma once

#include "BaseState.hpp"
#include "GameLevel.hpp"
#include "ParticleEffect.hpp"
#include "PauseMenu.hpp"
#include "PhysicsWorld.hpp"
#include "PlayerRocket.hpp"
#include "SoundCentral.hpp"

#include <memory>

class PlayState : public BaseState {
public:
    PlayState(sf::RenderWindow& window);
    ~PlayState() = default;

    virtual void enter() override;
    virtual void update(const sf::Time& dt) override;
    virtual void draw() const override;

private:
    enum class Status { Playing, Paused, End_Of_Level };

    void updatePlaying(const sf::Time& dt);
    void updatePaused(const sf::Time& dt);
    void updateEndOfLevel(const sf::Time& dt);
    void particleEffectUpdate();
    void outOfBoundsUpdate();

    SoundCentral m_soundCentral;
    PhysicsWorld m_physicsWorld;
    GameLevel m_gameLevel;
    PlayerRocket m_rocket;
    PauseMenu m_pauseMenu;

    sf::RectangleShape m_backgroundSprite;
    sf::RectangleShape m_oobDirectionIndicator;
    sf::Text m_uiAttempts;
    sf::Text m_uiOOB;
    sf::Clock m_oobTimer; // out of bounds timer

    std::vector<std::unique_ptr<ParticleEffect>> m_particleEffects;
    bool m_isOutOfBounds { false };
    PlayState::Status m_status { PlayState::Status::Playing };
};
