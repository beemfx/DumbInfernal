// (c) 2018 Beem Media

#pragma once

#include "EGEnt.h"
#include "EGInputTypes.h"

class DeGame;

class DeEnt : public EGEnt
{
	EG_CLASS_BODY( DeEnt , EGEnt )

protected:

	DeGame* GetGame();
	const DeGame* GetCame() const;
	egLockstepCmds GetInputForPlayer( const eg_lockstep_id& LockstepId ) const;
};
