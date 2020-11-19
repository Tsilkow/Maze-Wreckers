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

struct RegionSettings
{
    sf::Vector2f dimensions;
    int tileSize;
    int texTileSize;
    int colorPerTile;
    int nestTotal;
    int genCenTotal; // generation centers total
    int genCenReach; // generation center reach
    std::vector<int> colorGenWghts; // color generation weights: r = 0, g = 1, b = 2
    std::vector<int> walkable;
    std::vector<int> diggable;
    std::vector<AgentProfile> agentProfiles;
};

enum TileType {open=0, wall=1, nest=2};

struct Vector2iComparator
{
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b);
};

struct Tile
{
    TileType type;
    int r;
    int g;
    int b;
    std::vector< std::map<sf::Vector2i, int, Vector2iComparator> > naiveDistance; // naive distance means a distance assuming no other agents are in the way
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
    int d; // diggingLeft
    int h; // heurestic
    
    PathCoord(sf::Vector2i coords, int time, int diggingLeft, int heurestic):
	x(coords.x), y(coords.y), t(time), d(diggingLeft), h(heurestic)
	{;}
    
    PathCoord(sf::Vector2i coords, int time, int diggingLeft):
	x(coords.x), y(coords.y), t(time), d(diggingLeft), h(1000000)
	{;}

    void move(int direction)
	{
	    sf::Vector2i difference = getMove(direction);
	    x += difference.x;
	    y += difference.y;
	}

    void print () const
	{
	    std::cout << "{ ";
	    std::cout << "x=" << x << " | ";
	    std::cout << "y=" << y << " | ";
	    std::cout << "t=" << t << " | ";
	    std::cout << "d=" << d << " | ";
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
    std::vector< std::vector<Tile> > m_data;
    std::vector< std::vector<sf::Vector2i> > m_nests;
    std::vector<sf::Vector2i> m_targets;
    //std::vector< std::map<sf::Vector2i, int, Vector2iComparator> > m_nestDomains;
    std::vector<sf::Vertex> m_representation;
    sf::RenderStates m_states;
    int m_ticks;
    
    // stores whether tile at pos 'x' and 'y' and from tick from' to tick 'to'; the value is -1 is it is undiggable, otherwise it's the amount to dig out
    std::map<Reservation, int, ReservationComparator> m_reservations;
    std::multimap<int, Reservation> m_toCleanAt;

    std::vector<sf::Vector2i> m_toUpdate;
    
    sf::Color getTileColor(sf::Vector2i coords);
        
    sf::Color getTileColor(int x, int y) {return getTileColor(sf::Vector2i(x, y)); };
    
    void generate();

    void update();
    
    std::pair<bool, int> isReserved(int x, int y, int from, int to);
	
    std::pair<bool, int> isReserved(sf::Vector2i coords, int from, int to);

    void reserve(sf::Vector2i coords, int from, int to);
    
    int evalByHeurestic(PathCoord path, sf::Vector2i target);

    void calcNaiveDistance(sf::Vector2i from, sf::Vector2i startAt = sf::Vector2i(-1, -1));

    public:
    Region(std::shared_ptr<RegionSettings>& rSetts, ResourceHolder<sf::Texture, std::string>& textures);

    std::vector<int> digOut(sf::Vector2i coords, int amount);

    std::vector<int> findPath
    (sf::Vector2i start, int time, sf::Vector2i target, int walkingSpeed, int diggingSpeed, int ableToDig,
     bool isReserving = true, bool infDigging = false);
    
    /*std::pair<std::vector<int>, int> findPath
    (sf::Vector2i start, int time, std::vector<sf::Vector2i> target, int walkingSpeed, int diggingSpeed,
    int ableToDig, bool reserve = true);*/
    
    bool tick(int ticksPassed);

    void draw(sf::RenderTarget& target);


    const Tile& getTile(sf::Vector2i coords) {return atCoords(m_data, coords); }

    sf::Vector2i getNestAt(int allegiance, sf::Vector2i coords, int profileIndex);
    
    bool inBounds(sf::Vector2i coords);
    
    bool isWalkable(sf::Vector2i coords);

    bool isDiggable(sf::Vector2i coords);
};
