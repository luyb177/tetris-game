#pragma once
#include "Game.hpp"
#include <SFML/Graphics.hpp>
class Renderer {
public: Renderer(); void draw(sf::RenderWindow& window, const Game& game);
private: sf::Font font_; bool fontOk_{false}; void text(sf::RenderWindow&, const std::string&, unsigned, float, float, sf::Color);
};

