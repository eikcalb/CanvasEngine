#include "Colour.h"

#include <exception>
#include <sstream>

using namespace std;

/*******************************************************************/
// Structors
/*******************************************************************/

Colour::Colour()
{
	set(1,1,1,1);
}

/*******************************************************************/

Colour::Colour(float r, float g, float b, float a)
{
	set(r,g,b,a);
}

/*******************************************************************/

Colour::Colour(std::string rgba)
{
		std::stringstream ss(rgba);
		std::string token;
		int index = 0;
		float result[3]{};

		while (std::getline(ss, token, ',')) {
			if (index >= 3) {
				// More components than expected
				break;
			}
			result[index++] = std::stof(token);
		}

		if (index != 3) {
			// Not enough components in the string
			throw std::invalid_argument("Invalid input string for glm::vec3");
		}

		Colour(result[0], result[1], result[2], 1.0f);
}
/*******************************************************************/

// This constructor accepts float arrays of 2, 3, or 4 floats
Colour::Colour(float values[])
{
	set(values);
}

/*******************************************************************/

Colour::~Colour()
{
}

/*******************************************************************/

// Copy constructor
Colour::Colour(const Vector4& toCopy)
{
	set(toCopy);
}

/*******************************************************************/

// Assignment operator
Colour& Colour::operator=(const Colour& toCopy)
{
	if (this == &toCopy) return *this;
	set(toCopy);

	return *this;
}

/*******************************************************************/
// Functions
/*******************************************************************/

Colour Colour::Red()
{
	return Colour(1,0,0);
}

Colour Colour::Blue()
{
	return Colour(0,0,1);
}

Colour Colour::Green()
{
	return Colour(0,1,0);
}

Colour Colour::Yellow()
{
	return Colour(1,1,0);
}

Colour Colour::Black()
{
	return Colour(0,0,0);
}

Colour Colour::White()
{
	return Colour(1,1,1);
}

Colour Colour::Grey()
{
	return Colour(0.5,0.5,0.5);
}

Colour Colour::Null()
{
	return Colour(0,0,0,0);
}

const std::unordered_map<std::string, Colour> Colour::ColourMap = {
	{"red", Colour::Red()},
	{"blue", Colour::Blue()},
	{"green", Colour::Green()},
	{"yellow", Colour::Yellow()},
	{"white", Colour::White()},
	{"grey", Colour::Grey()},
	{"magenta", Colour(1, 0, 1)},
	{"orange",  Colour(1, 0.5, 0)},
	{"purple",Colour(0.5, 0, 0.5)},
};
