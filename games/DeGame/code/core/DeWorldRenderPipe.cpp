// (c) 2018 Beem Media

#include "DeWorldRenderPipe.h"
#include "EGEntDef.h"
#include "EGDisplayList.h"
#include "EGRenderer.h"
#include "EGGlobalConfig.h"

EG_CLASS_DECL( DeWorldRenderPipe )

void DeWorldRenderPipe::OnConstruct()
{
	Super::OnConstruct();

	EGMaterialDef TOMtrDef;
	TOMtrDef.SetShader( "/game/shaders/de_terrain_outline" );
	m_TerrainOutlineMtr = EGRenderer::Get().CreateMaterial( &TOMtrDef , "TerrainOutlineMtr" );
}

void DeWorldRenderPipe::OnDestruct()
{
	EGRenderer::Get().DestroyMaterial( m_TerrainOutlineMtr );
	m_TerrainOutlineMtr = EGV_MATERIAL_NULL;

	Super::OnDestruct();
}

void DeWorldRenderPipe::DrawWorld( EGDisplayList* DisplayList , const EGCamera2& Camera )
{
	unused( Camera );

	// Sky box
	DisplayList->PushDepthStencilState( eg_depthstencil_s::ZTEST_DEFAULT_ZWRITE_OFF );
	DrawEnts( eg_crc("Background") );
	DisplayList->PopDepthStencilState();

	// Terrain Outline
	DisplayList->SetMaterialOverride( m_TerrainOutlineMtr );
	DisplayList->SetVec4( eg_rv4_t::GAME_PALETTE_0 , eg_vec4(.5f,0.f,0.f,0.f) );
	DisplayList->SetVec4( eg_rv4_t::GAME_PALETTE_1 , eg_color(0.f,0.f,0.f,1.f).ToVec4() );
	DrawTerrain();
	DisplayList->SetMaterialOverride( EGV_MATERIAL_NULL );
	DisplayList->ClearDS( 1.f , 0 );

	// Regular world draw
	DrawTerrain();
	DrawEnts( eg_string_crc( ENT_RENDER_FILTER_WORLD ) );
	DrawMap();
	DrawEnts( eg_string_crc( ENT_RENDER_FILTER_PRE_FX ) );
	if( VideoConfig_PostMotionBlur.GetValue() )
	{
		DisplayList->DoSceneProc( DL_F_PROC_STOREDEPTH | DL_F_PROC_MOTIONBLUR );
	}
	else
	{
		DisplayList->DoSceneProc( DL_F_PROC_STOREDEPTH );
	}
	DrawEnts( eg_string_crc( ENT_RENDER_FILTER_POST_FX ) );
}
