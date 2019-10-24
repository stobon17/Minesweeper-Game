#pragma once

#include "map.h"
#include "eventClass.h"
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>

enum class userChoice : char;

class Game : public EventClass {
public:
	Game();
	bool reset;
	void assertTilesCovered();
	void runGame();
private:
	//Game conditions
	void toggleFlag(unsigned int xPos, unsigned yPos);
	void checkWinCondition();
	void uncoverSquare(unsigned int xPos, unsigned int yPos);

	//Drawing window and closing
	sf::RenderWindow gameWindow;
	void draw();
	void endGame();

	//Game statusi
	bool gameisRunning;
	
	bool eventTriggered;
	Map board;
	vector<userChoice> gameChoice;
	int numBombs;
	userChoice getChoice(unsigned int xPos, unsigned int yPos) {
		return gameChoice[yPos*board.x() + xPos];
	}
	void setChoice(unsigned int xPos, unsigned int yPos, userChoice choice) {
		gameChoice[yPos*board.x() + xPos] = choice;
	}
	void getSquareFromPos(int &xPos, int &yPos, const sf::Event &function) {
		xPos = function.mouseButton.x / (squareSize + 1);
		yPos = function.mouseButton.y / (squareSize + 1);
	}
	void getButtonFromPos(int &xPos, int &yPos, const  sf::Event &function) {
		xPos = function.mouseButton.x / (64);
		yPos = function.mouseButton.y / 64;
	}
	//Textures/Sprites
	vector<sf::Texture> gameTextures;
	void loadTexture();
	sf::Sprite getSprite(int xPos, int yPos);

	//Overrides
	virtual void handleClose(sf::Event function) override;
	virtual void handleLeftClick(sf::Event function) override;
	virtual void handleRightClick(sf::Event function) override;
	virtual void handleResize(sf::Event function) override;
	virtual void handleMiddleClick(sf::Event function) override;
	

};