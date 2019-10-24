#pragma once

#include <chrono>
#include <random>
#include <vector>

using namespace std;

const unsigned int squareSize = 31;

class Map {
public:
	//Default constructor
	Map();
	//Paramater constructor
	Map(const unsigned int x, const unsigned int y, const unsigned int amount);

	//Member functions
	void set(const unsigned int x, const unsigned int y, const unsigned int value);
	unsigned int get(unsigned int x, unsigned y);
	unsigned int x() const {
		return xSize;
	}
	unsigned int y() const {
		return ySize;
	}
	int size() {
		return mineField.size();
	}
	void insertBomb(const unsigned int x, const unsigned int y);
	void fill(const unsigned int amount) {
		default_random_engine RNG((unsigned int)chrono::system_clock::now().time_since_epoch().count());
		fill(RNG(), amount);
	}
	void Reset();
private:
	unsigned int xSize, ySize;
	vector<unsigned short> mineField;
	void increment(const unsigned int x, const unsigned int y);
	void fill(const unsigned int seed, const unsigned int amount);
};