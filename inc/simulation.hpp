#pragma once

#include <iostream>
#include <vector>

#include "commons.hpp"
#include "region.hpp"
#include "agent.hpp"
#include "command.hpp"


class Simulation
{
    private:
    std::shared_ptr<RegionSettings> m_rSetts;
    std::shared_ptr<AgentSettings> m_aSetts;
    Region m_region;
    std::shared_ptr<Region> m_shr_region;
    std::vector<Agent> m_agents;
    std::vector<Command> m_commands;

    int m_ticks;
    
    public:
    Simulation(std::shared_ptr<RegionSettings>& rSetts, std::shared_ptr<AgentSettings>& aSetts,
	       ResourceHolder<sf::Texture, std::string>& textures);

    bool tick();
    
    void draw(sf::RenderTarget& target);
};