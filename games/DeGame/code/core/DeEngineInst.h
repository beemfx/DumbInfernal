// (c) 2018 Beem Media

#pragma once

#include "EGEngineInst.h"

class DeEngineInst : public EGEngineInst
{
	EG_CLASS_BODY( DeEngineInst , EGEngineInst )

public:

	virtual void OnGameEvent( eg_game_event EventType ) override;
	virtual void OnRegisterInput( class ISdkInputRegistrar* Registrar ) override;
	virtual void SetupViewports( eg_real ScreenAspect, egScreenViewport* aVpOut, eg_uint VpCount ) const override;
};
