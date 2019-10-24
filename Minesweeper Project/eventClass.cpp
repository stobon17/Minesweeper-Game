#include "eventClass.h"

EventClass::EventClass() : LMBstatus{ false }, previousMouseLocation{}{
}

void EventClass::handleEvent(sf::Event function) {
	switch (function.type) {
	case (sf::Event::Closed): handleClose(function); break;
	case (sf::Event::MouseButtonPressed): handleClick(function); break;
	case (sf::Event::MouseButtonReleased): handleRelease(function); break;
	case (sf::Event::Resized): handleResize(function); break;
	case (sf::Event::MouseEntered): handleFocus(function); break;
	case (sf::Event::MouseMoved): mouseMovedHelper(function); break;
	}
}

void EventClass::handleClick(sf::Event function) {
	switch (function.mouseButton.button) {
	case sf::Mouse::Left: leftClickHelper(function); break;
	case sf::Mouse::Right: handleRightClick(function); break;
	case sf::Mouse::Middle: handleMiddleClick(function); break;
	}
}

void EventClass::handleRelease(sf::Event function) {
	switch (function.mouseButton.button) {
	case sf::Mouse::Left: leftReleaseHelper(function); break;
	case sf::Mouse::Right: handleRightRelease(function); break;
	case sf::Mouse::Middle: handleMiddleRelease(function); break;
	}
}

void EventClass::leftClickHelper(sf::Event function) {
	LMBstatus = true;
	previousMouseLocation.x = function.mouseButton.x;
	previousMouseLocation.y = function.mouseButton.y;
	handleLeftClick(function);
}

void EventClass::leftReleaseHelper(sf::Event function) {
	LMBstatus = false;
	handleLeftRelease(function);
}

void EventClass::mouseMovedHelper(sf::Event function) {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		handleMouseDrag(function);
		previousMouseLocation.x = function.mouseMove.x;
		previousMouseLocation.y = function.mouseMove.y;
	}
	handleMouseMove(function);
}