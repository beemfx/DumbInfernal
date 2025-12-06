// (c) 2019 Beem Media

#pragma once

#include "EGUiMeshWidget.h"

class DeHud;
class DePlayerEnt;
class DeGame;

class DeHudWidget : public EGUiMeshWidget
{
	EG_CLASS_BODY( DeHudWidget , EGUiMeshWidget )

public:

	virtual void Show( eg_bool bImmediate );
	virtual void Hide( eg_bool bImmediate );

	DeHud* GetHud();
	const DeHud* GetHud() const;
	DePlayerEnt* GetOwningPlayer();
	const DePlayerEnt* GetOwningPlayer() const;
	DeGame* GetGame();
	const DeGame* GetGame() const;
};
