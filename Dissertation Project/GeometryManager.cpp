#include "GeometryManager.h"


CGeometryManager::CGeometryManager(void)
{
	numMeshes = 0;
}


CGeometryManager::~CGeometryManager(void)
{

}


CGeometry* CGeometryManager::LoadObject(const char file[100],const int length)
{
	//Hash the file path
	int hash = Hash(file, length);
	hash %= MAX_MESHES;

	//check to see if it exists. 
	if( !meshes[hash].empty() )			//first check if the list is empty. 
	{
		//create iterator
		std::list<CGeometry*>::iterator temp = meshes[hash].begin();
		//scroll through the list
		while( temp != meshes[hash].end() )
		{
			//check to see if item in list matches input
			if( (*temp)->CompareSource( file ) )
			{
				//if so return the existing pointer
				return *temp;
			}// end if compare
		}//end while
	}//end if empty
	
	//Load mesh

	CGeometry* newGeometry = new CGeometry;
	//newGeometry->Load(file, length, NULL, false);
	newGeometry->Load(string(file), NULL, false);
	if( false /*newGeometry->Load(file, length, NULL, true)*/ )
	{
		meshes[hash].push_back( newGeometry );
		return newGeometry;
	}

	return nullptr;
	
}//end CGeometryManager::LoadObject

//Jenkins hash function.
int CGeometryManager::Hash(const char input[100], const UINT32 length)
{
	int hash = 0;
    for(UINT32 i = 0; i < length; i++)
    {
        hash += input[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);   //XOR
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);	//XOR
    hash += (hash << 15);
    return hash;
}