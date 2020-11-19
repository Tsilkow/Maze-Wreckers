#include "agent.hpp"


Agent::Agent(std::shared_ptr<AgentSettings>& aSetts, std::shared_ptr<Region>& world,
	ResourceHolder<sf::Texture, std::string>& textures, std::string name, int allegiance, int type,
	sf::Vector2i coords):
    m_aSetts(aSetts),
    m_world(world),
    m_name(name),
    m_allegiance(allegiance),
    m_type(type),
    m_coords(coords),
    m_direction(0),
    m_cargo(-1),
    m_currAction(ActionType::wait),
    m_actionProgress(0)
{
    m_mask.push_back(m_coords);
    m_representation.setTexture(textures.get(m_aSetts->agentTypes[m_type].texture));
    
    sf::Vector2f size = sf::Vector2f(m_representation.getTexture()->getSize());
    m_representation.setOrigin(size * 0.5f);
    m_representation.setColor(m_aSetts->allColors[m_allegiance]);
    m_representation.setScale(sf::Vector2f(1.f, 1.f) * (float)m_aSetts->tileSize / size.x);
    m_position = (sf::Vector2f(m_coords) + sf::Vector2f(0.5f, 0.5f)) * (float)m_aSetts->tileSize;
    m_representation.setPosition(m_position);
}

bool Agent::attack()
{
    return true;
}

bool Agent::move()
{
    ++m_actionProgress;

    m_position = m_position + sf::Vector2f(getMove(m_direction) *
					   (int)std::round((float)m_aSetts->tileSize /
							   m_aSetts->agentTypes[m_type].walkingSpeed));
    m_representation.setPosition(m_position);
    
    if(m_actionProgress >= m_aSetts->agentTypes[m_type].walkingSpeed)
    {
	m_coords += getMove(m_direction);
	m_position = (sf::Vector2f(m_coords) + sf::Vector2f(0.5f, 0.5f)) * (float)m_aSetts->tileSize;
	m_mask.clear();
	m_mask.emplace_back(m_coords);
	
	m_actionProgress = 0;
	return true;
    }
    else if(m_mask.size() < 2) m_mask.emplace_back(m_coords + getMove(m_direction));
    
    return false;
}

bool Agent::dig()
{
    ++m_actionProgress;

    if(m_actionProgress % 5 == 0)
    {
	m_representation.setPosition(m_position + sf::Vector2f(getMove(m_direction)));
    }
    else m_representation.setPosition(m_position);
    
    if(m_actionProgress >= m_aSetts->agentTypes[m_type].diggingSpeed)
    {
	return m_world->digOut(m_coords + getMove(m_direction));
    }
    
    return false;
}

int Agent::unload()
{
    int temp = m_cargo;
    m_cargo = -1;
    return temp;
}

bool Agent::moveTo(sf::Vector2i target, bool dig)
{
    std::vector<int> temp = m_world->findPath(m_coords, -1, target, m_aSetts->agentTypes[m_type].profileIndex);

    if(temp.size() > 0)
    {
	m_path = temp;
	return true;
    }
    return false;
}

bool Agent::tick()
{
    /*
    std::cout << "{ ";
    for(int i = 0; i < m_path.size(); ++i)
    {
	std::cout << m_path[i] << " ";
    }
    std::cout << "} \n";
    */
    
    sf::Vector2i nest = m_world->getClosestNest(m_allegiance, m_coords,
						m_aSetts->agentTypes[m_type].profileIndex);

    // if agent is in a nest, unload
    if(m_coords == nest)
    {
	unload();
    }

    // if action is finished
    if(m_actionProgress == 0)
    {
	// if you have no path, find one to destination; if you can't, your destination is now here
	if(m_path.size() == 0 && m_destination != m_coords)
	{
	    if(!moveTo(m_destination, true))
	    {
		std::cout << "lost destination\n";
		m_destination = m_coords;
	    }
	}

	// path processing
	if(m_path.size() > 0)
	{    
	    m_pathRepres.clear();
	    // drawing path
	    sf::Vector2i temp = m_coords;
	    m_pathRepres.emplace_back(m_position, sf::Color::White);
	    for(int i = 0; i < m_path.size(); ++i)
	    {
		temp += getMove(m_path[i]);
		m_pathRepres.emplace_back((sf::Vector2f(temp) + sf::Vector2f(0.5f, 0.5f)) *
					  (float)m_aSetts->tileSize, sf::Color::White);
	    }

	    if(m_path[0] == 4)
	    {
		m_currAction = ActionType::wait;
	    }
	    else
	    {
		m_direction = m_path[0];
		m_representation.setRotation(m_direction * 90.f);

		// choose action based on next tile 
		if     (m_world->isWalkable(m_coords + getMove(m_direction)))
		{
		    m_currAction = ActionType::move;
		    m_path.erase(m_path.begin());
		}
		else if(m_world->isDiggable(m_coords + getMove(m_direction))) m_currAction = ActionType::dig;
		else // if you can't, discard the path (attempt to find a new way will be in the next tick)
		{
		    m_currAction = ActionType::wait;
		    m_path.clear();
		}
	    }
	}
	else // if no path
	{
	    m_currAction = ActionType::wait;
	}
    }
    
    m_pathRepres[0].position = m_position;
    
    switch(m_currAction)
    {
	case ActionType::attack: attack(); break;
	case ActionType::move:   move();   break;
	case ActionType::dig:    dig();    break;
	case ActionType::wait:   ; break;
    }

    return true;
}

void Agent::draw(sf::RenderTarget& target)
{
    target.draw(m_representation);
    target.draw(&m_pathRepres[0], m_pathRepres.size(), sf::LineStrip);
}
