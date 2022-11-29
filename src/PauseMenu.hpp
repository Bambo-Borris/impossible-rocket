#pragma once

#include "SoundCentral.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

class GameLevel;

class PauseMenu : public sf::Drawable {
public:
    enum class SubMenuStage { Default = 0, Options, LevelSummary };

    PauseMenu(sf::RenderWindow& window, SoundCentral& soundCentral, GameLevel& level);

    void update(const sf::Time& dt);
    void reset();

    auto returnToPlaying() const -> bool;

    void setSubMenuStage(SubMenuStage stage);
    auto getStage() const -> SubMenuStage;

protected:
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

private:
    void setupUIText();
    void setupTextProperty(sf::Text& text, sf::Font* font, const sf::String& initString, std::uint32_t charSize) const;
    sf::Vector2f getSpacedLocation(const sf::Text& previousNode) const;

    void updateDefault(const sf::Time& dt);
    void updateOptions(const sf::Time& dt);
    void updateLevelSummary();
    bool updateHoveredStatus(sf::Shape& shape);
    bool updateHoveredStatus(sf::Text& text);
    void updateVolumeUIPositions();

    sf::RenderWindow& m_window;

    sf::RectangleShape m_pauseMenuDim;

    sf::Text m_uiMenuTitle;

    // Default submenu
    sf::Text m_uiResumeButton;
    sf::Text m_uiOptionsButton;
    sf::Text m_uiQuitButton;

    // Options submenu
    sf::Text m_uiMasterVolumeTitle;
    sf::Text m_uiBackToDefaultSubMenu;
    sf::Text m_uiMasterVolumeIndicator;
    
    sf::CircleShape m_uiUpVolume;
    sf::CircleShape m_uiDownVolume;
    
    // LevelSummary submenu
    sf::Text m_uiAttemptsIndicator;
    sf::Text m_uiContinueLevelButton; 

    SoundCentral* m_soundCentral;
    GameLevel* m_level;
    SubMenuStage m_stage;
    sf::Shape* m_lastHoveredShape { nullptr };
    sf::Text* m_lastHoveredText { nullptr };
    
    bool m_returnToPlaying { false };
};
