#include "MapSquare.h"


MapSquare::MapSquare(std::string fileName)
{
	mi_mapWidth = 0;
	mi_mapHeight = 0;
	//throws an exception?
	/*mv_nodes.resize( start_size );
	for ( int i = 0; i < start_size; i++ )
	{
		mv_nodes[i].resize(start_size);
	}*/

	LoadMap(fileName);
	CalcWallInfluence();

}


MapSquare::~MapSquare(void)
{
	free(mp_memStart);
}

void MapSquare::FillWallInfluence( INT32 range, INT32 x, INT32 y )
{
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
		{
			// max alias decided to break horribly?
			int dist = ManhattanDistance( j, i, x, y );
			if( dist < 5 )
			{
				mv_nodes[i][j]->wallInfluence += 5 - dist;
			}
		}
	}
}

void MapSquare::FillWallInfluenceRecurse( INT32 range, INT32 x, INT32 y)
{

}

bool MapSquare::LoadMap(std::string mapName)
{
	//create file streams and buffer string.
	std::ifstream mapFile;
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

		mi_mapHeight = i;
		mi_mapWidth = buffer[0].length();

		// allocate enough memory to hold all the nodes
		// this will crash and burn if the map isn't square.
		mp_memStart = (MapData*) malloc( sizeof(MapData) * mi_mapHeight * mi_mapWidth );
		mp_memNext = mp_memStart;

		//convert to int - probably a neater way to do this
		for (UINT32 y = 0; y < mi_mapHeight; y++)
		{
			for (int x = 0; x < mi_mapWidth; x++)
			{
				//add pointer to the array
				mv_nodes[x][y] = mp_memNext;

				//initialise - not strictly needed, but safer. 
				mp_memNext->cost = buffer[y][x] - 48;
				mp_memNext->heavyTurretInfluence = 0;
				mp_memNext->lightTurretInfluence = 0;
				mp_memNext->mediumTurretInfluence = 0;
				mp_memNext->wallInfluence = 0;
				mp_memNext->pathInfluence = 0;

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

void MapSquare::CalcWallInfluence()
{
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
		{
			if( mv_nodes[j][i]->cost == 0 )
			{
				FillWallInfluence(WALL_INFLUENCE_RANGE, j, i);
			}
		}
	}
}

