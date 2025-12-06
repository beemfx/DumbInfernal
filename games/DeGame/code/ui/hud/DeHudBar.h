// (c) 2019 Beem Media

#pragma once

#include "DeHudWidget.h"

class EGMeshComponent;
class EGTextNodeComponent;

class DeHudBar : public DeHudWidget
{
	EG_CLASS_BODY( DeHudBar , DeHudWidget )

private:

	EGWeakPtr<EGTextNodeComponent> m_PlayerHealthText;
	EGWeakPtr<EGTextNodeComponent> m_PlayerScoreText;
	eg_vec4                        m_ExtraPalette;

private:

	virtual void Init( EGMenu* InOwner , const egUiWidgetInfo* InInfo ) override;
	virtual void Update( eg_real DeltaTime, eg_real AspectRatio ) override;
	virtual void Draw( eg_real AspectRatio ) override;
};
