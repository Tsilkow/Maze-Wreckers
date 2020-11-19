// Tsilkow

#include <iostream>
#include <vector>
#include <time.h>
#include <memory>
#include <math.h>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "commons.hpp"
#include "resources.hpp"
#include "region.hpp"
#include "agent.hpp"
#include "simulation.hpp"


using namespace std;

int main()
{
    srand(time(NULL));

    TileType open    = {true , false, false, sf::Color(  0,   0,   0), 0};
    TileType wall    = {false, true , false, sf::Color(  0,   0, 128), 1};
    TileType nest    = {true , false, false, sf::Color(255, 255, 255), 2};
    TileType terrace = {true , false, true , sf::Color(  0, 255, 255), 3};

    RegionSettings rSetts =
    {
	sf::Vector2f(50, 50),        // dimensions
	16,                          // tileSize
	16,                          // texTileSize
	4,                           // nestTotal
	{                            // tileTypes
	    {0, open},
	    {1, wall},
	    {10, nest},
	    {2, terrace}}, 
	{                            // agentProfiles
	    {1, 16, 0},              // agentNoDigging
	    {1, 16, 64},             // agentNormal
	    {1, 24, 32}              // agentHeavy
	}
    };
    shared_ptr<RegionSettings> shr_rSetts = make_shared<RegionSettings>(rSetts);

    // TODO: digging/walking ratio must be constant, in order to be able to calculate univesral abstract distances
    AgentType digger  = {16, 64,  0, "worker", 1};
    AgentType warrior = {16,  0, 10, "soldier", 0};
    
    AgentSettings aSetts =
    {
	{warrior, digger},          // agentTypes
	{sf::Color(255, 255, 255)}, // allColors
	rSetts.tileSize             // tileSize
    };
    shared_ptr<AgentSettings> shr_aSetts = make_shared<AgentSettings>(aSetts);

    ResourceHolder<sf::Texture, std::string> textures;
    textures.load("tileset", "data/tileset.png");
    textures.load("worker", "data/worker.png");
    textures.load("soldier", "data/soldier.png");

    sf::RenderWindow window(sf::VideoMode(850, 800), "Maze Wreckers");
    window.setFramerateLimit(60);
    
    sf::View actionView(sf::Vector2f(425.f, 400.f), sf::Vector2f(850, 800));
    window.setView(actionView);

    Simulation simulation(shr_rSetts, shr_aSetts, textures);

    enum GameState{Menu, Play, Scores};
    GameState currState = GameState::Play;
    bool hasFocus = true;
    int ticksPassed = 0;

    while(window.isOpen())
    {
	sf::Event event;
	std::pair<std::string, std::string> input;
	
	window.clear();
	
	while (window.pollEvent(event))
	{
	    switch(event.type)
	    {
		case sf::Event::Closed:
		    window.close();
		    break;
		case sf::Event::LostFocus:
		    hasFocus = false;
		    //std::cout << "LOST FOCUS" << std::endl;
		    break;
		case sf::Event::GainedFocus:
		    hasFocus = true;
		    //std::cout << "GAINED FOCUS" << std::endl;
		    break;
		case sf::Event::KeyPressed:
		    if(hasFocus)
		    {
			switch(event.key.code)
			{
			    case sf::Keyboard::Escape:
				window.close();
				break;
				
			    default: break;
			}
		    }
		    break;
		    
		default: break;
	    }

	    if(hasFocus)
	    {
		switch(currState)
		{
		    case GameState::Play:
			break;
			
		    default: break;
		}
	    }
	}
	
	switch(currState)
	{	
	    case GameState::Play:
		simulation.tick();
		simulation.draw(window);
		break;
	}

	++ticksPassed;
	
	window.display();
    }
    
    return 0;
}