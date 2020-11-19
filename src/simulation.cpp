#include "simulation.hpp"


Simulation::Simulation(std::shared_ptr<RegionSettings>& rSetts, std::shared_ptr<AntSettings>& aSetts,
	       ResourceHolder<sf::Texture, std::string>& textures):
    m_rSetts(rSetts),
    m_aSetts(aSetts),
    m_region(rSetts, textures),
    m_ticks(0)
{
    m_shr_region = std::make_shared<Region>(m_region);
    
    m_ants.emplace_back(m_aSetts, m_shr_region, textures, "auntie", 0, AntType::worker, sf::Vector2i(12, 25));
    m_ants.back().moveTo(sf::Vector2i(0, 0), true);
    m_ants.emplace_back(m_aSetts, m_shr_region, textures, "auntie", 0, AntType::worker, sf::Vector2i(25, 12));
    m_ants.back().moveTo(sf::Vector2i(1, 0), true);
    m_ants.emplace_back(m_aSetts, m_shr_region, textures, "auntie", 0, AntType::worker, sf::Vector2i(37, 25));
    m_ants.back().moveTo(sf::Vector2i(2, 0), true);
    m_ants.emplace_back(m_aSetts, m_shr_region, textures, "auntie", 0, AntType::worker, sf::Vector2i(25, 37));
    m_ants.back().moveTo(sf::Vector2i(3, 0), true);
}

bool Simulation::tick()
{
    ++m_ticks;
    
    m_shr_region->tick(m_ticks);
    for(int i = 0; i < m_ants.size(); ++i)
    {
	m_ants[i].tick();
    }
    std::cout << m_ticks << std::endl;

    return true;
}

void Simulation::draw(sf::RenderTarget& target)
{
    m_shr_region->draw(target);
    for(int i = 0; i < m_ants.size(); ++i)
    {
	m_ants[i].draw(target);
    }
}
