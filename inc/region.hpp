#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "commons.hpp"
#include "resources.hpp"


struct AgentProfile
{
    int waiting;
    int walking;
    int digging;
};

struct TileType
{
    bool isWalkable;
    bool isDiggable;
    bool isYankable;
    sf::Color normColor;
    int textureIndex;
};

struct RegionSettings
{
    sf::Vector2f dimensions;
    int tileSize;
    int texTileSize;
    int nestTotal;
    std::map<int, TileType> tileTypes;
    // 0 is open
    // 1 is wall
    // 2 is flower
    // 10 is nest
    std::vector<AgentProfile> agentProfiles;
    int pathWindowSize;
};

struct Vector2iComparator
{
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b);
};

struct Reservation
{
    int x;
    int y;
    int from;
    int to;
};

struct PathCoord
{
    int x;
    int y;
    int t; // time
    int h; // heurestic
    
    PathCoord(sf::Vector2i coords, int time, int heurestic):
	x(coords.x), y(coords.y), t(time), h(heurestic)
	{;}
    
    PathCoord(sf::Vector2i coords, int time):
	x(coords.x), y(coords.y), t(time), h(1000000)
	{;}

    void move(int direction)
	{
	    sf::Vector2i difference = getMove(direction);
	    
	    x += difference.x;
	    y += difference.y;
	}
/*
    int move(int direction, std::vector<int> profile, bool digging=false)
	{
	    sf::Vector2i difference = getMove(direction);
	    int result = 0;
	    x += difference.x;
	    y += difference.y;

	    if(direction == 4) result = profile[0];
	    else if(digging) result = profile[1] + profile[2];
	    else result = profile[1];
	    t += result;

	    return result;
	}*/

    void print () const
	{
	    std::cout << "{ ";
	    std::cout << "x=" << x << " | ";
	    std::cout << "y=" << y << " | ";
	    std::cout << "t=" << t << " | ";
	    std::cout << "h=" << h << " }\n";
	}

    const sf::Vector2i coords() const {return sf::Vector2i(x, y); }
    const sf::Vector3i toords() const {return sf::Vector3i(x, y, t); }
};

struct PathCoordComparator
{
    bool operator() (const PathCoord& a, const PathCoord& b);
};

struct PathHeuresticComparator
{
    bool operator() (const PathCoord& a, const PathCoord& b);
};

struct ReservationComparator
{
    bool operator()(const Reservation& a, const Reservation& b);
};

class Region
{
    private:
    std::shared_ptr<RegionSettings> m_rSetts;
    std::vector< std::vector<int> > m_data;
    std::vector< std::vector<sf::Vector2i> > m_nests;
    std::vector<sf::Vector2i> m_targets;
    //std::vector< std::map<sf::Vector2i, int, Vector2iComparator> > m_nestDomains;
    std::vector<sf::Vertex> m_representation;
    sf::RenderStates m_states;
    int m_ticks;
    
    // stores whether tile at pos 'x' and 'y' and from tick from' to tick 'to'; the value is -1 if it is undiggable, otherwise it's 1
    std::map<Reservation, int, ReservationComparator> m_reservations;
    std::vector< std::vector< std::vector< std::map<sf::Vector2i, int, Vector2iComparator> > > >
    m_naiveDistance;
    std::multimap<int, Reservation> m_toCleanAt;

    std::vector<sf::Vector2i> m_toUpdate;
    
    void generate();

    void update();

    int isReserved(int x, int y, int from, int to);
	
    int isReserved(sf::Vector2i coords, int from, int to);

    void reserve(sf::Vector2i coords, int from, int to);
    
    bool dereserve(sf::Vector2i coords, int freeAt);

    void calcNaiveDistance(sf::Vector2i from, sf::Vector2i startAt = sf::Vector2i(-1, -1));
    
    int getHeurestic(int profileIndex, sf::Vector2i at, sf::Vector2i to, int time);

    public:
    Region(std::shared_ptr<RegionSettings>& rSetts, ResourceHolder<sf::Texture, std::string>& textures);

    bool digOut(sf::Vector2i coords);

    std::vector<int> findPath
    (sf::Vector2i start, int time, sf::Vector2i target, int profileIndex);
    
    std::vector<int> findPath(sf::Vector2i start, int time, std::vector<sf::Vector2i> target,
			      int profileIndex);
    
    bool tick(int ticksPassed);

    void draw(sf::RenderTarget& target);


    sf::Vector2i getClosestNest(int allegiance, sf::Vector2i coords, int profileIndex);
    
    bool inBounds(sf::Vector2i coords);
    
    bool isWalkable(sf::Vector2i coords);

    bool isDiggable(sf::Vector2i coords);

    bool isYankable(sf::Vector2i coords);
};
