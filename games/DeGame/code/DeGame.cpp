// (c) 2018 Beem Media

#include "DeGame.h"
#include "EGEntWorld.h"
#include "DePlayerEnt.h"
#include "DeSpawnerEnt.h"
#include "DeClient.h"
#include "EGMenuStack.h"
#include "DeImpEnt.h"
#include "EGSettings2Types.h"

EG_CLASS_DECL( DeGame )

static EGSettingsBool DeGame_UseOriginalYouTubeDifficulty( "DeGame.UseOriginalYouTubeDifficulty" , eg_loc("UseOriginalYouTubeDifficultyText","Hard Mode (YouTube Difficulty)") , false , EGS_F_USER_SAVED|EGS_F_EDITABLE );

DeGame::DeGame()
: m_Clients( deGameClient() )
{
#define REG_BH( _bh_ ) m_REHandler.RegisterEvent( eg_crc(#_bh_) , EGREHandler2::egCall( this , &ThisClass::RE_##_bh_ ) );
	REG_BH( CreateNewGame )
	REG_BH( QuitGame )
	REG_BH( OpenPauseMenu )
	REG_BH( RestartGame )
	REG_BH( SetGamePaused )
#undef REG_BH
}

void DeGame::OnConstruct()
{
	Super::OnConstruct();
}

void DeGame::OnDestruct()
{
	Super::OnDestruct();
}

void DeGame::Init( EGEntWorld* WorldOwner )
{
	Super::Init( WorldOwner );

	if( IsServer() )
	{
		WorldOwner->LoadWorld( "/game/world/DeWorld" );
	}
}

void DeGame::Update( eg_real DeltaTime )
{
	Super::Update( DeltaTime );

	if( IsClient() )
	{
		DePlayerEnt* PlayerEnt = GetClientEnt( GetClientOwner()->GetLockstepId() );

		if( PlayerEnt )
		{
			m_CameraPose = PlayerEnt->GetCameraPose();
		}
		else
		{
			const DeSpawnerEnt* MainMenuCameraPose = GetMainMenuCameraPoseSpawner();
			m_TestCameraTime += DeltaTime*.05f;
			if( m_TestCameraTime >= EG_PI*2.f )
			{
				m_TestCameraTime -= EG_PI*2.f;
			}

			const eg_vec3 MainMenuCameraPoseVec = MainMenuCameraPose ? MainMenuCameraPose->GetPose().GetPosition().ToVec3() : eg_vec3( CT_Clear );

			m_CameraPose = eg_transform::BuildIdentity();
			m_CameraPose.RotateXThis( EG_Deg( 10.f ) );
			m_CameraPose.TranslateThis( 0.f , 0.f , -7.f );
			m_CameraPose.RotateYThis( EG_Rad( m_TestCameraTime ) );
			m_CameraPose.TranslateThis( MainMenuCameraPoseVec );
			m_CameraPose.TranslateThis( 0.f , 3.f , 0.f );	
		}
	}

	if( IsServer() && m_GameState == de_game_state::Playing  && !IsGamePaused() )
	{
		const eg_bool bOriginalDifficulty = DeGame_UseOriginalYouTubeDifficulty.GetValueThreadSafe();

		m_TimeToNextDifficulty += DeltaTime;
		if( m_TimeToNextDifficulty >= m_DifficultyInterval )
		{
			m_TimeToNextDifficulty = 0.f;
			m_Difficulty++;
		}
			
		m_TimeSinceLastSpawn += DeltaTime;
		if( m_TimeSinceLastSpawn >= m_TimeToNextSpawn )
		{
			m_TimeSinceLastSpawn = 0.f;
			m_TimeToNextSpawn = 1.f;
			eg_int NumImpsToSpawn = 0;
			if( bOriginalDifficulty )
			{
				NumImpsToSpawn = m_Rng.GetRandomRangeI( EG_Max<eg_int>(1,m_Difficulty/2) , EG_Max<eg_int>(1,m_Difficulty) );
			}
			else
			{
				NumImpsToSpawn = m_Rng.GetRandomRangeI( 1 , EG_Max<eg_int>(1,EGMath_floor(EGMath_sqrt(EG_To<eg_real>(m_Difficulty)))) );
			}

			for( eg_int i=0; i<NumImpsToSpawn; i++ )
			{
				const DeSpawnerEnt* ImpSpawner = GetRandomImpSpawner();
				if( ImpSpawner && m_ImpTracker.Len() < 50 )
				{
					SDK_SpawnEnt( eg_crc( "DeImpEnt" ), ImpSpawner->GetPose() );
				}
			}
		}
	}
}

void DeGame::OnClientEnterWorld( const eg_lockstep_id& LockstepId )
{
	Super::OnClientEnterWorld( LockstepId );

	assert( !m_Clients.Contains( LockstepId ) );

	deGameClient NewClient;
	NewClient.LockstepId = LockstepId;
	if( m_GameState == de_game_state::Playing )
	{
		CreateEntForClient( NewClient );
	}
	m_Clients.Insert( LockstepId , NewClient );

	// Replicate data to clients
	SDK_ReplicateToClient( &m_GameState , sizeof(m_GameState) );
}

void DeGame::OnClientLeaveWorld( const eg_lockstep_id& LockstepId )
{
	assert( m_Clients.Contains( LockstepId ) );

	if( m_Clients.Contains( LockstepId ) )
	{
		deGameClient& ClientInfo = m_Clients[ LockstepId ];
		if( ClientInfo.PlayerEnt.IsValid() )
		{
			ClientInfo.PlayerEnt->DeleteFromWorld();
			ClientInfo.PlayerEnt = nullptr;
		}
		m_Clients.Delete( LockstepId );
	}

	Super::OnClientLeaveWorld( LockstepId );
}

void DeGame::CreateNewGameFromClient()
{
	SDK_RunServerEvent( eg_crc("CreateNewGame") );

	EGClient* OwnerClient = GetClientOwner();
	if( OwnerClient )
	{
		OwnerClient->SDK_PlayBgMusic( "Music:GameStart" );
	}
}

void DeGame::QuitGameFromClient()
{
	assert( IsClient() );

	SDK_RunServerEvent( eg_crc("QuitGame") );

	if( IsClient() )
	{
		EGMenuStack* MenuStack = GetClientOwner()->SDK_GetMenuStack();
		assert( MenuStack->IsEmpty() ); // Should clear menu stack before calling this
		MenuStack->Push( eg_crc("MainMenu") );
	}
}

void DeGame::OpenPauseMenuFromServer( eg_lockstep_id ClientId )
{
	SDK_RunClientEvent( eg_crc("OpenPauseMenu") , eg_lockstep_id::IdToRaw( ClientId ) );
}

void DeGame::AddPlayerEnt( DePlayerEnt* Ent )
{
	assert( !m_PlayerTracker.Contains( Ent ) );
	m_PlayerTracker.Append( Ent );
}

void DeGame::RemovePlayerEnt( DePlayerEnt* Ent )
{
	assert( m_PlayerTracker.Contains( Ent ) );
	m_PlayerTracker.DeleteByItem( Ent );
}

void DeGame::AddSpawnerEnt( DeSpawnerEnt* Ent )
{
	assert( !m_SpawnerTracker.Contains( Ent ) );
	m_SpawnerTracker.Append( Ent );
}

void DeGame::RemoveSpawnerEnt( DeSpawnerEnt* Ent )
{
	assert( m_SpawnerTracker.Contains( Ent ) );
	m_SpawnerTracker.DeleteByItem( Ent );
}

void DeGame::AddImpEnt( DeImpEnt* Ent )
{
	assert( !m_ImpTracker.Contains( Ent ) );
	m_ImpTracker.Append( Ent );
}

void DeGame::RemoveImpEnt( DeImpEnt* Ent )
{
	assert( m_ImpTracker.Contains( Ent ) );
	m_ImpTracker.DeleteByItem( Ent );
}

void DeGame::SetGameState( de_game_state NewGameState )
{
	assert( IsServer() );
	m_GameState = NewGameState;
	SDK_ReplicateToClient( &m_GameState , sizeof(m_GameState) );

	if( m_GameState != de_game_state::Playing )
	{
		DeImpTracker ImpTracker = m_ImpTracker;
		for( DeImpEnt* ImpEnt : ImpTracker )
		{
			ImpEnt->DeleteFromWorld();
		}
	}
}

void DeGame::CreateEntForClient( deGameClient& Client )
{
	assert( !Client.PlayerEnt.IsValid() );
	assert( IsServer() );

	const DeSpawnerEnt* PlayerStart = GetPlayerStartSpawner();
	Client.PlayerEnt = EGCast<DePlayerEnt>(SDK_SpawnEnt( eg_crc("DePlayer") , PlayerStart ? PlayerStart->GetPose() : CT_Default ));
	if( Client.PlayerEnt )
	{
		Client.PlayerEnt->SetOwnerId( Client.LockstepId );
	}
}

DePlayerEnt* DeGame::GetClientEnt( const eg_lockstep_id& ClientLockstepId )
{
	for( DePlayerEnt* Ent : m_PlayerTracker )
	{
		if( Ent && Ent->GetOwnerId() == ClientLockstepId )
		{
			return Ent;
		}
	}

	return nullptr;
}

eg_transform DeGame::GetMainMenuCameraTransform() const
{
	const DeSpawnerEnt* MainMenuCameraPose = GetMainMenuCameraPoseSpawner();
	return MainMenuCameraPose ? MainMenuCameraPose->GetPose() : CT_Default;
}

eg_transform DeGame::GetImpTarget()
{
	eg_transform Out(CT_Default);

	const DeSpawnerEnt* PlayerSpawnPose = GetPlayerStartSpawner();
	if( PlayerSpawnPose )
	{
		Out.SetTranslation( eg_vec3( m_Rng.GetRandomRangeF( -4.f , 4.f ) , 0.f , 0.f ) );
		Out *= PlayerSpawnPose->GetPose();
	}

	return Out;
}

void DeGame::HurtPlayers( eg_int Amount )
{
	assert( IsServer() );
	if( IsServer() )
	{
		for( eg_size_t i = 0; i < m_Clients.Len(); i++ )
		{
			deGameClient& Client = m_Clients.GetByIndex( i );
			if( Client.PlayerEnt.IsValid() )
			{
				Client.PlayerEnt->DoDamage( Amount );
			}
		}
	}
}

eg_bool DeGame::ArePlayersAlive() const
{
	eg_bool bAnyAlive = false;

	for( const DePlayerEnt* Player : m_PlayerTracker )
	{
		if( Player )
		{
			if( Player->GetHealth() > 0 )
			{
				bAnyAlive = true;
			}
		}
	}

	return bAnyAlive;
}

void DeGame::RestartGame()
{
	SDK_RunServerEvent( eg_crc("RestartGame") );
}

void DeGame::SetGamePaused( eg_bool bValue )
{
	SDK_RunServerEvent( eg_crc("SetGamePaused") , bValue );
}

eg_color DeGame::GetRandomImpColor()
{
	eg_color32 Out( CT_Default );
	Out.R = EG_To<eg_uint8>(m_Rng.GetRandomRangeI( 0 , 255 ));
	Out.G = EG_To<eg_uint8>(m_Rng.GetRandomRangeI( 0 , 255 ));
	Out.B = EG_To<eg_uint8>(m_Rng.GetRandomRangeI( 0 , 255 ));
	return eg_color(Out);
}

const DeSpawnerEnt* DeGame::GetPlayerStartSpawner() const
{
	for( DeSpawnerEnt* Ent : m_SpawnerTracker )
	{
		if( Ent && Ent->GetSpawnerType() == de_spawner_ent_t::PlayerStart )
		{
			return Ent;
		}
	}

	return nullptr;
}

const DeSpawnerEnt* DeGame::GetMainMenuCameraPoseSpawner() const
{
	for( DeSpawnerEnt* Ent : m_SpawnerTracker )
	{
		if( Ent && Ent->GetSpawnerType() == de_spawner_ent_t::MainMenuCameraPose )
		{
			return Ent;
		}
	}

	return nullptr;
}

void DeGame::RE_CreateNewGame( const eg_event_parms& Parms )
{
	unused( Parms );

	assert( IsServer() );
	assert( m_GameState == de_game_state::MainMenu );

	m_Difficulty = 0;
	m_TimeToNextDifficulty = 0.f;
	m_TimeSinceLastSpawn = 0.f;
	m_TimeToNextSpawn = 0.f;

	SetGameState( de_game_state::Playing );

	// Create entities for all the clients.
	for( eg_size_t i=0; i<m_Clients.Len(); i++ )
	{
		deGameClient& Client = m_Clients.GetByIndex( i );
		CreateEntForClient( Client );
	}
}

void DeGame::RE_QuitGame( const eg_event_parms& Parms )
{
	unused( Parms );

	assert( IsServer() );
	assert( m_GameState == de_game_state::Playing );

	// Clear any client entities
	for( eg_size_t i=0; i<m_Clients.Len();  i++ )
	{
		deGameClient& Client = m_Clients.GetByIndex( i );
		if( Client.PlayerEnt )
		{
			Client.PlayerEnt->DeleteFromWorld();
			Client.PlayerEnt = nullptr;
		}
	}

	SetGameState( de_game_state::MainMenu );
}

void DeGame::RE_OpenPauseMenu( const eg_event_parms& Parms )
{
	eg_lockstep_id TargetClientId = eg_lockstep_id::RawToId( EG_To<eg_uint>(Parms.as_int64()) );

	assert( IsClient() );
	assert( GetGameState() == de_game_state::Playing );

	if( IsClient() && GetGameState() == de_game_state::Playing )
	{
		if( GetClientOwner()->GetLockstepId() == TargetClientId )
		{
			EGMenuStack* MenuStack = GetClientOwner()->SDK_GetMenuStack();
			if( MenuStack->IsEmpty() )
			{
				MenuStack->Push( eg_crc("PauseMenu") );
			}
		}
	}
}

void DeGame::RE_RestartGame( const eg_event_parms& Parms )
{
	unused( Parms );

	assert( IsServer() );
	
	if( m_GameState == de_game_state::Playing )
	{
		m_Difficulty = 0;
		m_TimeToNextDifficulty = 0.f;
		m_TimeSinceLastSpawn = 0.f;
		m_TimeToNextSpawn = 0.f;

		// Clear imps
		DeImpTracker ImpTrackerCopy = m_ImpTracker; // Need a copy of the tracker since the real tracker will be modified as imps are deleted.
		for( DeImpEnt* Imp : ImpTrackerCopy )
		{
			if( Imp )
			{
				Imp->DeleteFromWorld();
			}
		}

		// Clear any client entities
		for( eg_size_t i = 0; i < m_Clients.Len(); i++ )
		{
			deGameClient& Client = m_Clients.GetByIndex( i );
			if( Client.PlayerEnt )
			{
				Client.PlayerEnt->DeleteFromWorld();
				Client.PlayerEnt = nullptr;
			}
		}

		// Create entities for all the clients.
		for( eg_size_t i = 0; i < m_Clients.Len(); i++ )
		{
			deGameClient& Client = m_Clients.GetByIndex( i );
			CreateEntForClient( Client );
		}
	}
}

void DeGame::RE_SetGamePaused( const eg_event_parms& Parms )
{
	assert( IsServer() );
	
	if( Parms.as_bool() )
	{
		m_PauseCount++;
	}
	else
	{
		m_PauseCount--;
	}

	assert( m_PauseCount >= 0 ); // Something didn't have a balanced pause/unpause.

	SDK_ReplicateToClient( &m_PauseCount , sizeof(m_PauseCount) );
}

const DeSpawnerEnt* DeGame::GetRandomImpSpawner()
{
	for( eg_int i=0; i<100; i++ )
	{
		eg_int RandomIndex = m_Rng.GetRandomRangeI( 0 , m_SpawnerTracker.LenAs<eg_int>() );
		if( m_SpawnerTracker.IsValidIndex( RandomIndex ) && m_SpawnerTracker[RandomIndex] && m_SpawnerTracker[RandomIndex]->GetSpawnerType() == de_spawner_ent_t::ImpSpawner )
		{
			return m_SpawnerTracker[RandomIndex];
		}
	}

	return nullptr;
}
