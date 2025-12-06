// (c) 2018 Beem Media

#include "DeClient.h"
#include "EGMenuStack.h"
#include "DeGame.h"
#include "DeProfileData.h"
#include "EGLoader.h"
#include "EGFileData.h"
#include "EGPlatform.h"

EG_CLASS_DECL( DeClient )

void DeClient::Init( const egClientId& Id , EGClass* GameClass )
{
	Super::Init( Id , GameClass );

	m_ProfileDataPath = GAME_USER_PATH "/UserProfile.egasset";
	m_ProfileData = EGDataAsset::LoadDataAsset<DeProfileData>( *eg_s_string_sml16(*m_ProfileDataPath) , true );
	if( m_ProfileData )
	{
		m_ProfileData->Init( this );
	}

	EGPlatform_GetPlatform().PlatformOverlayActivatedDelegate.AddUnique( this , &ThisClass::OnPlatformOverlayActivated );
}

void DeClient::Deinit()
{
	EGPlatform_GetPlatform().PlatformOverlayActivatedDelegate.RemoveAll( this );
	
	if( m_ProfileData )
	{
		EGDeleteObject( m_ProfileData );
		m_ProfileData = nullptr;
	}

	Super::Deinit();
}

void DeClient::BeginGame()
{
	Super::BeginGame();

	m_MenuStack->SetMouseCursor( eg_crc("DeMouseCursor") );
	m_MenuStack->Push( eg_crc("MainMenu") );
}

void DeClient::SetupCamera( class EGClientView& View ) const
{
	const DeGame* Game = EGCast<DeGame>(SDK_GetGame());
	if( Game )
	{
		View.SetPose( Game->GetCameraPose() , 90.f , .1f ,  1000.f );
	}
	else
	{
		Super::SetupCamera( View );
	}
}

void DeClient::Draw( eg_bool bForceSimple )
{
	MainDisplayList->PushSamplerState( eg_sampler_s::TEXTURE_WRAP_FILTER_POINT );
	Super::Draw( bForceSimple );
	MainDisplayList->PopSamplerState();
}

void DeClient::SaveProfile()
{
	if( m_ProfileData && MainLoader )
	{
		EGFileData ProfileFileData( eg_file_data_init_t::HasOwnMemory );
		m_ProfileData->SaveGameDataTo( ProfileFileData );
		MainLoader->SaveData( *m_ProfileDataPath , ProfileFileData.GetDataAs<eg_byte>() , ProfileFileData.GetSize() );
	}
}

void DeClient::OnPlatformOverlayActivated( eg_bool bActive )
{
	if( bActive )
	{
		DeGame* Game = EGCast<DeGame>(SDK_GetGame());
		if( Game && Game->GetGameState() == de_game_state::Playing )
		{
			SDK_RunClientEvent( egRemoteEvent( eg_crc("OpenPauseMenu") ,  eg_lockstep_id::IdToRaw( GetLockstepId() ) ) );
		}
	}
}
