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

int getMoveTotal();

int getDirectionTotal();

sf::Vector2i getMove(int direction);

int reverseDirection(int direction);

int modulo(int a, int b);

float modulo(float a, float b);

int randomI(int min, int max);

int randomIWeights(std::vector<int> weights);

int distance(sf::Vector2i);

float determineLightness(sf::Color input);
    
sf::Color randomColor(std::vector<float> lightRange);

std::string trailingZeroes(float number, int zeroes = 2);

std::vector<sf::Color> colorGradient(sf::Color start, sf::Color end, int stepTotal);

std::vector<sf::Color> generatePalette(int colorTotal);

void printVector(sf::Vector2i a, bool enter);

void printVector(sf::Vector3i a, bool enter);

void printVector(sf::Vector2f a, bool enter);

sf::Vector2f alongSquare(float point);
