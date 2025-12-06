// (c) 2018 Beem Media

#include "DeEngineInst.h"
#include "DeInput.h"
#include "EGGlobalConfig.h"

EG_CLASS_DECL( DeEngineInst )

void DeEngineInst::OnGameEvent( eg_game_event EventType )
{
	switch( EventType )
	{
		case EGEngineInst::eg_game_event::INIT:
		{
			VideoConfig_PostFXAA.SetValue( false ); // In case a previous version already had saved this as true.
		} break;
		case EGEngineInst::eg_game_event::DEINIT:
		{

		} break;
	}
}

void DeEngineInst::OnRegisterInput( class ISdkInputRegistrar* Registrar )
{
	DeInput_RegisterInput( Registrar );
}

void DeEngineInst::SetupViewports( eg_real ScreenAspect , egScreenViewport* aVpOut , eg_uint VpCount ) const
{
	unused( ScreenAspect , aVpOut , VpCount );
}
