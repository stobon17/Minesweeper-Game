#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

enum Texture : char {
	HIDDEN, FLAGGED, BOMB = 11
};

enum class userChoice : char {
	HIDDEN, UNHIDDEN, FLAGGED
};

//Variable INITIALIZATION
bool resetGame;
bool noWinOrLoss = true;
bool lose = false;
bool win = false;
bool debugActive = false;
int clickCounter;
bool test1board = false;
bool test2board = false;
sf::Sprite happyFace;
sf::Sprite loseFace;
sf::Sprite winFace;
sf::Sprite debugMineSprite;
sf::Sprite test1Sprite;
sf::Sprite test2Sprite;
sf::Texture hiddenTile;
sf::Sprite tileHidden;
//Initialize game
Game::Game()
	: EventClass(), gameisRunning(true), board(Map()), gameChoice(board.size(), userChoice::HIDDEN),
	gameWindow(sf::VideoMode(800, 600), "Minesweeper", sf::Style::Close),
	eventTriggered(true), numBombs(50) {
	loadTexture();
	board.fill(numBombs);
	gameWindow.clear(sf::Color(255, 255, 255, 0));
	assertTilesCovered();
}

//Game logic

//FLAGS
void Game::toggleFlag(unsigned int xPos, unsigned yPos) {
	//Parameter validity check
	if (xPos < 0 || xPos >= board.x() || yPos < 0 || yPos >= board.y()) {
		return;
	}
	if (getChoice(xPos, yPos) == userChoice::FLAGGED) {
		setChoice(xPos, yPos, userChoice::HIDDEN);
		numBombs++;
	}
	else if (getChoice(xPos, yPos) == userChoice::HIDDEN) {
		hiddenTile.loadFromFile("images/tile_hidden.png");
		tileHidden.setTexture(hiddenTile);
		tileHidden.setPosition(xPos, yPos);
		gameWindow.draw(tileHidden);//Keep hidden layer tho
		setChoice(xPos, yPos, userChoice::FLAGGED);
		numBombs--;
	}
	else if (getChoice(xPos, yPos) == userChoice::UNHIDDEN) {
		hiddenTile.loadFromFile("images/tile_hidden.png");
		tileHidden.setTexture(hiddenTile);
		tileHidden.setPosition(xPos, yPos);
		gameWindow.draw(tileHidden);//Keep hidden layer tho
		setChoice(xPos, yPos, userChoice::FLAGGED);
		numBombs--;
	}
	return;
}
//END GAME
void Game::endGame() {
	hiddenTile.loadFromFile("images/tile_hidden.png");
	tileHidden.setTexture(hiddenTile);
	for (unsigned int x = 0; x < board.x(); x++) {
		for (unsigned int y = 0; y < board.y(); y++) {
			if (board.get(x, y) > 8) { // Square 8 neighbors so if greater bomb reveal all bombs
				if (win == true) {
					tileHidden.setPosition(x, y);
					gameWindow.draw(tileHidden);
					setChoice(x, y, userChoice::FLAGGED);
				}//Set bombs to flagged
				else if (win == false) {
					setChoice(x, y, userChoice::UNHIDDEN); // Reveal bombs
				}
			}
		}
	}
	return;
}

//CHECK WIN CONDITION
void Game::checkWinCondition() { //Still some bugs
	for (unsigned int xPos = 0; xPos < board.x(); xPos++) {
		for (unsigned int yPos = 0; yPos < board.y(); yPos++) {
			if (getChoice(xPos, yPos) == userChoice::HIDDEN) {
				if (board.get(xPos, yPos) > 8 && debugActive == false) { //Reveal all bombs that are hidden and end game
					endGame();
				}
				else {
					return;
				}
			}
			if (getChoice(xPos, yPos) == userChoice::FLAGGED) {
				if (board.get(xPos, yPos) <= 8) {
					return;
				}
			}
		}
	}
	
	if (lose == false) {
		cout << "You win" << endl;
		win = true;
		lose = false;
		noWinOrLoss = false;
		numBombs = 0;
	}
	endGame();
}

//UNCOVER SQUARES
void Game::uncoverSquare(unsigned int xPos, unsigned int yPos) {
	//Parameter validity check
	if (xPos < 0 || xPos >= board.x() || yPos < 0 || yPos >= board.y()) {
		return;
	}

	userChoice currentChoice = getChoice(xPos, yPos);
	if (currentChoice != userChoice::HIDDEN) {
		return;
	}
	else if (currentChoice == userChoice::UNHIDDEN && board.get(xPos, yPos) > 8) {
		cout << "You lost!" << endl; //Since it was found a bomb, display dead face and end game
		noWinOrLoss = false;
		lose = true;
		win = false;
		endGame();
		return;
	}
	//uncover current square and check if it's a bomb before proceeding
	setChoice(xPos, yPos, userChoice::UNHIDDEN);
	if (board.get(xPos, yPos) > 8 || board.get(0,0) == Texture::BOMB) { // If the number is greater than 8 then it has to be a bomb, a square has 8 neighbors one must be bomb
		cout << "You lost!" << endl; //Since it was found a bomb, display dead face and end game
		noWinOrLoss = false;
		lose = true;
		win = false;
		endGame();
		return;
	}

	//uncover the neighboring squares if this square is an empty one
	if (board.get(xPos, yPos) == 0) {
		for (int i = -1; i <= 1; i += 2) {
			uncoverSquare(xPos + i, yPos);
			uncoverSquare(xPos, yPos + i);
		}
	}
	//if this square isn't empty, return without uncovering the neighbors, added for clarity
	else {
		return;
	}
}

//Textures and Sprite and Drawing
sf::Sprite Game::getSprite(int xPos, int yPos) {
	userChoice currentChoice = getChoice(xPos, yPos);
	if (currentChoice == userChoice::FLAGGED) {
		return sf::Sprite(gameTextures[Texture::FLAGGED]); // Return flag texture
	}
	if (currentChoice == userChoice::HIDDEN) {
		return sf::Sprite(gameTextures[Texture::HIDDEN]); //Return hidden square texture (solid)
	}
	if (currentChoice == userChoice::UNHIDDEN) {
		int bombCount = board.get(xPos, yPos);
		if (bombCount > 8) { // Again if > 8 must be bomb
			return sf::Sprite(gameTextures[Texture::BOMB]); // Return bomb texture
		}
		return sf::Sprite(gameTextures[bombCount + 2]);
	}
	return sf::Sprite();
}

void Game::draw() {
	gameWindow.clear(sf::Color(255, 255, 255, 0)); //Set background to white
	//iterate through squares, checking their status
	for (unsigned int x = 0; x < board.x(); x++) {
		for (unsigned int y = 0; y < board.y(); y++) {
			sf::Sprite sprite = getSprite(x, y);
			sf::Sprite unhidden; //The unhidden sprite is the layer under the actual relevant sprite, this is so it won't have a black background
			unhidden.setTexture(gameTextures[2]);
			sprite.setPosition(x*(squareSize + 1), y*(squareSize + 1));
			unhidden.setPosition(x*(squareSize + 1), y*(squareSize + 1));
			gameWindow.draw(unhidden);
			gameWindow.draw(sprite);//Draw corresponding status
			if (noWinOrLoss) {
				sf::Texture happyText;
				happyText.loadFromFile("images/face_happy.png");
				happyFace.setTexture(happyText);
				happyFace.setPosition(364, 512);
				gameWindow.draw(happyFace);
				lose = false;
				win = false;
			}
			else if (lose) {
				sf::Texture loseText;
				loseText.loadFromFile("images/face_lose.png");
				loseFace.setTexture(loseText);
				loseFace.setPosition(364, 512);
				gameWindow.draw(loseFace);
				win = false;
				noWinOrLoss = false;
				debugActive = false;
			}
			else if (win) {
				sf::Texture winText;
				winText.loadFromFile("images/face_win.png");
				winFace.setTexture(winText);
				winFace.setPosition(364, 512);
				gameWindow.draw(winFace);
				noWinOrLoss = false;
				lose = false;
				debugActive = false;
			}
		}
	}
	//Test 1 Button
	sf::Texture test1Text;
	test1Text.loadFromFile("images/test_1.png");
	//sf::Sprite test1Sprite; -- declared in global scope, added for clarity
	test1Sprite.setTexture(test1Text);
	test1Sprite.setPosition(560, 512);
	gameWindow.draw(test1Sprite);

	//Test 2 button
	sf::Texture test2Text;
	test2Text.loadFromFile("images/test_2.png");
	//sf::Sprite test2Sprite; -- declared in global scope, added for clarity
	test2Sprite.setTexture(test2Text);
	test2Sprite.setPosition(624, 512);
	gameWindow.draw(test2Sprite);

	//Debug(see mines) button
	sf::Texture debugMineText;
	debugMineText.loadFromFile("images/debug.png");
	//sf::Sprite debugMineSprite; --already declared in global scope, added for clarity
	debugMineSprite.setTexture(debugMineText);
	debugMineSprite.setPosition(496, 512);
	gameWindow.draw(debugMineSprite);

	//Mine counter drawing :
	//----Set The sprites first

	//Num 0
	sf::Texture num0Text;
	num0Text.loadFromFile("images/digits.png", sf::IntRect(0, 0, 21, 32));
	sf::Sprite num0Sprite(num0Text); // Wow found a shortcut just now

	//Num 1
	sf::Texture num1Text;
	num1Text.loadFromFile("images/digits.png", sf::IntRect(21, 0, 21, 32));
	sf::Sprite num1Sprite(num1Text);

	//Num 2
	sf::Texture num2Text;
	num2Text.loadFromFile("images/digits.png", sf::IntRect(42, 0, 21, 32));
	sf::Sprite num2Sprite(num2Text);

	//Num 3
	sf::Texture num3Text;
	num3Text.loadFromFile("images/digits.png", sf::IntRect(63, 0, 21, 32));
	sf::Sprite num3Sprite(num3Text);
	
	//Num 4
	sf::Texture num4Text;
	num4Text.loadFromFile("images/digits.png", sf::IntRect(84, 0, 21, 32));
	sf::Sprite num4Sprite(num4Text);

	//Num 5
	sf::Texture num5Text;
	num5Text.loadFromFile("images/digits.png", sf::IntRect(105, 0, 21, 32));
	sf::Sprite num5Sprite(num5Text);

	//Num 6
	sf::Texture num6Text;
	num6Text.loadFromFile("images/digits.png", sf::IntRect(126, 0, 21, 32));
	sf::Sprite num6Sprite(num6Text);

	//Num 7
	sf::Texture num7Text;
	num7Text.loadFromFile("images/digits.png", sf::IntRect(147, 0, 21, 32));
	sf::Sprite num7Sprite(num7Text);

	//Num 8
	sf::Texture num8Text;
	num8Text.loadFromFile("images/digits.png", sf::IntRect(168, 0, 21, 32));
	sf::Sprite num8Sprite(num8Text);
	

	//Num 9
	sf::Texture num9Text;
	num9Text.loadFromFile("images/digits.png", sf::IntRect(189, 0, 21, 32));
	sf::Sprite num9Sprite(num9Text);

	//Actually Drawing the Mine count
	int numberOfMines = numBombs;
	string str = to_string(numberOfMines);

	if (numBombs < 0) {
		num0Sprite.setPosition(0, 512);
		gameWindow.draw(num0Sprite);
		num0Sprite.setPosition(21, 512);
		gameWindow.draw(num0Sprite);
		num0Sprite.setPosition(42, 512);
		gameWindow.draw(num0Sprite); //This will yield '000'
	}
	else if (str.size() == 1) { //1 Digit ex. 0-9
		num0Sprite.setPosition(0, 512);
		gameWindow.draw(num0Sprite);
		num0Sprite.setPosition(21, 512);
		gameWindow.draw(num0Sprite);

		if(str[0] == '0') {
			num0Sprite.setPosition(42, 512);
			gameWindow.draw(num0Sprite);
		}
		else if (str[0] == '1') {
			num1Sprite.setPosition(42, 512);
			gameWindow.draw(num1Sprite);
		}
		else if (str[0] == '2') {
			num2Sprite.setPosition(42, 512);
			gameWindow.draw(num2Sprite);
		}
		else if (str[0] == '3') {
			num3Sprite.setPosition(42, 512);
			gameWindow.draw(num3Sprite);
		}
		else if (str[0] == '4') {
			num4Sprite.setPosition(42, 512);
			gameWindow.draw(num4Sprite);
		}
		else if (str[0] == '5') {
			num5Sprite.setPosition(42, 512);
			gameWindow.draw(num5Sprite);
		}
		else if (str[0] == '6') {
			num6Sprite.setPosition(42, 512);
			gameWindow.draw(num6Sprite);
		}
		else if (str[0] == '7') {
			num7Sprite.setPosition(42, 512);
			gameWindow.draw(num7Sprite);
		}
		else if (str[0] == '8') {
			num8Sprite.setPosition(42, 512);
			gameWindow.draw(num8Sprite);
		}
		else if (str[0] == '9') {
			num9Sprite.setPosition(42, 512);
			gameWindow.draw(num9Sprite);
		}
	}
	else if (str.size() == 2) { //2 Digits ex. 10 - 99
		for (int i = 0; i < str.size() + 1; i++) {
			if (i == 0) {
				num0Sprite.setPosition(0, 512);
				gameWindow.draw(num0Sprite);
			}
			else {
				if (str[i - 1] == '0') {
					num0Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num0Sprite);
				}
				else if (str[i - 1] == '1') {
					num1Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num1Sprite);
				}
				else if (str[i - 1] == '2') {
					num2Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num2Sprite);
				}
				else if (str[i - 1] == '3') {
					num3Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num3Sprite);
				}
				else if (str[i - 1] == '4') {
					num4Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num4Sprite);
				}
				else if (str[i - 1] == '5') {
					num5Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num5Sprite);
				}
				else if (str[i - 1] == '6') {
					num6Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num6Sprite);
				}
				else if (str[i - 1] == '7') {
					num7Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num7Sprite);
				}
				else if (str[i - 1] == '8') {
					num8Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num8Sprite);
				}
				else if (str[i - 1] == '9') {
					num9Sprite.setPosition(21 * i, 512);
					gameWindow.draw(num9Sprite);
				}
			}
		}
	}
	else if (str.size() == 3) { //3 Digit ex. 100 - 999
		for (int i = 0; i < str.size() + 1; i++) {
			if (str[i] == '0') {
				num0Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num0Sprite);
			}
			else if (str[i] == '1') {
				num1Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num1Sprite);
			}
			else if (str[i] == '2') {
				num2Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num2Sprite);
			}
			else if (str[i] == '3') {
				num3Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num3Sprite);
			}
			else if (str[i] == '4') {
				num4Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num4Sprite);
			}
			else if (str[i] == '5') {
				num5Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num5Sprite);
			}
			else if (str[i] == '6') {
				num6Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num6Sprite);
			}
			else if (str[i] == '7') {
				num7Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num7Sprite);
			}
			else if (str[i] == '8') {
				num8Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num8Sprite);
			}
			else if (str[i] == '9') {
				num9Sprite.setPosition(21 * i, 512);
				gameWindow.draw(num9Sprite);
			}
		}
	}
	//Display all sprites
	gameWindow.display();
}

//Just make sure tiles are covered
void Game::assertTilesCovered() {
	for (unsigned int x = 0; x < board.x(); x++) {
		for (unsigned int y = 0; y < board.y(); y++) {
			if (board.get(x, y) > 8) {
				setChoice(x, y, userChoice::HIDDEN);
			}
		}
	}
	setChoice(0, 0, userChoice::HIDDEN);
}

//RUN GAME
void Game::runGame() {
	while (gameWindow.isOpen()) {
		sf::Event function;
		if (noWinOrLoss == true) {
			draw();
			checkWinCondition();
		}
		while (gameWindow.pollEvent(function)) {
			if (noWinOrLoss == true) {
				handleEvent(function);
			}
			else if (noWinOrLoss == false) {
				if (happyFace.getGlobalBounds().contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
					resetGame = true;
					handleEvent(function);
				}

			}
		}
		if (eventTriggered) {
			draw();
			eventTriggered = false;
		}
	}
}

void Game::loadTexture() {
	//Refresh texture
	gameTextures.clear();

	vector<string> images = { "images/tile_hidden.png", "images/flag.png", "images/tile_revealed.png", "images/number_1.png", "images/number_2.png", "images/number_3.png", "images/number_4.png", "images/number_5.png", "images/number_6.png", "images/number_7.png", "images/number_8.png", "images/mine.png" };

	for (auto image : images) {
		sf::Texture texture;
		texture.loadFromFile(image);
		gameTextures.push_back(texture);
	}
}
//CLOSE
void Game::handleClose(sf::Event function) {
	gameWindow.close();
}

//LEFT CLICKS
void Game::handleLeftClick(sf::Event function) {
	if (resetGame == false) {
		int xPos = 0;
		int yPos = 0;
		getSquareFromPos(xPos, yPos, function);
		uncoverSquare(xPos, yPos);
		checkWinCondition();
		eventTriggered = true;
	}
	else {
		//Reset the game
		gameisRunning = true; 
		board = Map(); 
		for (int i = 0; i < board.size(); i++) {
			gameChoice[i] = userChoice::HIDDEN;
		}
		numBombs = 50;
		board.fill(numBombs);
		checkWinCondition();
		gameWindow.clear(sf::Color(255, 255, 255, 0));
		eventTriggered = true;
		noWinOrLoss = true;
		win = false;
		lose = false;
		resetGame = false;
		clickCounter = 0;
		
	}
	
	//Get new board
	if (happyFace.getGlobalBounds().contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
		gameisRunning = true;
		board = Map();
		for (int i = 0; i < board.size(); i++) {
			gameChoice[i] = userChoice::HIDDEN;
		}
		numBombs = 50;
		board.fill(numBombs);
		gameWindow.clear(sf::Color(255, 255, 255, 0));
		eventTriggered = true;
		noWinOrLoss = true;
		win = false;
		lose = false;
		resetGame = false;
		if (clickCounter == 1) {
			for (unsigned int x = 0; x < board.x(); x++) {
				for (unsigned int y = 0; y < board.y(); y++) {
					if (board.get(x, y) > 8) {
						setChoice(x, y, userChoice::UNHIDDEN);
					}
				}
			}
		}
		else {
			clickCounter = 0;
		}
	}
	//Debug mode
	if (debugMineSprite.getGlobalBounds().contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
		if (clickCounter == 0) {
			for (unsigned int x = 0; x < board.x(); x++) {
				for (unsigned int y = 0; y < board.y(); y++) {
					if (board.get(x, y) > 8) {
						setChoice(x, y, userChoice::UNHIDDEN);
					}
				}
			}
			debugActive == true;
			clickCounter = 1;
			eventTriggered = true;
		}
		else if (clickCounter == 1) {
			for (unsigned int x = 0; x < board.x(); x++) {
				for (unsigned int y = 0; y < board.y(); y++) {
					if (board.get(x, y) > 8) {
						setChoice(x, y, userChoice::HIDDEN);
					}
				}
			}
			noWinOrLoss = true;
			win = false;
			lose = false;
			eventTriggered = true;
			debugActive = false;
			clickCounter = 0;
		}
	}
	//Testboards

	//Testboard 1
	if (test1Sprite.getGlobalBounds().contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
		//Refresh game
		gameisRunning = true;
		board = Map();
		for (int i = 0; i < board.size(); i++) {
			gameChoice[i] = userChoice::HIDDEN;
		}
		numBombs = 0; //we will add to later
		//board.fill(numBombs);
		gameWindow.clear(sf::Color(255, 255, 255, 0));
		eventTriggered = true;
		noWinOrLoss = true;
		win = false;
		lose = false;
		resetGame = false;
		test1board = true;

		vector <string> testboard1;
		vector <char> charVec;
		char myArray[16][25];
		ifstream file("boards/testboard.brd");
		if (!file) {
			cout << "failed to open";
			throw - 1;
		}
		else {
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 25; j++) {
				file >> myArray[i][j];
			}
		}
			}
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 25; j++) {
					if (myArray[i][j] == '1') {
						board.insertBomb(j, i);
						numBombs++;
						setChoice(j, i, userChoice::HIDDEN);
					}
				}
			}
			//Hide tiles
			assertTilesCovered();
	}

	//Testboard 2
	if (test2Sprite.getGlobalBounds().contains(sf::Mouse::getPosition(gameWindow).x, sf::Mouse::getPosition(gameWindow).y)) {
		//Refresh game
		gameisRunning = true;
		board = Map();
		for (int i = 0; i < board.size(); i++) {
			gameChoice[i] = userChoice::HIDDEN;
		}
		numBombs = 0; //we will add to later
		//board.fill(numBombs);
		gameWindow.clear(sf::Color(255, 255, 255, 0));
		eventTriggered = true;
		noWinOrLoss = true;
		win = false;
		lose = false;
		resetGame = false;
		test1board = true;

		vector <string> testboard2;
		vector <char> charVec;
		char myArray[16][25];
		ifstream file("boards/testboard2.brd");
		if (!file) {
			cout << "failed to open";
			throw - 1;
		}
		else {
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 25; j++) {
					file >> myArray[i][j];
				}
			}
		}
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 25; j++) {
				if (myArray[i][j] == '1') {
					board.insertBomb(j, i);
					numBombs++;
					setChoice(j, i, userChoice::HIDDEN);
				}
			}
		}
	}
}

//Handle RIGHT CLICK
void Game::handleRightClick(sf::Event function) {
	int xPos = 0;
	int yPos = 0;
	getButtonFromPos(xPos, yPos, function);
	getSquareFromPos(xPos, yPos, function);
	toggleFlag(xPos, yPos);
	checkWinCondition();
	eventTriggered = true;
}

void Game::handleResize(sf::Event function) {
	eventTriggered = true;
}

void Game::handleMiddleClick(sf::Event event) {
	//Just to prevent unhandled case if someone middleclicked
}