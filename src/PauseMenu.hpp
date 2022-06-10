#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

class PauseMenu : public sf::Drawable {
public:
    PauseMenu(sf::RenderWindow& window);

    void update(const sf::Time& dt);
    void reset(); 

    auto returnToPlaying() const -> bool;

protected:
    virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

private:
    sf::RenderWindow& m_window;
    sf::Text m_uiPauseTitle;
    sf::Text m_uiResumeButton;
    sf::Text m_uiOptionsButton;
    sf::Text m_uiQuitButton;
    sf::RectangleShape m_pauseMenuDim;
    sf::Sound m_pauseButtonHoverSfx;
    bool m_returnToPlaying { false };
};