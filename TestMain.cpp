#include "GraphicSize.h"

#include <iostream>

bool testFile(char* fileName)
{
	int x;
	int y;

	if (!getImageSize(fileName, &x, &y)) {
		std::cerr << "Unable to get image size for " << fileName << std::endl;
		return false;
	}
	if (x != 640 || y != 480) {
		std::cerr << "Incorrect image size for " << fileName << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	testFile("TestImage640480.jpg");
	testFile("TestImage640480.gif");
	testFile("TestImage640480.png");
}