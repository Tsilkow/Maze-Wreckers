#include "simulation.hpp"


Simulation::Simulation(std::shared_ptr<RegionSettings>& rSetts, std::shared_ptr<AgentSettings>& aSetts,
		       ResourceHolder<sf::Texture, std::string>& textures, Test test):
    m_rSetts(rSetts),
    m_aSetts(aSetts),
    m_test(test),
    m_region(rSetts, textures, test.genMaze),
    m_ticks(0)
{
    m_shr_region = std::make_shared<Region>(m_region);

    for(int i = 0; i < m_test.agents.size(); ++i)
    {
	m_shr_region->digOut(std::get<0>(m_test.agents[i]));
	m_shr_region->digOut(std::get<1>(m_test.agents[i]));
	m_agents.emplace_back(m_aSetts, m_shr_region, textures, "auntie"+std::to_string(i), 0,
			      std::get<2>(m_test.agents[i]), std::get<0>(m_test.agents[i]));
	m_agents.back().setDestination(std::get<1>(m_test.agents[i]));
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

    for(int i = 0; i < m_agents.size(); ++i)
    {
	if(m_agents[i].getCoords() != std::get<1>(m_test.agents[i])) break;
	if(i == m_agents.size()-1) return false;
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
