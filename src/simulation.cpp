#include "simulation.hpp"


Simulation::Simulation(std::shared_ptr<RegionSettings>& rSetts, std::shared_ptr<AgentSettings>& aSetts,
		       ResourceHolder<sf::Texture, std::string>& textures,
		       std::vector<std::pair<sf::Vector2i, sf::Vector2i>> test):
    m_rSetts(rSetts),
    m_aSetts(aSetts),
    m_region(rSetts, textures),
    m_ticks(0),
    m_test(test)
{
    m_shr_region = std::make_shared<Region>(m_region);

    for(int i = 0; i < m_test.size(); ++i)
    {
	m_shr_region->digOut(m_test[i].first);
	m_agents.emplace_back(m_aSetts, m_shr_region, textures, "auntie"+std::to_string(i), 0, 0,
			      m_test[i].first);
	m_agents.back().setDestination(m_test[i].second);
    }
}

bool Simulation::tick()
{
    ++m_ticks;
    
    for(int i = 0; i < m_agents.size(); ++i)
    {
	m_agents[i].manifest();
    }

    m_shr_region->commitPaths();
    
    for(int i = 0; i < m_agents.size(); ++i)
    {
	m_agents[i].tick();
    }
    
    m_shr_region->tick(m_ticks);

    for(int i = 0; i < m_test.size(); ++i)
    {
	if(m_agents[i].getCoords() != m_test[i].second) break;
	if(i == m_test.size()-1) return false;
    }
    //std::cout << m_ticks << std::endl;

    return true;
}

void Simulation::draw(sf::RenderTarget& target)
{
    m_shr_region->draw(target);
    for(int i = 0; i < m_agents.size(); ++i)
    {
	m_agents[i].draw(target);
    }
}
