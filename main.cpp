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
#include "ant.hpp"
#include "simulation.hpp"


using namespace std;

int main()
{
    srand(time(NULL));

    RegionSettings rSetts =
    {
	sf::Vector2f(50, 50), // dimensions
	16,                   // tileSize
	16,                   // texTileSize
	128,                  // colorPerTile
	4,                    // nestTotal
	1000,                  // genCenTotal
	10,                   // genCenReach
	{3, 3, 3},            // colorGenWghts
	{1, 0, 1},            // walkable
	{0, 1, 0},           // diggable
	{                     // agentProfiles
	    {1, 16, 0},       // agentNoDigging
	    {1, 16, 64},       // agentNormal
	    {1, 24, 32}       // agentHeavy
	}
    };
    shared_ptr<RegionSettings> shr_rSetts = make_shared<RegionSettings>(rSetts);

    // TODO: digging/walking ratio must be constant, in order to be able to calculate univesral abstract distances
    AntSettings aSetts =
    {
	128,
	{sf::Color(255, 255, 255)},
	16,
	16,
	64
    };
    shared_ptr<AntSettings> shr_aSetts = make_shared<AntSettings>(aSetts);

    ResourceHolder<sf::Texture, std::string> textures;
    textures.load("tileset", "data/tileset.png");
    textures.load("worker", "data/worker.png");
    textures.load("soldier", "data/soldier.png");

    sf::RenderWindow window(sf::VideoMode(850, 800), "Color Quarry");
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
