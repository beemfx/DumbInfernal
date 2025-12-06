// (c) 2018 Beem Media

#pragma once

#include "EGMenu.h"

class DeGame;

class DeMenu : public EGMenu
{
	EG_CLASS_BODY( DeMenu , EGMenu )

public:

	DeGame* GetGame();
	const DeGame* GetGame() const;
};
