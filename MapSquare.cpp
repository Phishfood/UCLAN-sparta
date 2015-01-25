#include "MapSquare.h"

int SortCoordsScore2( const void* a, const void* b )
{
	Coords* x = (Coords*)a;
	Coords* y = (Coords*)b;
	return (x->score - y->score);
}

MapSquare::MapSquare(std::string fileName, SettingsManager* settings)
{
	mp_timer = new CTimer();
	mySettings = settings;
	mb_writeXML = false;
	mi_mapWidth = 0;
	mi_mapHeight = 0;
	
	mi_nodeCount = 0;
	mi_splineCount = 0;

	mi_heavyTurretCount = 0;
	mi_mediumTurretCount = 0;
	mi_lightTurretCount = 0;

	ms_P1Start.x = -1;
	ms_P1Start.y = -1;

	ms_P2Start.x = -1;
	ms_P2Start.y = -1;

	LoadMap(fileName);

	CalcWallInfluence();
	FillBaseInfluence(mySettings->baseRange, ms_P1Start.x, ms_P1Start.y);
	FillBase2Influence( mySettings->baseRange, ms_P2Start.x, ms_P2Start.y );
	//FillInfluenceRecurse2(BASE_OFFSET, mySettings->baseRange, ms_P1Start.x, ms_P1Start.y);
	//FillInfluenceRecurse2(BASE2_OFFSET, mySettings->baseRange, ms_P2Start.x, ms_P2Start.y);

	FindPath();

	if(mb_writeXML)
	{
		WriteMap( fileName );
	}
}


MapSquare::~MapSquare(void)
{
	free(mp_memStart);
}

void MapSquare::CalcWallInfluence()
{
	for(uint32_t i = 0; i < mi_mapHeight; i++)
	{
		for(uint32_t j = 0; j < mi_mapWidth; j++)
		{
			if( mv_nodes[j][i]->cost == 0 )
			{
				//iterative way
				FillWallInfluence( mySettings->wallRange, j, i);
				
				//sane way
				//FillInfluenceRecurse(mySettings->wallRange, j, i);

				//insane way
				//FillInfluenceRecurse2( WALL_OFFSET, mySettings->wallRange, j, i);
			}
		}
	}
}

void MapSquare::FillWallInfluence( INT32 range, INT32 x, INT32 y )
{
	for(uint32_t i = 0; i < mi_mapHeight; i++)
	{
		for(uint32_t j = 0; j < mi_mapWidth; j++)
		{
			// max alias decided to break horribly?
			int dist = ManhattanDistance( j, i, x, y );
			if( dist < range )
			{
				mv_nodes[j][i]->wallInfluence += range - dist;
			}
		}
	}
}

void MapSquare::FillBaseInfluence( INT32 range, INT32 x, INT32 y )
{
	for(uint32_t i = 0; i < mi_mapHeight; i++)
	{
		for(uint32_t j = 0; j < mi_mapWidth; j++)
		{
			// max alias decided to break horribly?
			int dist = ManhattanDistance( j, i, x, y );
			if( dist < range )
			{
				mv_nodes[j][i]->baseInfluence += range - dist;
			}
		}
	}
}

void MapSquare::FillBase2Influence( INT32 range, INT32 x, INT32 y )
{
	for(uint32_t i = 0; i < mi_mapHeight; i++)
	{
		for(uint32_t j = 0; j < mi_mapWidth; j++)
		{
			// max alias decided to break horribly?
			int dist = ManhattanDistance( j, i, x, y );
			if( dist < range )
			{
				mv_nodes[j][i]->base2Influence += range - dist;
			}
		}
	}
}

void MapSquare::FillInfluenceRecurse( INT32 range, INT32 x, INT32 y)
{
	mv_nodes[x][y]->wallInfluence++;
	range--;
	if(range == 0)				return;
	if( (x-1) >= 0 )			FillInfluenceRecurse( range, x-1, y);
	if( (x+1) < mi_mapWidth)	FillInfluenceRecurse( range, x+1, y);
	if( (y-1) >= 0 )			FillInfluenceRecurse( range, x, y-1);
	if( (y+1) < mi_mapHeight)	FillInfluenceRecurse( range, x, y+1);
}

void MapSquare::FillInfluenceRecurse2( INT32 offset, INT32 range, INT32 x, INT32 y)
{
	UINT32* target = (UINT32*) mv_nodes[x][y];
	target += offset;
	(*target)++;
	range--;
	if(range == 0)				return;
	if( (x-1) >= 0 )			FillInfluenceRecurse2( offset, range, x-1, y);
	if( (x+1) < mi_mapWidth)	FillInfluenceRecurse2( offset, range, x+1, y);
	if( (y-1) >= 0 )			FillInfluenceRecurse2( offset, range, x, y-1);
	if( (y+1) < mi_mapHeight)	FillInfluenceRecurse2( offset, range, x, y+1);
}

bool MapSquare::LoadMap(std::string mapName)
{
	std::string xmlName = mapName + ".xml";
	std::ifstream fileStream(xmlName);
    if (fileStream.good()) {
        fileStream.close();
		return LoadXMLMap(mapName);
    } else {
        fileStream.close();
		mb_writeXML = true;
		return LoadTextMap(mapName);
    }   
}

bool MapSquare::WriteMap(std::string fileName)
{
	char temp[12];
	fileName += ".xml";
	tinyxml2::XMLDocument xmlDoc;

	tinyxml2::XMLDeclaration* dec = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild(dec);

	tinyxml2::XMLElement* rootElement;
	tinyxml2::XMLElement* rowElement;
	tinyxml2::XMLElement* nodeElement;

	rootElement = xmlDoc.NewElement("map");

	for (UINT32 y = 0; y < mi_mapHeight; y++)
	{
		rowElement = xmlDoc.NewElement("row");
		for (UINT32 x = 0; x < mi_mapWidth; x++)
		{
			 nodeElement = xmlDoc.NewElement("node");
			
			_itoa_s( mv_nodes[x][y]->cost, temp, 10 );
			nodeElement->SetAttribute("cost", temp );

			_itoa_s( mv_nodes[x][y]->wallInfluence, temp, 10 );
			nodeElement->SetAttribute("wallim", temp );

			_itoa_s( mv_nodes[x][y]->baseInfluence, temp, 10 );
			nodeElement->SetAttribute("baseim", temp );

			_itoa_s( mv_nodes[x][y]->base2Influence, temp, 10 );
			nodeElement->SetAttribute("base2im", temp );

			rowElement->LinkEndChild(nodeElement);
		}
		rootElement->LinkEndChild(rowElement);
	}

	xmlDoc.LinkEndChild(rootElement);

	return !xmlDoc.SaveFile(fileName.c_str());

}

bool MapSquare::LoadXMLMap(std::string fileName)
{
	//TODO - read the XML map for real.
	return LoadTextMap(fileName);
}

bool MapSquare::LoadTextMap(std::string mapName)
{
	//create file streams and buffer string.
	std::ifstream mapFile;
	// note - the buffer will read y first (line) then x (character) 
	// unlike the map array which reads x then y.
	std::string buffer[MAX_MAP_SIZE];

	//append expected filenames
	mapFile.open(mapName + ".txt");

	//check files opened correctly. 
	if (mapFile.is_open())
	{
		UINT32 i = 0;
		while (!mapFile.eof() && i < MAX_MAP_SIZE){
			getline(mapFile, buffer[i]);
			i++;
		}

		mi_mapHeight = i-1;
		mi_mapWidth = buffer[0].length()-1;

		// allocate enough memory to hold all the nodes
		// this will crash and burn if the map isn't square.
		mp_memStart = (MapData*) malloc( sizeof(MapData) * mi_mapHeight * mi_mapWidth );
		mp_memNext = mp_memStart;

		for (UINT32 y = 0; y < mi_mapHeight; y++)
		{
			for (UINT32 x = 0; x < mi_mapWidth; x++)
			{
				//add pointer to the array
				mv_nodes[x][y] = mp_memNext;

				//convert single ascii char to int - x/y reversed. See declaration.
				mp_memNext->cost = buffer[y][x] - 48;

				if( mp_memNext->cost == 2 )
				{
					ms_P1Start.x = x;
					ms_P1Start.y = y;
				}

				if( mp_memNext->cost == 3 )
				{
					ms_P2Start.x = x;
					ms_P2Start.y = y;
				}

				//initialise to 0 
				mp_memNext->heavyTurretInfluence = 0;
				mp_memNext->lightTurretInfluence = 0;
				mp_memNext->mediumTurretInfluence = 0;
				mp_memNext->wallInfluence = 0;
				mp_memNext->pathInfluence = mp_memNext->cost;
				mp_memNext->baseInfluence = 0;
				mp_memNext->base2Influence = 0;

				mp_memNext++;
			}
		}


		mapFile.close();
		return true;
	}
	return false;
}

INT32 MapSquare::ManhattanDistance(INT32 x1, INT32 y1, INT32 x2, INT32 y2)
{
        int x = abs(x1 - x2);
        int y = abs(y1 - y2);
        return x + y;
}

//tries to generate a path between both player's bases
bool MapSquare::FindPath()
{
	mi_nodeCount = 0;
	mi_splineCount = 0;
	Coords* current = new Coords();
	current->x = ms_P1Start.x;
	current->y = ms_P1Start.y;
	current->parent = NULL;
	current->cost = GetCost(ms_P1Start.x, ms_P1Start.y);

	std::vector<Coords*>* open = new std::vector<Coords*>;
	std::vector<Coords*>* closed = new std::vector<Coords*>;

	GenerateCoords(current, open, closed);

	//while we haven't reached the other start
	while( !CompareCoords( current, &ms_P2Start ) )
	{
		if(open->size() == 0)
		{
			return false; //out of nodes to explore - no route found.
		}
		closed->push_back(current);
		current = GetSmallest(open);

		//qsort(open, open->size(), sizeof(Coords*), SortCoordsScore2);
		//current = *open->end();

		if( current == NULL )
		{
			return false; //something terrible has happened. 
		}

		GenerateCoords( current, open, closed );

	}

	//debug
	DisplayPath( current );

	while( current->parent != NULL )
	{
		ms_path[mi_nodeCount] = (*current);
		mi_nodeCount++;
		current = current->parent;
	}
	ms_path[mi_nodeCount] = (*current);
	mi_nodeCount++;


	for( uint32_t i = 0; i < mi_nodeCount; i ++ )
	{
		if( i == 0 )
		{
			QuarterSplines(&ms_path[0], &ms_path[0], &ms_path[1], &ms_path[2], ms_pathSplines[0], ms_pathSplines[1], ms_pathSplines[2]);
		}
		else if( i == mi_nodeCount - 2 )
		{
			QuarterSplines( &ms_path[i-1], &ms_path[i], &ms_path[i+1], &ms_path[i+1], ms_pathSplines[mi_splineCount], ms_pathSplines[mi_splineCount + 1], ms_pathSplines[mi_splineCount + 2] );
		}
		else
		{
			QuarterSplines( &ms_path[i - 1], &ms_path[i], &ms_path[i + 1], &ms_path[i + 2], ms_pathSplines[mi_splineCount], ms_pathSplines[mi_splineCount + 1], ms_pathSplines[mi_splineCount + 2] );
		}

		mi_splineCount+=3;
	}

	FindChoke();
	FillHTIM();
	FillMTIM();
	FillLTIM();
	FillPathIM();

	return true;
}

//returns true if two coords are equal
bool MapSquare::CompareCoords(Coords* a, Coords* b)
{
	return (a->x == b->x && a->y == b->y);
}

int MapSquare::SortCoordsScore(const void* a, const void* b)
{
	Coords* x = (Coords*)a;
	Coords* y = (Coords*)b;
	return (x->score - y->score);
}



void MapSquare::GenerateCoords(Coords* source, std::vector<Coords*>* &open, std::vector<Coords*>* &closed)
{
	
	if( (source->x-1) >= 0 && GetCost(source->x - 1, source->y) != 0)
	{
		GenerateOneNode( source->x -1, source->y, source, open, closed);
	}

	if( (source->x + 1) < mi_mapWidth  && GetCost( source->x + 1, source->y ) != 0 )
	{
		GenerateOneNode( source->x + 1, source->y, source, open, closed );
	}

	if( (source->y - 1) >= 0 && GetCost( source->x, source->y - 1) != 0 )
	{
		GenerateOneNode( source->x, source->y-1, source, open, closed );
	}

	if( (source->y + 1) < mi_mapHeight && GetCost( source->x , source->y + 1) != 0 )
	{
		GenerateOneNode( source->x, source->y+1, source, open, closed );
	}

}

void MapSquare::GenerateOneNode( uint32_t x, uint32_t y, Coords* parent, std::vector<Coords*>* &open, std::vector<Coords*>* &closed )
{
	Coords* temp = new Coords;
	temp->x = x;
	temp->y = y;
	temp->cost = parent->cost + GetCost(x, y);
	temp->score = abs(temp->x - ms_P2Start.x) + abs(temp->y - ms_P2Start.y) +temp->cost + GetWallI(temp->x, temp->y);
	temp->parent = parent;
	bool exists = CheckExists( open, temp ) || CheckExists( closed, temp );
	if( !exists )
	{
		open->push_back( temp );
		temp = new Coords;
	}
	else
	{
		delete temp;
	}
}

bool MapSquare::CheckExists(std::vector<Coords*>* list, Coords* item)
{
	if( list->size() == 0 )
	{
		return false;
	}
	auto it = list->begin();
	while( it != list->end() )
	{
		if( CompareCoords( item, (*it)) )
		{
			return true;
		}
		it++;
	}
	return false;
}

Coords* MapSquare::GetSmallest( std::vector<Coords*>* list )
{
	Coords* smallest = NULL;
	int32_t smallestScore=9999999;
	auto it = list->begin();
	auto deleteMe = it;
	while( it != list->end() )
	{
		if( (*it)->score < smallestScore )
		{
			smallest = (*it);
			smallestScore = smallest->score;
			deleteMe = it;
		}
		it++;
	}
	list->erase(deleteMe);
	return smallest;
}

void MapSquare::DisplayPath( Coords* start )
{
	char buffer[12];
	char fileName[12] = "path.xml";

	tinyxml2::XMLDocument xmlDoc;

	tinyxml2::XMLDeclaration* dec = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild( dec );

	tinyxml2::XMLElement* rowElement;
	tinyxml2::XMLElement* nodeElement;


	rowElement = xmlDoc.NewElement( "path" );
	Coords* temp = start;
	std::cout << start->x << "," << start->y << std::endl;
	while( temp->parent != NULL )
	{ 
		nodeElement = xmlDoc.NewElement( "node" );
		_itoa_s( temp->x, buffer, 10 );
		nodeElement->SetAttribute( "X", buffer );

		_itoa_s( temp->y, buffer, 10 );
		nodeElement->SetAttribute( "Y", buffer );

		rowElement->LinkEndChild( nodeElement );
		temp = temp->parent;
		
	}
	xmlDoc.LinkEndChild( rowElement );
	xmlDoc.SaveFile( fileName );
	return; 

}

void MapSquare::QuarterSplines( Coords* point1, Coords* point2, Coords*  point3, Coords*  point4, sCoords*  &quarter, sCoords*  &half, sCoords*  &threeQuarter )
{
	// calculate a quarter of the way along the set of points Bezier
	quarter = new sCoords();
	quarter->x = float( point1->x ) * 0.421875f + float( point2->x ) * 0.421875f + float( point3->x ) * 0.140625f + float( point4->x ) * 0.015625f;
	quarter->y = float( point1->y ) * 0.421875f + float( point2->y ) * 0.421875f + float( point3->y ) * 0.140625f + float( point4->y ) * 0.015625f;

	
	// calculate a half of the way along the set of points Bezier
	half = new sCoords();
	half->x = float( point1->x ) * 0.125f + float( point2->x ) * 0.375f + float( point3->x ) * 0.375f + float( point4->x ) * 0.125f;
	half->y = float( point1->y ) * 0.125f + float( point2->y ) * 0.375f + float( point3->y ) * 0.375f + float( point4->y ) * 0.125f;

	// calculate three-quarters of the way along the set of points Bezier
	threeQuarter = new sCoords();
	threeQuarter->x = float( point1->x ) * 0.015625f + float( point2->x ) * 0.140625f + float( point3->x ) * 0.421875f + float( point4->x ) * 0.421875f;
	threeQuarter->y = float( point1->y ) * 0.015625f + float( point2->y ) * 0.140625f + float( point3->y ) * 0.421875f + float( point4->y ) * 0.421875f;
}

void MapSquare::FindChoke()
{
	uint32_t min = mySettings->MinPathLength; //minimum distance before placing turrets. 
	uint32_t max = mi_nodeCount * mySettings->MaxPathLength; //maximum distance to find a choke point. 
	
	//set values where we start on the path.
	Coords choke = ms_path[min];
	uint32_t currentScore, lastScore = GetWallI(ms_path[min].x, ms_path[min].y);

	min++;

	for (uint32_t i = min; i < max; i++)
	{
		currentScore = GetWallI(ms_path[i].x, ms_path[i].y);
		if (currentScore > lastScore)
		{
			lastScore = currentScore;
			choke = ms_path[i];
		}
	}

	ms_chokePoint = choke;

}

void MapSquare::FillHTIM()
{
	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			uint32_t dist = ManhattanDistance(x, y, ms_chokePoint.x, ms_chokePoint.y);
			if (dist < mySettings->HTRange)
			{
				mv_nodes[x][y]->heavyTurretInfluence = mySettings->HTRange - dist;
			}
		}
	}
}

void MapSquare::FillMTIM()
{
	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			uint32_t dist = ManhattanDistance(x, y, ms_chokePoint.x, ms_chokePoint.y);
			if (dist < mySettings->MTRange)
			{
				mv_nodes[x][y]->mediumTurretInfluence = mySettings->MTRange - dist;
			}
		}
	}
}

void MapSquare::FillLTIM()
{
	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			uint32_t dist = ManhattanDistance(x, y, ms_chokePoint.x, ms_chokePoint.y);
			if (dist < mySettings->LTRange)
			{
				mv_nodes[x][y]->lightTurretInfluence = mySettings->LTRange - dist;
			}
		}
	}
}

void MapSquare::FillPathIM()
{
	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			mv_nodes[x][y]->pathInfluence = mv_nodes[x][y]->cost;
		}
	}

	for (uint32_t i = 0; i < mi_nodeCount; i++)
	{
		mv_nodes[ms_path[i].x][ms_path[i].y]->pathInfluence = 0;
	}
}

void MapSquare::PlaceHeavyTurret()
{
	Coords best;
	uint32_t bestScore = 9999999;
	uint32_t currentScore = 0;

	if (mi_heavyTurretCount >= MAX_TURRETS)
	{
		return;
	}

	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			if (mv_nodes[x][y]->heavyTurretInfluence == 1 && mv_nodes[x][y]->pathInfluence > 0)
			{
				currentScore = ManhattanDistance(ms_P1Start.x, ms_P1Start.y, x, y);
			}
			else
			{
				currentScore = 9999999;
			}

			if (currentScore < bestScore)
			{
				bestScore = currentScore;
				best.x = x;
				best.y = y;
			}

		}
	}

	ms_heavyTurrets[mi_heavyTurretCount] = best;
	mi_heavyTurretCount++;

	mv_nodes[best.x][best.y]->pathInfluence = 0;
}

void MapSquare::PlaceMediumTurret()
{
	Coords best;
	uint32_t bestScore = 9999999;
	uint32_t currentScore = 0;

	if (mi_mediumTurretCount >= MAX_TURRETS)
	{
		return;
	}

	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			if (mv_nodes[x][y]->mediumTurretInfluence == 1 && mv_nodes[x][y]->pathInfluence > 0)
			{
				currentScore = ManhattanDistance(ms_P1Start.x, ms_P1Start.y, x, y);
			}
			else
			{
				currentScore = 9999999;
			}

			if (currentScore < bestScore)
			{
				bestScore = currentScore;
				best.x = x;
				best.y = y;
			}

		}
	}

	ms_mediumTurrets[mi_mediumTurretCount] = best;
	mi_mediumTurretCount++;

	mv_nodes[best.x][best.y]->pathInfluence = 0;
}


void MapSquare::PlaceLightTurret()
{
	Coords best;
	uint32_t bestScore = 9999999;
	uint32_t currentScore = 0;

	if (mi_lightTurretCount >= MAX_TURRETS)
	{
		return;
	}

	for (uint32_t y = 0; y < mi_mapHeight; y++)
	{
		for (uint32_t x = 0; x < mi_mapWidth; x++)
		{
			if (mv_nodes[x][y]->lightTurretInfluence == 1 && mv_nodes[x][y]->pathInfluence > 0)
			{
				currentScore = ManhattanDistance(ms_P1Start.x, ms_P1Start.y, x, y);
			}
			else
			{
				currentScore = 9999999;
			}

			if (currentScore < bestScore)
			{
				bestScore = currentScore;
				best.x = x;
				best.y = y;
			}

		}
	}

	ms_lightTurrets[mi_lightTurretCount] = best;
	mi_lightTurretCount++;

	mv_nodes[best.x][best.y]->pathInfluence = 0;
}