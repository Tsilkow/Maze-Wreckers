#pragma once

#include <iostream>
#include <vector>

#include "commons.hpp"
#include "region.hpp"
#include "ant.hpp"
#include "command.hpp"


class Simulation
{
    private:
    std::shared_ptr<RegionSettings> m_rSetts;
    std::shared_ptr<AntSettings> m_aSetts;
    Region m_region;
    std::shared_ptr<Region> m_shr_region;
    std::vector<Ant> m_ants;
    std::vector<Command> m_commands;

    int m_ticks;
    
    public:
    Simulation(std::shared_ptr<RegionSettings>& rSetts, std::shared_ptr<AntSettings>& aSetts,
	       ResourceHolder<sf::Texture, std::string>& textures);

    bool tick();
    
    void draw(sf::RenderTarget& target);
};
