#include "MapSquare.h"

MapSquare::MapSquare(std::string fileName)
{
	mb_writeXML = false;
	mi_mapWidth = 0;
	mi_mapHeight = 0;
	//throws an exception?
	/*mv_nodes.resize( start_size );
	for ( int i = 0; i < start_size; i++ )
	{
		mv_nodes[i].resize(start_size);
	}*/

	ms_P1Start.x = -1;
	ms_P1Start.y = -1;

	ms_P2Start.x = -1;
	ms_P2Start.y = -1;

	LoadMap(fileName);

	CalcWallInfluence();
	FillBaseInfluence(BASE_INFLUENCE_RANGE, ms_P1Start.x, ms_P1Start.y);
	FillBase2Influence(BASE_INFLUENCE_RANGE, ms_P2Start.x, ms_P2Start.y);
	//FillInfluenceRecurse2(BASE_OFFSET, BASE_INFLUENCE_RANGE, ms_P1Start.x, ms_P1Start.y);
	//FillInfluenceRecurse2(BASE2_OFFSET, BASE_INFLUENCE_RANGE, ms_P2Start.x, ms_P2Start.y);

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
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
		{
			if( mv_nodes[j][i]->cost == 0 )
			{
				//iterative way
				FillWallInfluence( WALL_INFLUENCE_RANGE, j, i);
				
				//sane way
				//FillInfluenceRecurse(WALL_INFLUENCE_RANGE, j, i);

				//insane way
				//FillInfluenceRecurse2( WALL_OFFSET, WALL_INFLUENCE_RANGE, j, i);
			}
		}
	}
}

void MapSquare::FillWallInfluence( INT32 range, INT32 x, INT32 y )
{
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
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
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
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
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
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

	tinyxml2::XMLElement* rowElement;
	tinyxml2::XMLElement* nodeElement;

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
		xmlDoc.LinkEndChild(rowElement);
	}

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
				mp_memNext->pathInfluence = 0;
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
	ms_path[0] = ms_P1Start;

	Coords* current = new Coords();
	current->x = ms_P1Start.x;
	current->y = ms_P1Start.y;

	std::vector<Coords*>* open = new std::vector<Coords*>;
	std::vector<Coords*>* closed = new std::vector<Coords*>;

	GenerateCoords(current, open, closed);

	//while we haven't reached the other start
	while( !CompareCoords( current, &ms_P2Start ) )
	{
		if(open->size() == 0)
		{
			return false;
		}
	}
}

//returns true if two coords are equal
bool MapSquare::CompareCoords(Coords* a, Coords* b)
{
	return (a->x == b->x) && (a->y == b->y);
}

int MapSquare::SortCoordsScore(const void* a, const void* b)
{
	Coords* x = (Coords*)a;
	Coords* y = (Coords*)b;
	return (x->score - y->score);
}

int SortCoordsScore2(const void* a, const void* b)
{
	Coords* x = (Coords*)a;
	Coords* y = (Coords*)b;
	return (x->score - y->score);
}

void MapSquare::GenerateCoords(Coords* source, std::vector<Coords*>* open, std::vector<Coords*>* closed)
{
	Coords* temp = new Coords;
	if( (source->x-1) >= 0 )
	{
		temp->x = source->x-1;
		temp->y = source->y;
		if( !( CheckExists(open, temp) || CheckExists(closed, temp) ) )
		{
			open->push_back(temp);
			temp = new Coords;
		}
	}

	if( (source->x+1) < mi_mapWidth)
	{
		temp->x = source->x+1;
		temp->y = source->y;
		if( !( CheckExists(open, temp) || CheckExists(closed, temp) ) )
		{
			open->push_back(temp);
			temp = new Coords;
		}
	}

	if( (source->y-1) >= 0 )
	{
		temp->x = source->x;
		temp->y = source->y-1;
		if( !( CheckExists(open, temp) || CheckExists(closed, temp) ) )
		{
			open->push_back(temp);
			temp = new Coords;
		}
	}

	if( (source->y+1) < mi_mapHeight)
	{
		temp->x = source->x;
		temp->y = source->y+1;
		if( !( CheckExists(open, temp) || CheckExists(closed, temp) ) )
		{
			open->push_back(temp);
		}
	}
	else
	{
		delete temp;
	}

	qsort(open, open->size(), sizeof(Coords*), SortCoordsScore2);
}

bool MapSquare::CheckExists(std::vector<Coords*>* list, Coords* item)
{
	if( list->size() == 0 )
	{
		return false;
	}
	for( int i = 0; i < list->size(); i++ )
	{
		if( CompareCoords( item, (*list)[i]) )
		{
			return true;
		}
	}
	return false;
}