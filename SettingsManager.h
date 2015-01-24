#pragma once

#include "Defines.h"

#define SCHECK(x) if(x==0) return false

//buffer size for handling numerical data.
const uint32_t BSIZE = 10;

//length of the buffer to read file names into.
const uint32_t FNAME_LENG = 30;

class SettingsManager
{
public:
	uint32_t resolutionX;
	uint32_t resolutionY;

	wchar_t nodeModel[FNAME_LENG];
	float nodeScale;
	wchar_t splineModel[FNAME_LENG];
	float splineScale;

	uint32_t maxMapSize;

	uint32_t wallRange;
	uint32_t baseRange;
	uint32_t LTRange;
	uint32_t MTRange;
	uint32_t HTRange;
	
private:
public:
	SettingsManager();
	~SettingsManager();
	bool Load(wchar_t fileName[]);
	bool Save(wchar_t fileName[]);
private:
};

