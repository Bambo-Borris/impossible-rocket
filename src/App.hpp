#pragma once

#include <SFML/Graphics.hpp>

#include "BaseState.hpp"

#include <memory>
#include <stack>

class App {
public:
    App();
    ~App();

    void run();

private:
    void logFPS(const sf::Time& dt);

    sf::RenderWindow m_window;
    std::stack<std::unique_ptr<BaseState>> m_states;
};
