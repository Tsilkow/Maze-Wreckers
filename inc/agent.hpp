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
#include "region.hpp"


struct AgentType
{
    int walkingSpeed;
    int diggingSpeed;
    int strength;
    std::string texture;
    int profileIndex;
};

struct AgentSettings
{
    std::vector<AgentType> agentTypes;
    std::vector<sf::Color> allColors; // allegiance colors
    int tileSize;
};

enum Action {wait, move, dig, attack};
    
class Agent
{
    private:
    std::shared_ptr<AgentSettings> m_aSetts;
    std::shared_ptr<Region> m_world;
    std::string m_name;
    int m_allegiance;
    int m_type;
    sf::Vector2i m_coords;
    Move m_direction;
    std::vector<sf::Vector2i> m_mask;
    sf::Vector2f m_position;
    sf::Sprite m_representation;
    std::vector<sf::Vertex> m_pathRepres;

    int m_cargo; // -1 is nothing
    Action m_currAction;
    int m_actionProgress;

    std::vector<Move> m_path;
    sf::Vector2i m_destination;

    bool attack();
    
    bool move();

    bool dig();

    int unload();
    
    public:
    Agent(std::shared_ptr<AgentSettings>& aSetts, std::shared_ptr<Region>& world,
	ResourceHolder<sf::Texture, std::string>& textures, std::string name, int allegiance, int type,
	sf::Vector2i coords);
    
    bool setDestination(sf::Vector2i destination);

    bool manifest();
    
    bool tick();

    void draw(sf::RenderTarget& target);


    std::string getId();

    const int& getType() {return m_type; }

    const sf::Vector2i& getCoords() {return m_coords; }

    const std::vector<sf::Vector2i>& getMask() {return m_mask; }
};
