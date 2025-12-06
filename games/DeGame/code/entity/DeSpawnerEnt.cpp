// (c) 2018 Beem Media

#include "DeSpawnerEnt.h"
#include "EGVisualComponent.h"
#include "EGParse.h"
#include "DeGame.h"

EG_CLASS_DECL( DeSpawnerEnt )

void DeSpawnerEnt::OnCreate( const egEntCreateParms& Parms )
{
	Super::OnCreate( Parms );

	auto ParseHandler = [this]( egParseFuncBase& Info ) -> void
	{
		if( EGString_EqualsI( Info.FunctionName , "SetType") && Info.NumParms >= 1  )
		{
			if( EGString_EqualsI( Info.Parms[0] , "PlayerStart" ) )
			{
				m_SpawnerType = de_spawner_ent_t::PlayerStart;
			}
			else if( EGString_EqualsI( Info.Parms[0] , "MainMenuCameraPose" ) )
			{
				m_SpawnerType = de_spawner_ent_t::MainMenuCameraPose;
			}
			else if( EGString_EqualsI( Info.Parms[0] , "ImpSpawner" ) )
			{
				m_SpawnerType = de_spawner_ent_t::ImpSpawner;
				if( Info.NumParms >= 2 )
				{
					m_EntSpawnerSeed = EGString_ToInt( Info.Parms[1] );
				}
			}
			else
			{
				m_SpawnerType = de_spawner_ent_t::Unknown;
			}
		}
		else
		{
			EGLogf( eg_log_t::Warning , "Invalid command in Spawner creation: %s" , Info.FunctionName );
		}
	};

	EGParse_ProcessFnCallScript( *Parms.InitString , Parms.InitString.Len() , ParseHandler );
}

void DeSpawnerEnt::OnEnterWorld()
{
	Super::OnEnterWorld();

	if( GetRole() != eg_ent_role::EditorPreview )
	{
		// In anything but the preview world hide any visual components.
		m_ComponentTree.IterateComponents( []( EGComponent* Comp ) -> void
		{
			EGVisualComponent* AsVisualComponent = EGCast<EGVisualComponent>(Comp);
			if( AsVisualComponent )
			{
				AsVisualComponent->SetHidden( true );
			}
		} );
	}

	if( GetRole() == eg_ent_role::Authority )
	{
		ReplicateDataToClient( &m_SpawnerType , sizeof(m_SpawnerType) );
	}

	if( GetGame() )
	{
		GetGame()->AddSpawnerEnt( this );
	}
}

void DeSpawnerEnt::OnLeaveWorld()
{
	if( GetGame() )
	{
		GetGame()->RemoveSpawnerEnt( this );
	}

	Super::OnLeaveWorld();
}
