// (c) 2018 Beem Media

#pragma once

#include "DeEnt.h"

enum class de_spawner_ent_t
{
	Unknown,
	PlayerStart,
	MainMenuCameraPose,
	ImpSpawner,
};

class DeSpawnerEnt : public DeEnt
{
	EG_CLASS_BODY( DeSpawnerEnt , DeEnt )

private:

	de_spawner_ent_t m_SpawnerType;
	eg_int           m_EntSpawnerSeed;

public:

	de_spawner_ent_t GetSpawnerType() const { return m_SpawnerType; }

	virtual void OnCreate( const egEntCreateParms& Parms ) override;
	virtual void OnEnterWorld() override;
	virtual void OnLeaveWorld() override;
};
