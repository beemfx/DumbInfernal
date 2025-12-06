// (c) 2018 Beem Media

#include "DeEnt.h"
#include "DeGame.h"

EG_CLASS_DECL( DeEnt )

DeGame* DeEnt::GetGame()
{
	return EGCast<DeGame>(SDK_GetGame());
}

const DeGame* DeEnt::GetCame() const
{
	return EGCast<DeGame>(SDK_GetGame());
}

egLockstepCmds DeEnt::GetInputForPlayer( const eg_lockstep_id& LockstepId ) const
{
	egLockstepCmds Cmds;
	SDK_GetGame()->SDK_GetCommandsForPlayer( LockstepId, &Cmds );
	return Cmds;
}
