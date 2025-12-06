// (c) 2018 Beem Media

#pragma once

#include "DeEnt.h"

class DeTargetWidgetEnt : public DeEnt
{
	EG_CLASS_BODY( DeTargetWidgetEnt , DeEnt )

protected:

	virtual void OnEnterWorld() override;
	virtual void OnLeaveWorld() override;

	void OnDebugVisibleChanged();

	void HandleComponentVisibility();

};
