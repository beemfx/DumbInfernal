// (c) 2019 Beem Media

#pragma once

#include "DeEnt.h"

class DeImpEnt : public DeEnt
{
	EG_CLASS_BODY( DeImpEnt , DeEnt )

protected:

	eg_real m_Speed = 10.f;
	eg_real m_DistanceToAttack = 2.f;
	eg_transform m_Target;

public:

	virtual void OnEnterWorld() override;
	virtual void OnLeaveWorld() override;
	virtual void OnUpdate( eg_real DeltaTime ) override;
};
