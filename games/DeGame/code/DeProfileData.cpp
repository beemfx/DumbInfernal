// (c) 2020 Beem Media. All Rights Reserved.

#include "DeProfileData.h"
#include "DeClient.h"

EG_CLASS_DECL( DeProfileData )

void DeProfileData::Init( DeClient* InOwningClient )
{
	m_OwningClient = InOwningClient;
}

void DeProfileData::SaveProfile()
{
	if( m_OwningClient )
	{
		m_OwningClient->SaveProfile();
	}
}

void DeProfileData::AddNewScore( eg_int NewScoreIn )
{
	if( NewScoreIn > m_HighScore )
	{
		m_HighScore = NewScoreIn;
		SaveProfile();
	}
}
