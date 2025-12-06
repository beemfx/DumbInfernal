// (c) 2018 Beem Media

#include "DePlayerEnt.h"
#include "DeGame.h"
#include "EGInputTypes.h"
#include "DeInput.h"
#include "EGDebugText.h"
#include "EGEntWorld.h"
#include "DeTargetWidgetEnt.h"
#include "EGComponent.h"
#include "DeClient.h"
#include "DeHud.h"
#include "DeImpEnt.h"
#include "EGSettings2Types.h"
#include "EGPlatform.h"
#include "DeProfileData.h"

static EGSettingsBool DePlayerEnt_InvertYAxis( "DePlayerEnt_InvertYAxis" , eg_loc("DeInvertYAxis","Invert Y Axis") , false , EGS_F_USER_SAVED|EGS_F_EDITABLE );
static EGSettingsBool DePlayerEnt_IsImmortal( "DePlayerEnt_IsImmortal" , eg_loc("IsPlayerImmortalText","DEBUG: Is Player Immortal") , false , EGS_F_DEBUG_EDITABLE|EGS_F_SYS_SAVED );
static eg_string_crc DePlayerEnt_ScoreText = eg_loc("ScoreText","Score: {0}");
static eg_string_crc DePlayerEnt_HealthText = eg_loc("HealthText","{0}%");

EG_CLASS_DECL( DePlayerEnt )

void DePlayerEnt::OnEnterWorld()
{
	Super::OnEnterWorld();

	GetGame()->AddPlayerEnt( this );

	if( GetRole() == eg_ent_role::Authority )
	{
		SetActive( true );

		// Since we actually track the player position by m_FaceDirDeg we need to
		// calculate what that is from the pose.
		eg_transform StartPose = GetPose();
		eg_vec4 ForwardVec = eg_vec4(0.f,0.f,1.f,0.f) * StartPose;

		m_FaceDirDeg = EGMath_atan2( ForwardVec.x , ForwardVec.z ).ToDeg();

		m_TargetWidget = EGCast<DeTargetWidgetEnt>(GetWorld()->SpawnEnt( eg_crc("DeTargetWidget") , GetPose() ));
	}
	else
	{
		SetActive( true );
	}

	if( IsClient() )
	{
		DeClient* Client = EGCast<DeClient>(GetOwnerClient());
		if( Client )
		{
			assert( !m_Hud.IsValid() );
			
			m_Hud = DeHud::CreateHud( Client , this );
		}

		RunEvent( eg_crc("GunDefault") );
	}
}

void DePlayerEnt::OnLeaveWorld()
{
	if( IsClient() )
	{
		DeClient* Client = EGCast<DeClient>(GetOwnerClient());
		if( Client )
		{
			assert( m_Hud.IsValid() );
			Client->SDK_RemoveOverlay( m_Hud.GetObject() );
			assert( !m_Hud.IsValid() );
			m_Hud = nullptr;
		}
	}

	if( m_TargetWidget )
	{
		assert( GetRole() == eg_ent_role::Authority );
		m_TargetWidget->DeleteFromWorld();
		m_TargetWidget = nullptr;
	}

	GetGame()->RemovePlayerEnt( this );

	Super::OnLeaveWorld();
}

void DePlayerEnt::OnUpdate( eg_real DeltaTime )
{
	Super::OnUpdate( DeltaTime );

	if( GetRole() == eg_ent_role::Authority )
	{
		eg_transform FinalPose = GetPose();

		HandleInputForUpdate( DeltaTime , FinalPose );
		HandleTargettingForUpdate( DeltaTime );

		ReplicateDataToClient( &m_PlayerState , sizeof(m_PlayerState) );

		MoveToPose( FinalPose );
	}
	
	if( IsClient() )
	{
		HandleLookInputForUpdate( DeltaTime );

		// Handle achievements
		if( !m_bUnlocked001 && m_Score >= 1 )
		{
			m_bUnlocked001 = true;
			EGPlatform_GetPlatform().UnlockAchievement( "ACH_KILLS_001" );
		}

		if( !m_bUnlocked010 && m_Score >= 10 )
		{
			m_bUnlocked010 = true;
			EGPlatform_GetPlatform().UnlockAchievement( "ACH_KILLS_010" );
		}

		if( !m_bUnlocked100 && m_Score >= 100 )
		{
			m_bUnlocked100 = true;
			EGPlatform_GetPlatform().UnlockAchievement( "ACH_KILLS_100" );
		}

		if( !m_bUnlocked200 && m_Score >= 200 )
		{
			m_bUnlocked200 = true;
			EGPlatform_GetPlatform().UnlockAchievement( "ACH_KILLS_200" );
		}

		if( m_PlayerState == de_player_ent_s::Dead )
		{
			DeClient* Client = EGCast<DeClient>(GetOwnerClient());
			DeProfileData* ProfileData = Client ? Client->GetProfileData() : nullptr;
			if( ProfileData && m_Score > ProfileData->GetHighScore() )
			{
				ProfileData->AddNewScore( m_Score );
			}
		}
	}
}

void DePlayerEnt::SetOwnerId( const eg_lockstep_id& InOwnerId )
{
	m_ClientOwnerId = InOwnerId;
	ReplicateDataToClient( &m_ClientOwnerId , sizeof(m_ClientOwnerId) );
}

eg_transform DePlayerEnt::GetCameraPose() const
{
	return GetEyeCamera();
}

void DePlayerEnt::DoDamage( eg_int Amount )
{
	const eg_bool bImmortal = IS_DEBUG && DePlayerEnt_IsImmortal.GetValue();
	if( m_NumHits > 0 && !bImmortal )
	{
		m_NumHits -= Amount;
		ReplicateDataToClient( &m_NumHits, sizeof( m_NumHits ) );
		RunEvent( m_NumHits > 0 ? eg_crc("GotHit") : eg_crc("Died") );
		if( m_NumHits <= 0 )
		{
			m_NumHits = 0;
			ReplicateDataToClient( &m_NumHits, sizeof( m_NumHits ) );
			m_PlayerState = de_player_ent_s::Dead;
			ReplicateDataToClient( &m_PlayerState, sizeof( m_PlayerState ) );
		}
	}
}

eg_int DePlayerEnt::GetHighScore() const
{
	eg_int Out = -1;
	if( IsClient() )
	{
		DeClient* Client = EGCast<DeClient>(const_cast<DePlayerEnt*>(this)->GetOwnerClient());
		DeProfileData* ProfileData = Client ? Client->GetProfileData() : nullptr;
		if( ProfileData )
		{
			Out = EG_Max( m_Score , ProfileData->GetHighScore() );
		}
	}
	return Out;
}

void DePlayerEnt::HandleInputForUpdate( eg_real DeltaTime , eg_transform& FinalPose )
{
	switch( m_PlayerState )
	{
		case de_player_ent_s::Default:
			HandleInputForUpdate_Default( DeltaTime , FinalPose );
			break;
		case de_player_ent_s::Dead:
			break;
	}

	const deInput Cmds = GetInputForPlayer( m_ClientOwnerId );

	if( Cmds.WasPausePressed() )
	{
		GetGame()->OpenPauseMenuFromServer( m_ClientOwnerId );
	}
}

void DePlayerEnt::HandleLookInputForUpdate( eg_real DeltaTime )
{
	unused( DeltaTime );
	
	if( m_ClientOwnerId == GetOwnerClient()->GetLockstepId() )
	{
		if( HANDLE_LOOK_ON_CLIENT )
		{
			if( m_PlayerState == de_player_ent_s::Default )
			{
				HandleLookUpdate( DeltaTime );
			}
		}
	}
}

void DePlayerEnt::HandleInputForUpdate_Default( eg_real DeltaTime, eg_transform& FinalPose )
{
	unused( DeltaTime );

	const deInput Cmds = GetInputForPlayer( m_ClientOwnerId );

	eg_vec4 Position = FinalPose.GetPosition();

	if( !HANDLE_LOOK_ON_CLIENT )
	{
		HandleLookUpdate( DeltaTime );
	}

	// EGLogToScreen::Get().Log( this , __LINE__ , 1.f , EGString_Format( "Position: %g %g %g" , Position.x , Position.y , Position.z ) );
	// EGLogToScreen::Get().Log( this , __LINE__ , 1.f , EGString_Format( "Velocity: %g %g %g" , m_CurMoveVel.x , m_CurMoveVel.y , m_CurMoveVel.z ) );

	// Build the final pose.
	eg_transform Rotation = eg_transform::BuildRotationY( EG_Deg( m_FaceDirDeg ) );
	FinalPose = Rotation;
	FinalPose.TranslateThis( Position.XYZ_ToVec3() );

	if( Cmds.WasShootPressed() )
	{
		HandleShooting();
	}
}

void DePlayerEnt::HandleTargettingForUpdate( eg_real DeltaTime )
{
	unused( DeltaTime );

	EGComponent* FaceComp = GetComponentById<EGComponent>( eg_crc("EyeCamera") );
	eg_transform RcSrc = GetPose();
	if( FaceComp )
	{
		RcSrc = FaceComp->GetWorldPose( RcSrc );
	}
	eg_vec4 RcOrg = RcSrc.GetPosition();
	eg_vec4 RcDir = eg_vec4(0.f,0.f,1.f,0.f) * eg_transform::BuildRotationX( EG_Deg( -m_LookYawDeg ) ) * RcSrc;

	// EGLogToScreen::Get().Log( this , __LINE__ , 1.f , EGString_Format( "Ray Dir: %g %g %g" , RcDir.x , RcDir.y , RcDir.z ) );

	if( m_TargetWidget )
	{
		egEntWorldRaycastResult RcRes = GetWorld()->RaycastWorld( RcOrg , RcDir );
		m_TargetWidget->SetPose( eg_transform::BuildTranslation( RcRes.HitPoint.ToVec3() ) );
	}
}

void DePlayerEnt::HandleShooting()
{
	RunEvent( eg_crc("GunMuzzleFlash") );

	EGComponent* FaceComp = GetComponentById<EGComponent>( eg_crc( "EyeCamera" ) );
	eg_transform RcSrc = GetPose();
	if( FaceComp )
	{
		RcSrc = FaceComp->GetWorldPose( RcSrc );
	}
	eg_vec4 RcOrg = RcSrc.GetPosition();
	eg_vec4 RcDir = eg_vec4( 0.f, 0.f, 1.f, 0.f ) * eg_transform::BuildRotationX( EG_Deg( -m_LookYawDeg ) ) * RcSrc;

	// EGLogToScreen::Get().Log( this , __LINE__ , 1.f , EGString_Format( "Ray Dir: %g %g %g" , RcDir.x , RcDir.y , RcDir.z ) );

	const egEntWorldRaycastResult RcRes = GetWorld()->RaycastWorld( RcOrg, RcDir );
	if( RcRes.HitEnt != INVALID_ID )
	{
		DeGame* Game = GetGame();
		DeImpEnt* HitImp = EGCast<DeImpEnt>(Game->SDK_GetEnt( RcRes.HitEnt ));
		if( HitImp )
		{
			HitImp->DeleteFromWorld();
			m_Score += 1;
			RunEvent( eg_crc("Scored") );
			ReplicateDataToClient( &m_Score , sizeof(m_Score) );
		}
	}
}

void DePlayerEnt::HandleLookUpdate( eg_real DeltaTime )
{	
	unused( DeltaTime );
	
	// EGLogToScreen::Get().Log( this , __LINE__ , 1.f , *EGSFormat8( "Look Delta: {0}" , DeltaTime ) );

	const eg_real LookFactor = DeltaTime*75.f;

	const deInput Cmds = GetInputForPlayer( IsServer() ? m_ClientOwnerId : INVALID_ID );

	// Calculate the new direction the player is facing.
	m_FaceDirDeg += Cmds.GetAxisX() * LookFactor;

	m_FaceDirDeg = EG_Clamp( m_FaceDirDeg , -MaxLookDegree , MaxLookDegree );

	// Calculate the new direction the player is looking.
	m_LookYawDeg += Cmds.GetAxisY()*(DePlayerEnt_InvertYAxis.GetValueThreadSafe()?-1.f:1.f)*LookFactor;
	m_LookYawDeg = EG_Clamp( m_LookYawDeg , -MaxPitch , MaxPitch );
	if( IsServer() && !IsClient() )
	{
		ReplicateDataToClient( &m_LookYawDeg , sizeof(m_LookYawDeg) );
		ReplicateDataToClient( &m_FaceDirDeg , sizeof(m_FaceDirDeg) );
	}
	else if( IsClient() && !IsServer() )
	{
		ReplicateDataToServer( &m_LookYawDeg , sizeof(m_LookYawDeg) );
		ReplicateDataToServer( &m_FaceDirDeg , sizeof(m_FaceDirDeg) );
	}
}

eg_transform DePlayerEnt::GetEyeCamera() const
{
	eg_bool bThirdPerson = false;
	eg_transform CameraPose = CT_Default;

	eg_transform LookPose = eg_transform::BuildRotationX( EG_Deg( -m_LookYawDeg ) );

	if( bThirdPerson )
	{
		CameraPose = eg_transform::BuildTranslation( 0.f , 1.5 , -5.f ) * LookPose * GetPose();
	}
	else
	{
		EGComponent* FaceComp = GetComponentById<EGComponent>( eg_crc("EyeCamera") );
		if( FaceComp )
		{
			CameraPose = LookPose * FaceComp->GetWorldPose( GetPose() );
		}
		else
		{
			CameraPose = LookPose * GetPose();
		}
	}
	return CameraPose;
}
