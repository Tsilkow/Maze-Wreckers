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

enum ActionType {wait, move, dig, attack};

// TODO: this doesn't require a struct
struct Action
{
    ActionType type;
    int direction;
};
    
class Agent
{
    private:
    std::shared_ptr<AgentSettings> m_aSetts;
    std::shared_ptr<Region> m_world;
    std::string m_name;
    int m_allegiance;
    int m_type;
    sf::Vector2i m_coords;
    int m_direction;
    std::vector<sf::Vector2i> m_mask;
    sf::Vector2f m_position;
    sf::Sprite m_representation;
    std::vector<sf::Vertex> m_pathRepres;

    int m_cargo; // -1 is nothing
    ActionType m_currAction;
    int m_actionProgress;

    std::vector<int> m_path;
    sf::Vector2i m_destination;

    bool attack();
    
    bool move();

    bool dig();

    int unload();

    bool setPath();
    
    public:
    Agent(std::shared_ptr<AgentSettings>& aSetts, std::shared_ptr<Region>& world,
	ResourceHolder<sf::Texture, std::string>& textures, std::string name, int allegiance, int type,
	sf::Vector2i coords);
    
    bool moveTo(sf::Vector2i target, bool dig);
    
    bool tick();

    void draw(sf::RenderTarget& target);


    const int& getType() {return m_type; }
};
