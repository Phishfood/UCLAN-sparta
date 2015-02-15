#include "SettingsManager.h"


SettingsManager::SettingsManager()
{
}


SettingsManager::~SettingsManager()
{
}

bool SettingsManager::Load( wchar_t fileName[] )
{
	wchar_t buffer[BSIZE];
	uint32_t count;

	  //////////////////////////////////////////
	 // GRAPHICS                             //
	//////////////////////////////////////////

	count = GetPrivateProfileString(L"Graphics", L"Width", NULL, buffer, BSIZE, fileName);
	SCHECK(count);
	resolutionX = _wtoi(buffer);

	count = GetPrivateProfileString(L"Graphics", L"Height", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	resolutionY = _wtoi( buffer );

	count = GetPrivateProfileString(L"Graphics", L"NodeModel", NULL, nodeModel, FNAME_LENG, fileName);
	SCHECK( count );

	count = GetPrivateProfileString(L"Graphics", L"SplineModel", NULL, splineModel, FNAME_LENG, fileName);
	SCHECK( count );

	count = GetPrivateProfileString(L"Graphics", L"NodeScale", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	nodeScale = static_cast<float>( _wtof(buffer) );

	count = GetPrivateProfileString(L"Graphics", L"SplineScale", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	splineScale = static_cast<float>( _wtof(buffer) );

	  //////////////////////////////////////////
	 // AI                                   //
	//////////////////////////////////////////

	count = GetPrivateProfileString(L"AI", L"MaxMapSize", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	maxMapSize = _wtoi( buffer );

	count = GetPrivateProfileString(L"AI", L"WallRange", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	wallRange = _wtoi( buffer );

	count = GetPrivateProfileString(L"AI", L"BaseRange", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	baseRange = _wtoi( buffer );

	count = GetPrivateProfileString(L"AI", L"LTRange", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	LTRange = _wtoi( buffer );

	count = GetPrivateProfileString(L"AI", L"MTRange", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	MTRange = _wtoi( buffer );

	count = GetPrivateProfileString(L"AI", L"HTRange", NULL, buffer, BSIZE, fileName);
	SCHECK( count );
	HTRange = _wtoi( buffer );

	count = GetPrivateProfileString(L"AI", L"MinPathLength", NULL, buffer, BSIZE, fileName);
	SCHECK(count);
	MinPathLength = _wtoi(buffer);

	count = GetPrivateProfileString(L"AI", L"MaxPathLength", NULL, buffer, BSIZE, fileName);
	SCHECK(count);
	MaxPathLength = static_cast<float>( _wtof(buffer) );

	count = GetPrivateProfileString(L"AI", L"ChokeThreshold", NULL, buffer, BSIZE, fileName);
	SCHECK(count);
	ChokeThreshold = _wtoi(buffer);

	count = GetPrivateProfileString(L"Other", L"Map", NULL, mapTemp, FNAME_LENG, fileName);
	SCHECK(count);
	char ch[FNAME_LENG];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, mapTemp, -1, ch, FNAME_LENG, &DefChar, NULL);
	mapFile = std::string(ch);

	return true;
}

bool SettingsManager::Save( wchar_t fileName[] )
{
	wchar_t buffer[BSIZE];

	  //////////////////////////////////////////
	 // GRAPHICS                             //
	//////////////////////////////////////////

	swprintf(buffer, BSIZE, L"%d", resolutionX);
	WritePrivateProfileString(L"Graphics", L"Width", buffer, fileName);
	
	swprintf(buffer, BSIZE, L"%d", resolutionY);
	WritePrivateProfileString( L"Graphics", L"Height", buffer, fileName );

	swprintf(buffer, BSIZE, L"%f", nodeScale);
	WritePrivateProfileString(L"Graphics", L"NodeScale", buffer, fileName);

	swprintf(buffer, BSIZE, L"%f", splineScale);
	WritePrivateProfileString(L"Graphics", L"SplineScale", buffer, fileName);

	WritePrivateProfileString(L"Graphics", L"NodeModel", nodeModel, fileName);

	WritePrivateProfileString(L"Graphics", L"SplineModel", splineModel, fileName);

	  //////////////////////////////////////////
	 // AI                                   //
	//////////////////////////////////////////

	swprintf(buffer, BSIZE, L"%d", maxMapSize);
	WritePrivateProfileString( L"AI", L"MaxMapSize", buffer, fileName );

	swprintf(buffer, BSIZE, L"%d", wallRange);
	WritePrivateProfileString( L"AI", L"WallRange", buffer, fileName );

	swprintf(buffer, BSIZE, L"%d", baseRange);
	WritePrivateProfileString( L"AI", L"BaseRange", buffer, fileName );

	swprintf(buffer, BSIZE, L"%d", LTRange);
	WritePrivateProfileString( L"AI", L"LTRange", buffer, fileName );

	swprintf(buffer, BSIZE, L"%d", MTRange);
	WritePrivateProfileString( L"AI", L"MTRange", buffer, fileName );

	swprintf(buffer, BSIZE, L"%d", HTRange);
	WritePrivateProfileString( L"AI", L"HTRange", buffer, fileName );

	swprintf(buffer, BSIZE, L"%d", MinPathLength);
	WritePrivateProfileString(L"AI", L"MinPathLength", buffer, fileName);

	swprintf(buffer, BSIZE, L"%d", ChokeThreshold);
	WritePrivateProfileString(L"AI", L"ChokeThreshold", buffer, fileName);
	return true;
}