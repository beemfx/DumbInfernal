// (c) 2019 Beem Media

#include "DeHudBar.h"
#include "EGMeshComponent.h"
#include "DePlayerEnt.h"
#include "DeGame.h"
#include "EGTextNodeComponent.h"
#include "EGDebugText.h"

EG_CLASS_DECL( DeHudBar )

void DeHudBar::Init( EGMenu* InOwner, const egUiWidgetInfo* InInfo )
{
	Super::Init( InOwner , InInfo );

	m_PlayerHealthText = m_EntObj.GetComponentTree().GetComponentById<EGTextNodeComponent>( eg_crc("PlayerHealthText") );
	m_PlayerScoreText = m_EntObj.GetComponentTree().GetComponentById<EGTextNodeComponent>( eg_crc("PlayerScoreText") );
}

void DeHudBar::Update( eg_real DeltaTime, eg_real AspectRatio )
{
	Super::Update( DeltaTime , AspectRatio );

	DePlayerEnt* OwningPlayer = GetOwningPlayer();
	DeGame* Game = GetGame();
	if( Game && OwningPlayer )
	{
		if( m_PlayerHealthText )
		{
			const eg_int Health = OwningPlayer->GetHealth();
			m_PlayerHealthText->SetText( EGFormat( eg_crc("HealthText") , Health ) );
			if( Health > 30 )
			{
				m_PlayerHealthText->SetPalette( eg_vec4(1.f,1.f,1.f,1.f) );
			}
			else
			{
				m_PlayerHealthText->SetPalette( eg_vec4(1.f,0.f,0.f,1.f) );
			}
		}

		if( m_PlayerScoreText )
		{
			m_PlayerScoreText->SetText( EGFormat( eg_crc("ScoreText") , OwningPlayer->GetScore() ) );
		}
	}
}

void DeHudBar::Draw( eg_real AspectRatio )
{
	MainDisplayList->SetVec4( eg_rv4_t::ENT_PALETTE_1 , m_ExtraPalette );
	Super::Draw( AspectRatio );
}
