// (c) 2019 Beem Media

#include "DeHud.h"
#include "DeHudWidget.h"
#include "DeClient.h"
#include "DeGame.h"
#include "EGMenuStack.h"
#include "EGUiTextWidget.h"

EG_CLASS_DECL( DeHud )

DePlayerEnt* DeHud::s_HudCreationEnt = nullptr;

DeHud* DeHud::CreateHud( DeClient* InClient , DePlayerEnt* InPlayerOwner )
{
	assert( nullptr == s_HudCreationEnt );
	s_HudCreationEnt = InPlayerOwner;
	DeHud* NewHud = EGCast<DeHud>(InClient->SDK_AddOverlay( eg_crc("DeHud") , 0.f ));
	s_HudCreationEnt = nullptr;
	return NewHud;
}

void DeHud::Init( eg_string_crc MenuCrc, EGMenuStack* MenuStackOwner, EGOverlayMgr* OverlayOwner, EGClient* OwnerClient )
{
	m_OwningPlayer = s_HudCreationEnt;
	Super::Init( MenuCrc , MenuStackOwner , OverlayOwner , OwnerClient );
}

void DeHud::OnActivate()
{
	Super::OnActivate();

	bool bMenuIsUp = false;
	EGMenuStack* MenuStack = GetClientOwner() ? GetClientOwner()->SDK_GetMenuStack() : nullptr;
	if( MenuStack )
	{
		bMenuIsUp = MenuStack->Len() != 0;
		MenuStack->MenuStackChangedDelegate.AddUnique( this , &ThisClass::OnMenuStackChanged );
	}

	Hide( true );
	if( !bMenuIsUp )
	{
		Show( false );
	}
}

void DeHud::OnDeactivate()
{
	EGMenuStack* MenuStack = GetClientOwner() ? GetClientOwner()->SDK_GetMenuStack() : nullptr;
	if( MenuStack )
	{
		MenuStack->MenuStackChangedDelegate.RemoveAll( this );
	}

	Hide( false );
	Super::OnDeactivate();
}

void DeHud::Show( eg_bool bImmediate )
{
	for( EGUiWidget* Obj : GetWidgets() )
	{
		DeHudWidget* HudWidget = EGCast<DeHudWidget>( Obj );
		if( HudWidget )
		{
			HudWidget->Show( bImmediate );
		}
	}
}

void DeHud::Hide( eg_bool bImmediate )
{
	for( EGUiWidget* Obj : GetWidgets() )
	{
		DeHudWidget* HudWidget = EGCast<DeHudWidget>( Obj );
		if( HudWidget )
		{
			HudWidget->Hide( bImmediate );
		}
	}
}

void DeHud::OnUpdate( eg_real DeltaTime, eg_real AspectRatio )
{
	Super::OnUpdate( DeltaTime , AspectRatio );

	const DePlayerEnt* Player = GetOwningPlayer();
	const eg_bool bPlayerIsDead = Player && Player->GetPlayerState() == de_player_ent_s::Dead;
	if( m_bIsPlayerDead != bPlayerIsDead )
	{
		m_bIsPlayerDead = bPlayerIsDead;
		EGUiWidget* FadeWidget = GetWidget<EGUiWidget>( eg_crc("Fade") );
		if( FadeWidget )
		{
			FadeWidget->RunEvent( m_bIsPlayerDead ? eg_crc("FadeToBlood") : eg_crc("SetToClear") );
		}
		EGUiTextWidget* FinalScoreText = GetWidget<EGUiTextWidget>( eg_crc("FinalScoreText") );
		if( FinalScoreText )
		{
			FinalScoreText->SetText( CT_Clear , m_bIsPlayerDead ? EGFormat( eg_loc("FinaScoreFormat","Final Score: {0}") , Player->GetScore() ) : CT_Clear );
		}
		EGUiTextWidget* HiScoreText = GetWidget<EGUiTextWidget>( eg_crc("HiScoreText") );
		if( HiScoreText )
		{
			HiScoreText->SetText( CT_Clear , m_bIsPlayerDead ? EGFormat( eg_loc("HiScoreFormat","High Score: {0}") , Player->GetHighScore() ) : CT_Clear );
		}
	}
}

DePlayerEnt* DeHud::GetOwningPlayer()
{
	return m_OwningPlayer.GetObject();
}

const DePlayerEnt* DeHud::GetOwningPlayer() const
{
	return m_OwningPlayer.GetObject();
}

void DeHud::OnMenuStackChanged( EGMenuStack* MenuStack )
{
	if( MenuStack )
	{
		if( MenuStack->Len() > 0 )
		{
			Hide( false );
		}
		else
		{
			Show( false );
		}
	}
	else
	{
		Show( false );
	}
}
