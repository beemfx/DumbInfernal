// (c) 2018 Beem Media

#include "EGEngineApp.h"
#include "EGWindowsAPI.h"
#include "EGEngine.h"
#include "DeWorldRenderPipe.h"
#include "EGGlobalConfig.h"
#include "EGEngineConfig.h"

#define EGRFL_SYSTEM_HEADER "DeGame.link.reflection.hpp"
#include "EGRflLinkSystem.hpp"
#undef EGRFL_SYSTEM_HEADER

int WINAPI WinMain( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow)
{
	unused( hInstance , hPrevInstance , lpCmdLine , nCmdShow );

	egSdkEngineInitParms InitParms( CT_Clear );
	InitParms.ServerClassName = "EGServer";
	InitParms.ClientClassName = "DeClient";
	InitParms.WorldRenderPipeClass = &DeWorldRenderPipe::GetStaticClass();
	InitParms.EngineInstClassName = "DeEngineInst";
	InitParms.GameClassName = "DeGame";
	InitParms.GameName = "DeGame";
	InitParms.bAllowConsole = Engine_IsEditor();
	InitParms.bAllowDedicatedServer = false;
	InitParms.bIsDedicatedServer = false;

	VideoConfig_PostFXAA.SetValue( false );

#if IS_PLATFORM_Steam
	GlobalConfig_PlatformAppId.SetValue( "1208370" );
#endif

	int Res = EGEngineApp_Run( InitParms );
	return Res;
}