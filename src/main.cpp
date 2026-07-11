#include "Game.hpp"
#include "Renderer.hpp"
#include <SFML/Graphics.hpp>
int main(){sf::RenderWindow window(sf::VideoMode(760,780),"Neon Tetris",sf::Style::Titlebar|sf::Style::Close);window.setFramerateLimit(60);Game game;game.restart();Renderer renderer;sf::Clock clock;while(window.isOpen()){sf::Event e{};while(window.pollEvent(e)){if(e.type==sf::Event::Closed)window.close();if(e.type==sf::Event::KeyPressed)game.handleKey(e.key.code);}game.update(clock.restart().asSeconds());renderer.draw(window,game);window.display();}return 0;}

