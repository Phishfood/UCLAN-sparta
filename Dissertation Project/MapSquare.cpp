#include "MapSquare.h"

const int start_size = 200;

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

}

void MapSquare::FillWallInfluence( INT32 range, INT32 x, INT32 y )
{
	for(int i = 0; i < mi_mapHeight; i++)
	{
		for(int j = 0; j < mi_mapWidth; j++)
		{
			mv_nodes[i][j]->wallInfluence += max( 0, WALL_INFLUENCE_RANGE - ManhattanDistance( j, i, x, y ) );
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


		//convert to int - probably a neater way to do this
		for (UINT32 y = 0; y < mi_mapHeight; y++)
		{
			std::vector<MapData*> row;
			for (int x = 0; x < buffer[y].length(); x++)
			{
				MapData* temp =  new MapData;
				temp->cost = buffer[y][x] - 48;
				temp->heavyTurretInfluence = 0;
				temp->lightTurretInfluence = 0;
				temp->mediumTurretInfluence = 0;
				temp->wallInfluence = 0;
				row.push_back( temp );
			}
			mv_nodes.push_back(row);
		}


		mapFile.close();
		return true;
	}
	return false;
}

INT32 MapSquare::ManhattanDistance(int x1, int y1, int x2, int y2)
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
			if( mv_nodes[i][j]->cost == 0 )
			{
				FillWallInfluence(WALL_INFLUENCE_RANGE, j, i);
			}
		}
	}
}

