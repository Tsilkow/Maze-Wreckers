#include "ant.hpp"


Ant::Ant(std::shared_ptr<AntSettings>& aSetts, std::shared_ptr<Region>& world,
	ResourceHolder<sf::Texture, std::string>& textures, std::string name, int allegiance, AntType type,
	sf::Vector2i coords):
    m_aSetts(aSetts),
    m_world(world),
    m_name(name),
    m_allegiance(allegiance),
    m_type(type),
    m_coords(coords),
    m_direction(0),
    m_storage(3, 0),
    m_currAction(ActionType::wait),
    m_actionProgress(0)
{
    m_storageLeft = m_aSetts->capacity;
    m_mask.push_back(m_coords);
    
    switch(m_type)
    {
	case worker: m_representation.setTexture(textures.get("worker")); break;
	case soldier: m_representation.setTexture(textures.get("soldier")); break;
    }
    sf::Vector2f size = sf::Vector2f(m_representation.getTexture()->getSize());
    m_representation.setOrigin(size * 0.5f);
    m_representation.setColor(m_aSetts->allColors[m_allegiance]);
    m_representation.setScale(sf::Vector2f(1.f, 1.f) * (float)m_aSetts->tileSize / size.x);
    m_position = (sf::Vector2f(m_coords) + sf::Vector2f(0.5f, 0.5f)) * (float)m_aSetts->tileSize;
    m_representation.setPosition(m_position);
}

bool Ant::attack()
{
    return true;
}

bool Ant::move()
{
    ++m_actionProgress;

    m_position = m_position + sf::Vector2f(getMove(m_direction) *
					   (int)std::round((float)m_aSetts->tileSize / m_aSetts->walkingSpeed));
    m_representation.setPosition(m_position);
    
    if(m_actionProgress >= m_aSetts->walkingSpeed)
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

bool Ant::dig()
{
    ++m_actionProgress;

    if(m_actionProgress % 5 == 0)
    {
	m_representation.setPosition(m_position + sf::Vector2f(getMove(m_direction)));
    }
    else m_representation.setPosition(m_position);
    
    if(m_actionProgress >= m_aSetts->diggingSpeed)
    {
	std::vector<int> load = m_world->digOut(m_coords + getMove(m_direction), m_storageLeft);
	int amount = load[0] + load[1] + load[2];

	for(int i = 0; i < 3; ++i)
	{
	    m_storage[i] += load[i];
	}
	m_storageLeft -= amount;
	m_actionProgress = 0;
	return true;
    }
    
    return false;
}

std::vector<int> Ant::unload()
{
    std::vector<int> result = {0, 0, 0};
    
    if(m_world->getTile(m_coords).type == 2)
    {
	result = m_storage;
	m_storage = {0, 0, 0};
	m_storageLeft = m_aSetts->capacity;
    }
    return result;
}

bool Ant::moveTo(sf::Vector2i target, bool dig)
{
    int toDig = m_storageLeft;
    if(dig) toDig = 0;

    std::vector<int> temp = m_world->findPath(m_coords, -1, target, m_aSetts->walkingSpeed,
					      m_aSetts->diggingSpeed, toDig);

    if(temp.size() > 0)
    {
	m_path = temp;
	return true;
    }
    return false;
}

bool Ant::tick()
{
    /*
    std::cout << "{ ";
    for(int i = 0; i < m_path.size(); ++i)
    {
	std::cout << m_path[i] << " ";
    }
    std::cout << "} \n";
    */
    
    sf::Vector2i nest = m_world->getNestAt(m_allegiance, m_coords, 0);

    // if ant is in a nest, unload
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

	// if you have to dig, but you have no storage, look for a way to the closest nest
	if(m_path.size() > 0)
	{
	    if(m_world->isDiggable(m_coords + getMove(m_path[0])) && m_storageLeft == 0)
	    {
		// TODO: compare heurestic to all alligned nests
		if(nest == sf::Vector2i(-1, -1) || !moveTo(nest, false))
		{
		    m_currAction = ActionType::wait;
		    m_destination = m_coords;
		    m_path.clear();
		}
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

void Ant::draw(sf::RenderTarget& target)
{
    target.draw(m_representation);
    target.draw(&m_pathRepres[0], m_pathRepres.size(), sf::LineStrip);
}
