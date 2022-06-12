#pragma once

#include "SoundCentral.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

class PauseMenu : public sf::Drawable {
public:
    PauseMenu(sf::RenderWindow& window, SoundCentral& soundCentral);

    void update(const sf::Time& dt);
    void reset();

    auto returnToPlaying() const -> bool;

protected:
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

private:
    enum class SubMenuStage { Default = 0, Options };

    void setupUIText();
    void setupTextProperty(sf::Text& text, sf::Font* font, const sf::String& initString, sf::Uint32 charSize) const;
    sf::Vector2f getSpacedLocation(const sf::Text& previousNode) const;

    void updateDefault(const sf::Time& dt);
    void updateOptions(const sf::Time& dt);
    bool updateHoveredStatus(sf::Shape& shape);
    bool updateHoveredStatus(sf::Text& text);
    void updateVolumeUIPositions();

    sf::RenderWindow& m_window;

    sf::Text m_uiPauseTitle;
    sf::Text m_uiResumeButton;
    sf::Text m_uiOptionsButton;
    sf::Text m_uiQuitButton;
    sf::Text m_uiMasterVolumeTitle;
    sf::Text m_uiMasterVolumeIndicator;

    sf::CircleShape m_uiUpVolume;
    sf::CircleShape m_uiDownVolume;
    sf::RectangleShape m_pauseMenuDim;

    SoundCentral* m_soundCentral;
    SubMenuStage m_stage;
    bool m_returnToPlaying { false };
    sf::Shape* m_lastHoveredShape { nullptr };
    sf::Text* m_lastHoveredText { nullptr };
};
