#pragma once

//This will take care of SFML duties
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

class EventClass {
public:
	EventClass();
	void handleEvent(sf::Event function);

protected:
	virtual void handleClose(sf::Event function) {};

	virtual void handleLeftClick(sf::Event function) {};
	virtual void handleLeftRelease(sf::Event function) {};

	virtual void handleRightClick(sf::Event function) {};
	virtual void handleRightRelease(sf::Event function) {};

	virtual void handleMiddleClick(sf::Event function) {};
	virtual void handleMiddleRelease(sf::Event function) {};

	virtual void handleResize(sf::Event function) {};
	virtual void handleFocus(sf::Event function) {};

	virtual void handleMouseMove(sf::Event function) {};
	virtual void handleMouseDrag(sf::Event function) {};

	void handleClick(sf::Event function);
	void handleRelease(sf::Event function);

	//Keep track of mouse previous location
	sf::Vector2i previousMouseLocation;

private:
	bool LMBstatus;
	void leftClickHelper(sf::Event function);
	void leftReleaseHelper(sf::Event function);
	void mouseMovedHelper(sf::Event function);
};