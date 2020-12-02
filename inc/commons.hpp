#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

template <typename T>
T& atCoords(std::vector< std::vector<T> >& data, sf::Vector2i coords)
{
    return data[coords.x][coords.y];
}

enum Move {north = 0, west = 1, south = 2, east = 3, stay = 4};
// if a move is not the waiting one and is out of range, it is expected to be modulo-d by 4

int getMoveTotal();

int getDirectionTotal(); // total of moves without waiting

sf::Vector2i getMove(Move direction);

sf::Vector2i getMove(int direction);

Move reverseDirection(Move direction);

int modulo(int a, int b);

float modulo(float a, float b);

int randomI(int min, int max);

int randomIWeights(std::vector<int> weights);

std::vector<int> RandomSequence(int min, int max, int length);

int distance(sf::Vector2i);

float determineLightness(sf::Color input);
    
sf::Color randomColor(std::vector<float> lightRange);

std::string trailingZeroes(float number, int zeroes = 2);

std::vector<sf::Color> colorGradient(sf::Color start, sf::Color end, int stepTotal);

std::vector<sf::Color> generatePalette(int colorTotal);

void printVector(sf::Vector2i a, bool enter=true);

void printVector(sf::Vector3i a, bool enter=true);

void printVector(sf::Vector2f a, bool enter=true);

sf::Vector2f alongSquare(float point);
