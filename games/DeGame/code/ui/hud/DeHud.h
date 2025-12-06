// (c) 2019 Beem Media

#pragma once

#include "DeMenu.h"

class EGMenuStack;
class DePlayerEnt;
class DeClient;

class DeHud : public DeMenu
{
	EG_CLASS_BODY( DeHud , DeMenu )

public:

	static DePlayerEnt* s_HudCreationEnt;

protected:

	EGWeakPtr<DePlayerEnt> m_OwningPlayer;
	eg_bool                m_bIsPlayerDead;

public:

	static DeHud* CreateHud( DeClient* InClient , DePlayerEnt* InPlayerOwner );

protected:

	virtual void Init( eg_string_crc MenuCrc, EGMenuStack* MenuStackOwner, EGOverlayMgr* OverlayOwner, EGClient* OwnerClient );
	virtual void OnActivate() override;
	virtual void OnDeactivate() override;

public:

	void Show( eg_bool bImmediate );
	void Hide( eg_bool bImmediate );

	virtual void OnUpdate( eg_real DeltaTime , eg_real AspectRatio ) override;

	DePlayerEnt* GetOwningPlayer();
	const DePlayerEnt* GetOwningPlayer() const;


private:

	void OnMenuStackChanged( EGMenuStack* MenuStack );
};
