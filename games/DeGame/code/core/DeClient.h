// (c) 2018 Beem Media

#pragma once

#include "EGClient.h"

class DeProfileData;

class DeClient : public EGClient
{
	EG_CLASS_BODY( DeClient , EGClient )

private:

	eg_s_string_sml8 m_ProfileDataPath;
	DeProfileData*   m_ProfileData;

public:

	virtual void Init( const egClientId& Id , EGClass* GameClass ) override;
	virtual void Deinit() override;
	virtual void BeginGame() override;
	virtual void SetupCamera( class EGClientView& View ) const override;
	virtual void Draw( eg_bool bForceSimple ) override;
	DeProfileData* GetProfileData() { return m_ProfileData; }
	void SaveProfile();

private:

	void OnPlatformOverlayActivated( eg_bool bActive );
};
