// (c) 2018 Beem Media

#pragma once

#include "EGWorldRenderPipe.h"
#include "EGRendererTypes.h"

class DeWorldRenderPipe : public EGWorldRenderPipe
{
	EG_CLASS_BODY( DeWorldRenderPipe , EGWorldRenderPipe )

private:

	egv_material m_TerrainOutlineMtr = EGV_MATERIAL_NULL;

private:

	virtual void OnConstruct() override;
	virtual void OnDestruct() override;
	virtual void DrawWorld( EGDisplayList* DisplayList , const EGCamera2& Camera ) override;
};
