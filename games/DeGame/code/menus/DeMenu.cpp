// (c) 2018 Beem Media

#include "DeMenu.h"
#include "DeGame.h"
#include "EGClient.h"
#include "DePlayerEnt.h"

EG_CLASS_DECL( DeMenu )

DeGame* DeMenu::GetGame()
{
	return GetOwnerClient() ? EGCast<DeGame>( GetOwnerClient()->SDK_GetGame() ) : nullptr;
}

const DeGame* DeMenu::GetGame() const
{
	return GetOwnerClient() ? EGCast<DeGame>( GetOwnerClient()->SDK_GetGame() ) : nullptr;
}
