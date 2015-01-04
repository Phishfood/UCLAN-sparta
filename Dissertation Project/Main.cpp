#include "resource.h"

#include "Defines.h" // General definitions shared by all source files
#include "CTimer.h"  // Timer class - not DirectX
#include "Input.h"   // Input functions - not DirectX

#include "Scene.h"			// Scene class - reduces globals.

V3 XMF3ToFloat3( DirectX::XMFLOAT3 in)
{
		V3 temp;
		temp.x = in.x;
		temp.y = in.y;
		temp.z = in.z;
		return temp;
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	CScene* myScene = new CScene();
	// Initialise everything in turn
	if( !myScene->InitWindow( hInstance, nCmdShow) )
	{
		return 0;
	}
	if( !myScene->InitDevice() || !myScene->LoadEffectFile() || !myScene->InitScene() )
	{
		myScene->ReleaseResources(); // Cleanup DirectX on failure
		return 0;
	}

	// Initialise simple input functions (in Input.h/.cpp, not part of DirectX)
	InitInput();

	// Initialise a timer class (in CTimer.h/.cpp, not part of DirectX). It's like a stopwatch - start it counting now
	CTimer Timer;
	Timer.Start();

	// Main message loop. The window will stay in this loop until it is closed
	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		// First check to see if there are any messages that need to be processed for the window (window resizing, minimizing, whatever), if there are then deal with them
		// If not then the window is idle and the D3D rendering occurs. This is in a loop. So the window is rendered over and over, as fast as possible as long as we are
		// not manipulating the window in some way
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			// Deal with messages
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else // Otherwise render & update
		{
			myScene->RenderScene();

			// Get the time passed since the last frame (since the last time this line was reached) - used so the rendering and update can be
			// synchronised to real time and won't be dependent on machine speed
			float frameTime = Timer.GetLapTime();
			myScene->UpdateScene( frameTime );
		}
	}

	// Release all the resources we've created before leaving
	myScene->ReleaseResources();

	delete myScene;

	return ( int )msg.wParam;
}
