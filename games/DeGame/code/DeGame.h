// (c) 2018 Beem Media

#pragma once

#include "EGGame.h"
#include "DePlayerEnt.h"

class DePlayerEnt;
class DeSpawnerEnt;
class DeImpEnt;

enum class de_game_state
{
	MainMenu,
	Playing,
};

struct deGameClient
{
	eg_lockstep_id         LockstepId = INVALID_ID;
	EGWeakPtr<DePlayerEnt> PlayerEnt  = nullptr;
};

class DeGame : public EGGame
{
	EG_CLASS_BODY( DeGame , EGGame )

private:

	typedef EGItemMap<eg_lockstep_id,deGameClient> DeClientMap;
	typedef EGArray<DePlayerEnt*> DePlayerTracker;
	typedef EGArray<DeSpawnerEnt*> DeSpawnerTracker;
	typedef EGArray<DeImpEnt*> DeImpTracker;

private:

	eg_real          m_TestCameraTime = 0.f;
	eg_transform     m_CameraPose = CT_Default;
	DeClientMap      m_Clients; // Server only - tracks the connected clients and their ents.
	DePlayerTracker  m_PlayerTracker;
	DeSpawnerTracker m_SpawnerTracker;
	DeImpTracker     m_ImpTracker;
	de_game_state    m_GameState = de_game_state::MainMenu;
	EGRandom         m_Rng = CT_Default;
	eg_real          m_TimeSinceLastSpawn = 0.f;
	eg_real          m_TimeToNextSpawn = 0.f;
	eg_real          m_DifficultyInterval = 10.f;
	eg_real          m_TimeToNextDifficulty = 0.f;
	eg_int           m_Difficulty = 0;
	eg_int           m_PauseCount = 0;

public:

	DeGame();

protected:

	virtual void OnConstruct() override;
	virtual void OnDestruct() override;
	virtual void Init( EGEntWorld* WorldOwner ) override;
	virtual void Update( eg_real DeltaTime ) override;
	virtual void OnClientEnterWorld( const eg_lockstep_id& LockstepId ) override;
	virtual void OnClientLeaveWorld( const eg_lockstep_id& LockstepId ) override;

public:

	de_game_state GetGameState() const { return m_GameState; }
	void CreateNewGameFromClient();
	void QuitGameFromClient();
	void OpenPauseMenuFromServer( eg_lockstep_id ClientId );
	eg_transform GetCameraPose() const { return m_CameraPose; }

	void AddPlayerEnt( DePlayerEnt* Ent );
	void RemovePlayerEnt( DePlayerEnt* Ent );

	void AddSpawnerEnt( DeSpawnerEnt* Ent );
	void RemoveSpawnerEnt( DeSpawnerEnt* Ent );
	void AddImpEnt( DeImpEnt* Ent );
	void RemoveImpEnt( DeImpEnt* Ent );
	DePlayerEnt* GetClientEnt( const eg_lockstep_id& ClientLockstepId );
	eg_transform GetMainMenuCameraTransform() const;
	eg_transform GetImpTarget();
	void HurtPlayers( eg_int Amount );
	eg_bool ArePlayersAlive() const;
	void RestartGame();
	void SetGamePaused( eg_bool bValue );
	eg_bool IsGamePaused() const { return m_PauseCount > 0; }
	eg_color GetRandomImpColor();

private:

	void SetGameState( de_game_state NewGameState );

	void CreateEntForClient( deGameClient& Client );

	const DeSpawnerEnt* GetPlayerStartSpawner() const;
	const DeSpawnerEnt* GetMainMenuCameraPoseSpawner() const;

	void RE_CreateNewGame( const eg_event_parms& Parms );
	void RE_QuitGame( const eg_event_parms& Parms );
	void RE_OpenPauseMenu( const eg_event_parms& Parms );
	void RE_RestartGame( const eg_event_parms& Parms );
	void RE_SetGamePaused( const eg_event_parms& Parms );

	const DeSpawnerEnt* GetRandomImpSpawner();
};
