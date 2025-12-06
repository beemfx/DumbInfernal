// (c) 2018 Beem Media

#pragma once

#include "DeEnt.h"
#include "EGRandom.h"

class DeTargetWidgetEnt;
class DeHud;

enum class de_player_ent_s
{
	Default,
	Dead,
};

class DePlayerEnt : public DeEnt
{
	EG_CLASS_BODY( DePlayerEnt , DeEnt )

private:

	static const eg_bool HANDLE_LOOK_ON_CLIENT = true;
	const eg_real MaxLookDegree = 60.f;
	const eg_real MaxPitch = 30.f;

private:

	de_player_ent_s              m_PlayerState     = de_player_ent_s::Default;
	eg_lockstep_id               m_ClientOwnerId   = INVALID_ID;
	eg_real                      m_FaceDirDeg      = 0.f;
	eg_real                      m_LookYawDeg      = 0.f;
	EGWeakPtr<DeTargetWidgetEnt> m_TargetWidget    = nullptr;
	EGWeakPtr<DeHud>             m_Hud             = nullptr;
	EGRandom                     m_Rng             = CT_Preserve;
	eg_int                       m_NumHits         = 100;
	eg_int                       m_Score           = 0;
	eg_bool                      m_bUnlocked001    = false;
	eg_bool                      m_bUnlocked010    = false;
	eg_bool                      m_bUnlocked100    = false;
	eg_bool                      m_bUnlocked200    = false;


public:

	virtual void OnEnterWorld() override;
	virtual void OnLeaveWorld() override;
	virtual void OnUpdate( eg_real DeltaTime ) override;

	void SetOwnerId( const eg_lockstep_id& InOwnerId );
	const eg_lockstep_id& GetOwnerId() const { return m_ClientOwnerId; }

	eg_transform GetCameraPose() const;

	void DoDamage( eg_int Amount );

	eg_int GetHealth() const { return m_NumHits; }
	eg_int GetScore() const { return m_Score; }
	eg_int GetHighScore() const;
	de_player_ent_s GetPlayerState() const { return m_PlayerState; }

private:

	void HandleInputForUpdate( eg_real DeltaTime , eg_transform& FinalPose );
	void HandleLookInputForUpdate( eg_real DeltaTime );
	void HandleInputForUpdate_Default( eg_real DeltaTime , eg_transform& FinalPose );
	void HandleTargettingForUpdate( eg_real DeltaTime );
	void HandleShooting();

	void HandleLookUpdate( eg_real DeltaTime );

	eg_transform GetEyeCamera() const;
};
