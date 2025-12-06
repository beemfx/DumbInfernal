// (c) 2019 Beem Media

#include "DeHudWidget.h"
#include "DeHud.h"

EG_CLASS_DECL( DeHudWidget )

void DeHudWidget::Show( eg_bool bImmediate )
{
	RunEvent( bImmediate ? eg_crc("ShowNow") : eg_crc("Show") );
}

void DeHudWidget::Hide( eg_bool bImmediate )
{
	RunEvent( bImmediate ? eg_crc("HideNow") : eg_crc("Hide") );
}

DeHud* DeHudWidget::GetHud()
{
	return EGCast<DeHud>(GetOwnerMenu());
}

const DeHud* DeHudWidget::GetHud() const
{
	return EGCast<DeHud>(GetOwnerMenu());
}

DePlayerEnt* DeHudWidget::GetOwningPlayer()
{
	DeHud* Hud = GetHud();
	return Hud ? Hud->GetOwningPlayer() : nullptr;
}

const DePlayerEnt* DeHudWidget::GetOwningPlayer() const
{
	const DeHud* Hud = GetHud();
	return Hud ? Hud->GetOwningPlayer() : nullptr;
}

DeGame* DeHudWidget::GetGame()
{
	DeHud* Hud = GetHud();
	return Hud ? Hud->GetGame() : nullptr;
}

const DeGame* DeHudWidget::GetGame() const
{
	const DeHud* Hud = GetHud();
	return Hud ? Hud->GetGame() : nullptr;
}
