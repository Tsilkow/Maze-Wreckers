#include "region.hpp"


bool Vector2iComparator::operator()(const sf::Vector2i& a, const sf::Vector2i& b)
{
    if(a.x == b.x)
    {
	return (a.y < b.y);
    }
    else return (a.x < b.x);
}

bool ReservationComparator::operator()(const Reservation& a, const Reservation& b)
{
    if(a.x == b.x)
    {
	if(a.y == b.y)
	{
	    return (a.from < b.from);
	}
	else return (a.y < b.y);
    }
    else return (a.x < b.x);
}

bool PathCoordComparator::operator() (const PathCoord& a, const PathCoord& b)
{
    if(a.x == b.x)
    {
	if(a.y == b.y)
	{
	    return (a.t < b.t);
	}
	return (a.y < b.y);
    }
    return (a.x < b.x);
}

bool PathHeuresticComparator::operator() (const PathCoord& a, const PathCoord& b)
{
    if(a.h == b.h)
    {
	if(a.t == b.t)
	{
	    if(a.x == b.x)
	    {
		return (a.y < b.y);
	    }
	    return (a.x < b.x);
	}
	return (a.t < b.t);
    }
    return (a.h < b.h);
};

Region::Region(std::shared_ptr<RegionSettings>& rSetts,
	       ResourceHolder<sf::Texture, std::string>& textures):
    m_rSetts(rSetts),
    m_ticks(0)
{
    m_states.texture = &textures.get("tileset");
    
    generate();
    
    for(int x = 0; x < m_rSetts->dimensions.x; ++x)
    {
	for(int y = 0; y < m_rSetts->dimensions.y; ++y)
	{
	    std::vector<sf::Vector2f> position = {
		{(float) x    * m_rSetts->tileSize, (float) y    * m_rSetts->tileSize},
		{(float)(x+1) * m_rSetts->tileSize, (float) y    * m_rSetts->tileSize},
		{(float)(x+1) * m_rSetts->tileSize, (float)(y+1) * m_rSetts->tileSize},
		{(float) x    * m_rSetts->tileSize, (float)(y+1) * m_rSetts->tileSize}
	    };
	    int type = m_data[x][y];
	    std::vector<sf::Vector2f> texCoords = {
		{(float) m_rSetts->tileTypes[type].textureIndex * m_rSetts->texTileSize, 0.f},
		{(float)(m_rSetts->tileTypes[type].textureIndex+1) * m_rSetts->texTileSize, 0.f},
		{(float)(m_rSetts->tileTypes[type].textureIndex+1) * m_rSetts->texTileSize,
		 (float)m_rSetts->texTileSize},
		{(float) m_rSetts->tileTypes[type].textureIndex    * m_rSetts->texTileSize,
		 (float)m_rSetts->texTileSize}
	    };

	    for(int i = 0; i < 4; ++i)
	    {
		m_representation.emplace_back(position[i], m_rSetts->tileTypes[type].normColor, texCoords[i]);
	    }
	}
    }
    
    for(int i = 0; i < m_targets.size(); ++i)
    {
	calcNaiveDistance(m_targets[i], sf::Vector2i(-1, -1));
    }
}

void Region::generate()
{
    m_data = std::vector< std::vector<int> >(m_rSetts->dimensions.x,
					     std::vector<int>(m_rSetts->dimensions.y, 1));
    m_naiveDistance = std::vector< std::vector< std::vector < std::map<sf::Vector2i, int, Vector2iComparator > > > >
	(m_rSetts->agentProfiles.size(), std::vector< std::vector< std::map<sf::Vector2i, int, Vector2iComparator> > >
	 (m_rSetts->dimensions.x, std::vector< std::map<sf::Vector2i, int, Vector2iComparator> >
	  (m_rSetts->dimensions.y)));
										       
    
    // nest generation
    // TODO: inflexible amount of sides having nests
    m_nests = std::vector< std::vector<sf::Vector2i> >(2, std::vector<sf::Vector2i>());
    for(int i = 0; i < m_rSetts->nestTotal; ++i)
    {
	sf::Vector2f temp = alongSquare((float)i / m_rSetts->nestTotal);
	sf::Vector2i nestCoords = sf::Vector2i(m_rSetts->dimensions.x/2.f + temp.x *
					       m_rSetts->dimensions.x/4.f,
					       m_rSetts->dimensions.x/2.f + temp.y *
					       m_rSetts->dimensions.y/4.f);

	printVector(temp, true);
	
	atCoords(m_data, nestCoords) = 10;
	m_nests[0].emplace_back(nestCoords);
	m_targets.emplace_back(m_nests[0].back());
	//m_nestDomains[0].insert({nestCoords, i});
    }

    // obstacles registration
    for(int x = 0; x < m_rSetts->dimensions.x; ++x)
    {
	for(int y = 0; y < m_rSetts->dimensions.y; ++y)
	{
	    if(m_data[x][y] == 1) // is a wall
	    {
		m_reservations.insert(std::make_pair(Reservation({x, y, 0, -1}), 1));
	    }
	}
    }
    
}

void Region::update()
{
    for(int x = 0; x < m_rSetts->dimensions.x; ++x)
    {
	for(int y = 0; y < m_rSetts->dimensions.y; ++y)
	{
	    m_toUpdate.push_back(sf::Vector2i(x, y));
	}
    }
    std::cout << m_ticks << std::endl;;
    
    for(int i = 0; i < m_toUpdate.size(); ++i)
    {
	int index = (m_toUpdate[i].x * m_rSetts->dimensions.y + m_toUpdate[i].y) * 4;
	int type = atCoords(m_data, m_toUpdate[i]);
	
	std::vector<sf::Vector2f> texCoords = {
	    {(float) m_rSetts->tileTypes[type].textureIndex    * m_rSetts->texTileSize, 0.f},
	    {(float)(m_rSetts->tileTypes[type].textureIndex+1) * m_rSetts->texTileSize, 0.f},
	    {(float)(m_rSetts->tileTypes[type].textureIndex+1) * m_rSetts->texTileSize,
	     (float)m_rSetts->texTileSize},
	    {(float) m_rSetts->tileTypes[type].textureIndex    * m_rSetts->texTileSize,
	     (float)m_rSetts->texTileSize}
	};

	sf::Color currColor = m_rSetts->tileTypes[type].normColor;

	if(isReserved(m_toUpdate[i], m_ticks, 1) == -1) currColor = sf::Color(255, 0, 0);
	else if(isReserved(m_toUpdate[i], m_ticks, 1) == 1) currColor = sf::Color(0, 255, 255);
		
	for(int j = 0; j < 4; ++j)
	{
	    m_representation[index + j].color = currColor;
	    m_representation[index + j].texCoords = texCoords[j];
	}
    }

    m_toUpdate.clear();
}

// -1 means reserved by another agent or undiggable obstacle
// 0 means free
// 1 means reserved by a diggable obstacle

int Region::isReserved(int x, int y, int from, int duration)
{
    int toDig = 0;
    int to = from + duration-1;
    // checking if there are any reservations
    if(m_reservations.begin() != m_reservations.end())
    {
	// first element that is not smaller than time
	auto it = m_reservations.lower_bound({x, y, from, to});

	// is it overlapping with previous reservation
	--it;
	if(it->first.x == x && it->first.y == y && (it->first.to >= from || it->first.to == -1))
	{
	    return it->second;
	}

	// is it ovelapping with next reservation
	++it;
	if(it->first.x == x && it->first.y == y && it->first.from <= to) return it->second;
    }

    return 0;
}

int Region::isReserved(sf::Vector2i coords, int from, int duration)
{
    return isReserved(coords.x, coords.y, from, duration);
}

void Region::reserve(sf::Vector2i coords, int from, int duration)
{
    int to = from + duration-1;
    if(duration == -1) to = -1;
    Reservation temp = {coords.x, coords.y, from, to};
    m_reservations[temp] = -1;
    //m_toUpdate.push_back(coords);

    m_toCleanAt.insert({to, temp});
}

bool Region::dereserve(sf::Vector2i coords, int freeAt)
{
    Reservation oldR = {coords.x, coords.y, 0, -1};
    if(m_reservations.find(oldR) != m_reservations.end())
    {
	Reservation newR = {coords.x, coords.y, 0, freeAt-1};
	m_reservations[newR] = m_reservations[oldR];
	std::cout << "Should not be -1: " << freeAt-1 << std::endl;
	m_reservations.erase(oldR);
	
	return true;
    }
    return false;
}

void Region::calcNaiveDistance(sf::Vector2i from, sf::Vector2i startAt)
{
    std::set<PathCoord, PathHeuresticComparator> toVisit;
    int startingValue;

    for(int i = 0; i < m_rSetts->agentProfiles.size(); ++i)
    {
	bool nogo = false;
	// if default argument is used, start from target itself
	if(startAt == from || startAt == sf::Vector2i(-1, -1))
	{
	    startAt = from;
	    startingValue = 0;
	}
	else
	{
	    // otherwise search for neighbour with best heurestic score and start with them
	    Move min = Move::stay;
	    int minValue = -1;
	    nogo = true;
	    
	    for(int j = 0; j < getDirectionTotal(); ++j)
	    {
		sf::Vector2i temp = startAt + getMove(j);
		if(inBounds(temp))
		{
		    auto found = atCoords(m_naiveDistance[i], temp).find(from);
	        
		    if(found != atCoords(m_naiveDistance[i], temp).end() &&
		       (minValue == -1 || minValue > found->second))
		    {
			min = static_cast<Move>(j);
			minValue = found->second;
			nogo = false;
		    }
		}
	    }

	    if(!nogo)
	    {
		startAt = startAt + getMove(min);
		startingValue = atCoords(m_naiveDistance[i], startAt)[from];
	    }
	}
	
	if(!nogo)
	{
	    toVisit.insert(PathCoord(startAt, 0, startingValue));
	    atCoords(m_naiveDistance[i], startAt)[from] = startingValue;
	    //std::cout << atCoords(m_naiveDistance[i], startAt)[from] << std::endl;
	    while(toVisit.size() > 0)
	    {
		PathCoord curr = *toVisit.begin();
		toVisit.erase(*toVisit.begin());
		m_toUpdate.push_back(curr.coords());

		//curr.print();
	    
		for(int j = 0; j < getDirectionTotal(); ++j)
		{
		    bool noWay = false;
		    PathCoord temp = curr;
		    temp.move(static_cast<Move>(j));

		    if(inBounds(temp.coords()))
		    {
			auto found = atCoords(m_naiveDistance[i], temp.coords()).find(from);
			
			if(m_rSetts->agentProfiles[i].digging != -1 && isDiggable(temp.coords()))
			{
			    temp.h += m_rSetts->agentProfiles[i].walking + m_rSetts->agentProfiles[i].digging;
			}
			else if(isWalkable(temp.coords()))
			{
			    temp.h += m_rSetts->agentProfiles[i].walking;
			}
			else noWay = true;
		    
			if(!noWay && (found == atCoords(m_naiveDistance[i], temp.coords()).end() ||
				      found->second > temp.h))
			{
			    //std::cout << (found == atCoords(m_naiveDistance[i], temp.coords()).end()) << std::endl;
			    //std::cout << temp.h << std::endl;
			    atCoords(m_naiveDistance[i], temp.coords())[from] = temp.h;
			    toVisit.insert(temp);
			}
		    }
		}
	    }
	    toVisit.clear();
	}
    }
}

bool Region::digOut(sf::Vector2i coords)
{
    if(isDiggable(coords))
    {
	atCoords(m_data, coords) = 0;
	m_toUpdate.push_back(coords);
	for(int i = 0; i < m_targets.size(); ++i)
	{
	    calcNaiveDistance(m_targets[i], coords);
	}
	return true;
    }
    return false;
}
/*
std::vector<int> Region::getPath(sf::Vector2i start, int time, sf::Vector2i target, int profileIndex,
				 int diggingCapabilities)
{
    int ws = m_profiles[profileIndex].walkingSpeed;
    int ds = m_profiles[profileIndex].diggingSpeed;
    sf::Vector2i curr = start;
    int currTime = time;
    int currDig = diggingCapabilities;
    
    calcNaiveDistance(target);
    
    while(curr != target)
    {
	int bestMove = 0;
	int bestHeurestic = -1;
	sf::Vector2i next;
	
	for(int i = 0; i < getDirectionTotal(); ++i)
	{
	    next = curr + getMove(i);
	    
	    if(inBounds(next) &&
	       (bestHeurestic == -1 ||
		atCoords(m_data, next).naiveDistance[profileIndex][target] < bestHeurestic))
	    {
		bestMove = i;
		bestHeurestic = atCoords(m_data, next).naiveDistance[profileIndex][target];
	    }
	}
	curr += getMove(bestMove);

	// next is walkable
 	if(isReserved(curr.x, curr.y, time, ws) <= 0)
	{
	    time += ws;
	}
	else
	{
	    time += ws + ds;
	    currDig -= isReserved(curr.x, curr.y, time, ws + ds);
	}
    }
}
*/

int Region::getHeurestic(int profileIndex, sf::Vector2i at, sf::Vector2i to, int time)
{
    return atCoords(m_naiveDistance[profileIndex], at)[to]/* + time*/;
}

std::vector<Move> Region::findPath (sf::Vector2i start, int time, sf::Vector2i target, int profileIndex)
{
    if(time == -1) time = m_ticks;
    std::vector<Move> finalPath = {Move::stay};
    std::set<PathCoord, PathHeuresticComparator> potenPaths; // potentialPaths
    // map storing best direction from these toords to start
    std::map<PathCoord, std::pair<Move, int>, PathCoordComparator> directions;
    PathCoord winner(start, time);
    bool stop = false;

    int ws = m_rSetts->agentProfiles[profileIndex].walking;
    int ds = m_rSetts->agentProfiles[profileIndex].digging;

    //std::cout << time << std::endl;

    m_targets.emplace_back(target);
    calcNaiveDistance(target);
    //std::cout << profileIndex << " " << ds << "\n";;
    //printVector(target);
    winner.h = getHeurestic(profileIndex, start, target, time);
    potenPaths.insert(winner);
    directions[winner] = std::make_pair(Move::stay, 0);

    while(potenPaths.size() > 0)
    {
	PathCoord curr = *potenPaths.begin();
	potenPaths.erase(potenPaths.begin());

	//curr.print();
	//getchar();

	// check if you are at target or if outside of window
	if(curr.coords() == target || curr.t - time >= m_rSetts->pathWindowSize)
	{
	    if(curr.h < winner.h) winner = curr;
	}
	else
	{
	    // check if these coords are available for walking off of them
	    if(isReserved(curr.coords(), curr.t, ws) != -1)
	    {
		for(int dir = 0; dir < getDirectionTotal(); ++dir) // consider moves without the waiting one
		{
		    // coords which it checks
		    PathCoord next(curr.coords() + getMove(dir), curr.t);

		    if(inBounds(next.coords()))
		    {
			// is it possible to walk there
			bool nextWalk = (isReserved(next.coords(), next.t, ws) == 0);
		
			// is it possible to dig there and how much digging there is
			bool nextDig = false;
			if(ds != -1 && isReserved(curr.coords(), curr.t, ws + ds) != -1 &&
			   isReserved(next.coords(), next.t, ws + ds) == 1)
			{
			    next.t += ws + ds;
			    nextDig = true;
			}
			else if(nextWalk)
			{
			    next.t += ws;
			}
			// evaluate next using a heurestic
			next.h = getHeurestic(profileIndex, next.coords(), target, next.t);

			if(directions.find(next) == directions.end() &&
			   // checks if coords haven't been visited
			   (nextWalk || nextDig))
			{
			    // marks which direction it came to these coords
			    //std::cout << directions[next].second << std::endl;

			    // if it has to (and can) dig
			    if(nextDig)
			    {
				directions[next] = std::make_pair(static_cast<Move>(dir), ws + ds);
				potenPaths.insert(next);
			    }
			    else // if it can walks here
			    {
				directions[next] = std::make_pair(static_cast<Move>(dir), ws);
				potenPaths.insert(next);
			    }
			}
		    }
		}
		// consider waiting as a move
		PathCoord next(curr.coords(), curr.t+1);
		next.h = getHeurestic(profileIndex, next.coords(), target, next.t);
		
		if(isReserved(curr.coords(), curr.t, 1) == 0 &&
		   directions.find(next) == directions.end())
		{
		    directions[next] = std::make_pair(Move::stay, 1);
		    potenPaths.insert(next);
		}
	    }
	    else std::cout << "duud, my place is reserved\n";
	}
    }

    std::cout << "What did the grand champion do?" << std::endl;
    //winner.print();
    //if(winner.coords() != start)
    {
	PathCoord curr = winner;
	finalPath.clear();
	// recreate best path found
	while(curr.coords() != start && curr.t != time)
	{
	    //printVector(curr.coords());
	    finalPath.push_back(directions[curr].first);
	    curr.t -= directions[curr].second;
	    curr.move(reverseDirection(finalPath.back()));
	    //getchar();
	}

	if(curr.t != time)
	{
	    std::cout << "WAHT A FUCKING LIAR DUDE" << curr.t << " " << time << std::endl;
	    getchar();
	    return {Move::stay};
	}

	std::reverse(finalPath.begin(), finalPath.end());

	// erase second half of the path if the search isn't finished
	/*if(winner.coords() != target)
	{
	    finalPath.erase(finalPath.cbegin() + finalPath.size()/2, finalPath.cend());
	}
	*/

	// calculate time and reserve the coords it was traversing
	std::cout << "{ ";
	for(int i = 0; i < finalPath.size(); ++i)
	{
	    std::cout << finalPath[i] << " ";
	    // if it's waiting
	    if(finalPath[i] == 4)
	    {
		reserve(curr.coords(), curr.t, 1);
		curr.t += 1;
	    } 
	    else
	    {
		PathCoord next = curr;
		next.move(finalPath[i]);
		// else if walking is possible
		if(isReserved(next.coords(), curr.t, ws) == 0)
		{
		    reserve(curr.coords(), curr.t, ws);
		    reserve(next.coords(), curr.t, ws);
		    next.t += ws;
		}
		else // otherwise dig (it has to be a legal move, cause it was checked before)
		{
		    dereserve(next.coords(), curr.t + ds);
		    reserve(curr.coords(), curr.t, ws + ds);
		    reserve(next.coords(), curr.t, ws + ds);
		    next.t += ws + ds;
		}
		curr = next;
	    }
	    
	    if(winner.coords() != target && curr.t - time >= m_rSetts->pathWindowSize/2)
	    {
		finalPath.erase(finalPath.cbegin()+i+1, finalPath.cend());
		break;
	    }
	    if(i == finalPath.size()-1 && winner.coords() == target) reserve(winner.coords(), curr.t, -1);
	    std::cout << "}\n";
	}
    }
    //else std::cout << "HE DIDN'T FIN MOVE" << std::endl;
    
    return finalPath;
}

/*
std::pair<std::vector<int>, int> findPath(sf::Vector2i start, int time, std::vector<sf::Vector2i> targets,
					  int walkingSpeed, int diggingSpeed, int ableToDig, bool 
    std::pair<std::vector<int>, int> result;
    
    for(int i = 0; i < targets.size(); ++i)
    {
	sf::Vector2i beginning;
	if(i == 0) beginning = start;
	else beginning = targets[i-1];
	
	auto temp = findPath(beginning, time, targets[i], walkingSpeed,
							 diggingSpeed, ableToDig, reserve);

	result.first.insert(result.end(), temp.begin(), temp.end());
	result.second += temp.second;
    }

    return result;
}

std::pair<std::vector<int>, int> findDiggingPath(sf::Vector2i start, int time, sf::Vector2i target,
						 int walkingSpeed, int diggingSpeed, int ableToDig)
{
    std::pair<std::vector<int>, int> result;
    auto diggingPath = findPath(start, time, target, walkingSpeed, diggingSpeed, ableToDig, false, true);
    sf::Vector2i waypoint; // coords to which ant can dig to on these settings without unloading
    int rubbleSoFar = 0;

    for(int i = 0; i < diggingPath.first.size(); ++i)
    {
	
    }
}
*/

bool Region::tick(int ticksPassed)
{
    m_ticks = ticksPassed;

    auto found = m_toCleanAt.find(m_ticks);
    while(found != m_toCleanAt.end())
    {
	m_reservations.erase(m_reservations.find(found->second));
	m_toCleanAt.erase(found);
	found = m_toCleanAt.find(m_ticks);
    }
    
    update();

    return false;
}

bool Region::inBounds(sf::Vector2i coords)
{
    if(coords.x < 0 || coords.x >= m_rSetts->dimensions.x ||
       coords.y < 0 || coords.y >= m_rSetts->dimensions.y)
	return false;
    return true;
}

sf::Vector2i Region::getClosestNest(int allegiance, sf::Vector2i coords, int profileIndex)
{
    sf::Vector2i result(-1, -1);
    int minDistance = -1;

    return result;
    
    for(int i = 0; i < m_nests[allegiance].size(); ++i)
    {
	auto found = atCoords(m_naiveDistance[profileIndex], coords).find(m_nests[allegiance][i]);
	if(found != atCoords(m_naiveDistance[profileIndex], coords).end() &&
	   (minDistance == -1 || minDistance > found->second))
	{
	    result = m_nests[allegiance][i];
	    minDistance = found->second;
	}
    }
    
    return result;
}

bool Region::isWalkable(sf::Vector2i coords)
{
    if(inBounds(coords) && m_rSetts->tileTypes[atCoords(m_data, coords)].isWalkable)
    {
	return true;
    }
    return false;
}

bool Region::isDiggable(sf::Vector2i coords)
{
    if(inBounds(coords) && m_rSetts->tileTypes[atCoords(m_data, coords)].isDiggable)
    {
	return true;
    }
    return false;
}

bool Region::isYankable(sf::Vector2i coords)
{
    if(inBounds(coords) && m_rSetts->tileTypes[atCoords(m_data, coords)].isYankable)
    {
	return true;
    }
    return false;
}

void Region::draw(sf::RenderTarget& target)
{
    target.draw(&m_representation[0], m_representation.size(), sf::Quads, m_states);
}
