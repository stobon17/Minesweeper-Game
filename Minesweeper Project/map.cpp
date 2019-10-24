#include "map.h"
#include <cstdio>
#include <random>
#include <functional>
#include <iostream>	
#include <chrono>

using namespace std;

Map::Map() :
	xSize(25),
	ySize(16),
	mineField(xSize*ySize) {
	const unsigned int numMines = 10;
	Map(xSize, ySize, numMines);
}

Map::Map(const unsigned int x, const unsigned int y, const unsigned int amount) :
	mineField(x*y, 0), xSize(x), ySize(y) {
}

void Map::fill(const unsigned int seed, const unsigned int amount) {
	//Create RNG
	mt19937 randomX(seed);
	mt19937 randomY(seed + 1);
	auto xDir = bind(uniform_int_distribution<unsigned int>(0, xSize - 1), randomX);
	auto yDir = bind(uniform_int_distribution<unsigned int>(0, ySize - 1), randomY);

	//Using random number generated 0 - x*y populate map with mines
	int holder = 0;
	while (holder < amount) {
		const unsigned int x = xDir(), y = yDir();
		//Square has 8 neighbors
		if (get(x, y) > 8) {
			continue;
		}
		insertBomb(x+1, y); //while we find bug
		holder++;
	}
	return;
}

unsigned int Map::get(unsigned int x, unsigned y) {
	return mineField[y*xSize + x];
}

void Map::set(const unsigned int x, const unsigned int y, const unsigned int value) {
	if (x >= xSize || y >= ySize || x < 0 || y < 0) {
		return;
	}
	mineField[y*xSize + x] = value;
}

void Map::increment(const unsigned int x, const unsigned int y) {
	if (x >= xSize || y >= ySize || x < 0 || y < 0) {
		return;
	}
	mineField[y*xSize + x]++;
}

void Map::insertBomb(const unsigned int x, const unsigned int y) {
	set(x, y, 9);//Will be 9 when it's incremented in the new loop
	//update the neigbouring squares
	for (short i = -1; i <= 1; i++) {
		for (short j = -1; j <= 1; j++) {
			increment(int(x) + i, int(y) + j);
		}
	}
}

void Map::Reset() {
	mineField.clear();
}
