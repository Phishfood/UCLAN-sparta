#include "Scene.h"

void TW_CALL ReloadSettings( void* pointer );
void TW_CALL WriteSettings(void* pointer);

void TW_CALL CalculateWallIM( void* pointer );
void TW_CALL CalculatePath( void* pointer );
void TW_CALL TWVisualisePath(void* pointer);
void TW_CALL TwPlaceHeavyTurret(void* pointer);
void TW_CALL TwPlaceMediumTurret(void* pointer);
void TW_CALL TwPlaceLightTurret(void* pointer);
void TW_CALL TwVisualiseTurrets(void* pointer);

void TW_CALL WriteMapXML(void* pointer);