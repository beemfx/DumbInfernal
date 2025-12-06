// (c) 2019 Beem Media

#include "DeImpEnt.h"
#include "DeGame.h"
#include "EGMeshComponent.h"

EG_CLASS_DECL( DeImpEnt )

void DeImpEnt::OnEnterWorld()
{
	Super::OnEnterWorld();

	DeGame* Game = GetGame();

	if( IsServer() )
	{
		SetActive( true );
		if( Game )
		{
			m_Target = Game->GetImpTarget();
		}
	}

	if( IsClient() )
	{
		EGMeshComponent* FaceComponent = GetComponentById<EGMeshComponent>( eg_crc("Face") );
		if( FaceComponent && Game )
		{
			FaceComponent->SetPalette( Game->GetRandomImpColor().ToVec4() );
		}
	}

	if( Game )
	{
		Game->AddImpEnt( this );
	}
}

void DeImpEnt::OnLeaveWorld()
{
	DeGame* Game = GetGame();
	if( Game )
	{
		Game->RemoveImpEnt( this );
	}
	
	Super::OnLeaveWorld();
}

void DeImpEnt::OnUpdate( eg_real DeltaTime )
{
	Super::OnUpdate( DeltaTime );

	if( IsServer() )
	{
		DeGame* Game = GetGame();
		if( Game && !Game->IsGamePaused() )
		{
			eg_vec4 Target = m_Target.GetPosition();
			eg_transform Pose = GetPose();
			eg_vec4 MoveDir = Target - Pose.GetPosition();

			if( MoveDir.LenSqAsVec3() < (m_DistanceToAttack*m_DistanceToAttack) )
			{
				DeleteFromWorld();
				Game->HurtPlayers( 10 );
			}
			else
			{
				MoveDir.NormalizeThisAsVec3();
				MoveDir *= m_Speed*DeltaTime;
				Pose.TranslateThis( MoveDir.XYZ_ToVec3() );
				MoveToPose( Pose );
			}
		}
	}
}
