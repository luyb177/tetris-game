#include "Game.hpp"
#include <SFML/Window/Keyboard.hpp>
void Game::restart() { board_.reset(); score_=lines_=0; level_=1; timer_=0; gameOver_=false; spawn(); }
void Game::spawn() { currentType_=randomPiece(); current_=baseShape(currentType_); x_=4; y_=0; if (!board_.canPlace(current_,x_,y_)) gameOver_=true; }
void Game::move(int dx) { if (!gameOver_ && board_.canPlace(current_,x_+dx,y_)) x_+=dx; }
void Game::drop() { if (board_.canPlace(current_,x_,y_+1)) { ++y_; score_+=1; } else { board_.lock(current_,x_,y_,currentType_); int n=board_.clearLines(); if(n){ lines_+=n; score_ += (n==4?1200:n*100)*level_; level_=1+lines_/10; } spawn(); } }
void Game::rotatePiece() { auto r=rotate(current_); for(int dx: {0,-1,1,-2,2}) if(board_.canPlace(r,x_+dx,y_)){current_=r;x_+=dx;return;} }
void Game::handleKey(int key) { if(key==sf::Keyboard::Left)move(-1); else if(key==sf::Keyboard::Right)move(1); else if(key==sf::Keyboard::Down)drop(); else if(key==sf::Keyboard::Up)rotatePiece(); else if(key==sf::Keyboard::Space)while(!gameOver_&&board_.canPlace(current_,x_,y_+1))++y_; else if(key==sf::Keyboard::R)restart(); }
void Game::update(float dt) { if(gameOver_)return; timer_+=dt; float speed=std::max(0.08f,0.72f-(level_-1)*0.055f); if(timer_>=speed){timer_=0;drop();} }

