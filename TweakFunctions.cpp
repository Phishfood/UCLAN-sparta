#include "TweakFunctions.h"

void TW_CALL ReloadSettings( void* pointer )
{
	SettingsManager* sm = static_cast<SettingsManager*>(pointer);
	sm->Load( L".\\Settings.ini" );
}

void TW_CALL WriteSettings( void* pointer )
{
	SettingsManager* sm = static_cast<SettingsManager*>(pointer);
	sm->Save( L".\\Settings.ini" );
}

void TW_CALL CalculateWallIM( void* pointer )
{
	MapSquare* map = static_cast<MapSquare*>(pointer);
	map->mp_timer->Reset();
	map->mp_timer->Start();

	map->CalcWallInfluence();
	
	map->mf_calcTime = map->mp_timer->GetLapTime();
	map->mp_timer->Stop();
}

void TW_CALL CalculatePath( void* pointer )
{
	MapSquare* map = static_cast<MapSquare*>(pointer);
	map->mp_timer->Reset();
	map->mp_timer->Start();

	map->FindPath();

	map->mf_calcTime = map->mp_timer->GetLapTime();
	map->mp_timer->Stop();
}

void TW_CALL TWVisualisePath(void* pointer)
{
	CScene* scene = static_cast<CScene*>(pointer);
	scene->VisualisePath();
}