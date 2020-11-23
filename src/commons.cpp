#include "commons.hpp"


#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <math.h>


int getMoveTotal() {return 5; }

int getDirectionTotal() {return 4; }

sf::Vector2i getMove(Move direction)
{
    if(direction != Move::stay) direction = static_cast<Move>(modulo(direction, 4));
    
    switch(direction)
    {
	case north: return sf::Vector2i( 0, -1); break;
	case west : return sf::Vector2i( 1,  0); break;
	case south: return sf::Vector2i( 0,  1); break;
	case east : return sf::Vector2i(-1,  0); break;
	case stay : return sf::Vector2i( 0,  0); break;
    }
}

sf::Vector2i getMove(int direction)
{
    return getMove(static_cast<Move>(direction));
}

Move reverseDirection(Move direction)
{
    if(direction == 4) return direction;
    return static_cast<Move>(modulo(direction - 2, 4));
}

int modulo(int a, int b)
{
    while(a <  0) a += b;
    while(a >= b) a -=b;

    return a;
}

float modulo(float a, float b)
{
    while(a <  0) a += b;
    while(a >= b) a -= b;

    return a;
}

int randomI(int min, int max)
{
    if(min == max) return min;
    return rand() % (max - min + 1) + min;
}

int randomIWeights(std::vector<int> weights)
{
    int variable;
    int sum = 0;
	
    for(int i = 0; i < weights.size(); ++i)
    {
	sum += weights[i];
    }
    variable = randomI(0, sum-1);
	
    for(int i = 0; i < weights.size(); ++i)
    {
	if(variable < weights[i]) return i;
	variable -= weights[i];
    }

    return weights.size()-1; // just in case
}

int distance(sf::Vector2i relation)
{
    return std::abs(relation.x) + std::abs(relation.y);
}

float determineLightness(sf::Color input)
{
    // https://thoughtbot.com/blog/closer-look-color-lightness
    // values have been normalized (ie divided by 255)
    return ((float)input.r) * 0.00083372549 +
	   ((float)input.g) * 0.00280470588 +
	   ((float)input.b) * 0.00028313725;
}

sf::Color randomColor(std::vector<float> lightRange)
{
    sf::Color result(0, 0, 0);
    do{
	result.r = randomI(0, 255);
	result.g = randomI(0, 255);
	result.b = randomI(0, 255);
    }while(determineLightness(result) < lightRange[0] || determineLightness(result) > lightRange[1]);
    
    return result;
}

std::string trailingZeroes(float number, int zeroes)
{
    std::ostringstream out;
    out << std::setprecision(zeroes) << std::fixed << number;
    return out.str();
}

std::vector<sf::Color> colorGradient(sf::Color start, sf::Color end, int stepTotal)
{
    std::vector<sf::Color> result;

    result.emplace_back(start);
    
    for(int i = 1; i < stepTotal-1; ++i)
    {
	int r = start.r + ((int)std::round(i * ((float)end.r - (float)start.r) / ((float)stepTotal-1)));
	int g = start.g + ((int)std::round(i * ((float)end.g - (float)start.g) / ((float)stepTotal-1)));
	int b = start.b + ((int)std::round(i * ((float)end.b - (float)start.b) / ((float)stepTotal-1)));

	result.emplace_back(r, g, b);
    }
    
    result.emplace_back(end);

    return result;
    
}

float colorValue(float point)
{
    point = modulo(point, 1.f);
    
    if(point <= 1.f/6.f) return point*6.f;
    if(point <= 3.f/6.f) return 1.f;
    if(point <= 4.f/6.f) return (4.f/6.f - point)*6.f;
    return 0.f;
}

sf::Color colorFromRange(float point)
{
    return sf::Color(std::round(255.f * colorValue(point + 2.f/6.f)),
		     std::round(255.f * colorValue(point          )),
		     std::round(255.f * colorValue(point - 2.f/6.f)));
}

std::vector<sf::Color> generatePalette(int colorTotal)
{
    std::vector<sf::Color> result;

    for(int i = 0; i < colorTotal; ++i)
    {
	result.emplace_back(colorFromRange((float)i/(float)colorTotal));
	std::cout << i << " = ("
		  << (int)result.back().r << ", "
		  << (int)result.back().g << ", "
		  << (int)result.back().b << ")\n";
    }

    return result;
}

void printVector(sf::Vector2i a, bool enter)
{
    std::cout << "{" << a.x << ", " << a.y << "}";
    if(enter) std::cout << std::endl;
}

void printVector(sf::Vector3i a, bool enter)
{
    std::cout << "{" << a.x << ", " << a.y << ", " << a.z << "}";
    if(enter) std::cout << std::endl;
}

void printVector(sf::Vector2f a, bool enter)
{
    std::cout << "{" << a.x << ", " << a.y << "}";
    if(enter) std::cout << std::endl;
}

sf::Vector2f squareTurn(float point)
{
    if(point != 1.f) point = modulo(point, 1.f);
    std::cout << point;
    
    if(point <= 1.f/2.f) return sf::Vector2f(point * 2.f, -1.f);
    else return sf::Vector2f(1.f, -2.f * (1.f - point));
}

sf::Vector2f alongSquare(float point)
{
    sf::Vector2f result;
    std::cout << std::setw(10) << point << " = ";
    
    if     (point < 0.25f)
    {
	std::cout << "1 (";
	result = squareTurn(4.f * point);
    }
    else if(point < 0.5f )
    {
	std::cout << "2 (";
	point -= 0.25f;
	result = squareTurn(1.f - 4.f * point);
	result.y = -result.y;
    }
    else if(point < 0.75f )
    {
	std::cout << "3 (";
	point -= 0.5f;
	result = squareTurn(4.f * point);
	result = -result;
    }
    else
    {
	std::cout << "4 (";
	point -= 0.75f;
	result = squareTurn(1.f - 4.f * point);
	result.x = -result.x;
    }
    
    std::cout << ") ";

    return result;
}
