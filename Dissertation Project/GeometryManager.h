#pragma once

#include "Defines.h"

//#include <vector>
#include <list>

#include "Geometry.h"

class CGeometryManager
{
public:
private:
	static const int MAX_MESHES = 20;
	//CGeometry* meshes[MAX_MESHES];
	std::list<CGeometry*> meshes[MAX_MESHES];
	int numMeshes;
public:
	CGeometryManager(void);
	~CGeometryManager(void);
private:
	CGeometry* LoadObject(const char file[100], const int length);
	int Hash(const char input[100], const UINT32 length);
};

