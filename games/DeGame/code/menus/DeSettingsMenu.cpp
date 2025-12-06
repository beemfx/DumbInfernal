// (c) 2018 Beem Media

#include "DeSettingsMenu.h"
#include "EGInputTypes.h"
#include "EGUiTextWidget.h"
#include "EGInput.h"
#include "EGUiGridWidget.h"
#include "EGEngine.h"
#include "EGSettings2.h"

EG_CLASS_DECL( DeSettingsMenu )

static const eg_cpstr DeSettingsMenu_Vars[] =
{
	"DePlayerEnt_InvertYAxis",
	"InputConfig.MouseAxisSensitivity",
	"InputConfig.MouseSmoothSamples",
	"InputConfig.GamepadAxisSensitivity",
	"Volume_Effect",
	"Volume_Music",
	"DeGame.UseOriginalYouTubeDifficulty",
	"EGRenderer_ScreenRes",
	"IsWindowed",
	"VSyncEnabled",
	// "PostFXAA",
	"PostMotionBlur",
	// "EGRenderer_ShaderQuality",
	"DebugDrawFPS",

};

void DeSettingsMenu::OnInit()
{
	Super::OnInit();

	m_HintTextWidget = GetWidget<EGUiTextWidget>( eg_crc("HintText") );
	m_SettingsItemsWidget = GetWidget<EGUiGridWidget>( eg_crc("SettingsItems") );

	m_SettingsItems.Clear();

	EGArray<EGSettingsVar*> GlobalSettings;
	const eg_bool bShowAll = IS_DEBUG;
	if( bShowAll )
	{
		EGSettingsVar::GetAllSettingsMatchingFlags( GlobalSettings , EGS_F_EDITABLE|EGS_F_DEBUG_EDITABLE);
	}
	else
	{
		for( eg_cpstr Str : DeSettingsMenu_Vars )
		{
			EGSettingsVar* Setting = EGSettingsVar::GetSettingByName( Str );
			assert( Setting ); // Bad var name?
			if( Setting && (Setting->IsEditable() || Setting->IsDebugEditable()) )
			{
				GlobalSettings.Append( Setting );
			}
		}
	}

	for( EGSettingsVar* Var : GlobalSettings )
	{
		if( Var )
		{
			deSettingsMenuItem NewItem;
			NewItem.Var = Var;
			NewItem.NameStr  = Var->GetDisplayName();
			NewItem.ValueStr = nullptr;
			NewItem.bApplyImmediately = true;
			NewItem.bNeedsVidRestart = Var->GetFlags().IsSet( EGS_F_NEEDSVRESTART );
			m_SettingsItems.Append( NewItem );
		}
	}

	if( m_SettingsItemsWidget )
	{
		m_SettingsItemsWidget->OnItemChangedDelegate.Bind( this , &ThisClass::OnSettingsItemChanged );
		m_SettingsItemsWidget->OnItemPressedDelegate.Bind( this , &ThisClass::OnSettingsItemPressed );
		m_SettingsItemsWidget->RefreshGridWidget( m_SettingsItems.LenAs<eg_uint>() );
		if( m_SettingsItems.HasItems() )
		{
			SetFocusedWidget( m_SettingsItemsWidget , 0 , false );
		}
	}
	
	PopulateHintBar();
}

eg_bool DeSettingsMenu::OnInput( eg_menuinput_t InputType )
{
	switch( InputType )
	{
		case eg_menuinput_t::BUTTON_BACK:
		{
			SaveAndExit();
		} return true;

		case eg_menuinput_t::BUTTON_RIGHT:
		{
			EGUiGridWidget* SettingsList = GetWidget<EGUiGridWidget>( eg_crc( "SettingsItems" ) );
			if( SettingsList && SettingsList == GetFocusedWidget() )
			{
				eg_uint Index = SettingsList->GetSelectedIndex();
				ChangeAtIndex( Index, true );
				SettingsList->RunEvent( eg_crc( "ClickSound" ) );
				SettingsList->RefreshGridWidget( m_SettingsItems.LenAs<eg_int>() );
			}
		} return true;

		case eg_menuinput_t::BUTTON_LEFT:
		{
			EGUiGridWidget* SettingsList = GetWidget<EGUiGridWidget>( eg_crc( "SettingsItems" ) );
			if( SettingsList && SettingsList == GetFocusedWidget() )
			{
				eg_uint Index = SettingsList->GetSelectedIndex();
				ChangeAtIndex( Index, false );
				SettingsList->RunEvent( eg_crc( "ClickSound" ) );
				SettingsList->RefreshGridWidget( m_SettingsItems.LenAs<eg_int>() );
			}
		} return true;

		default: break;
	}

	return Super::OnInput( InputType );
}

void DeSettingsMenu::OnInputCmds( const struct egLockstepCmds& Cmds )
{
	unused( Cmds );
	
	// if( Cmds.WasMenuPressed( CMDA_MENU_BTN2 ) )
	// {
	// 	MenuStack_Pop();
	// }
}

void DeSettingsMenu::PopulateHintBar()
{
	eg_loc_text HintText;
	auto AddHint = [&HintText]( eg_cmd_t Cmd , eg_string_crc CmdHintCrc ) -> void
	{
		eg_cpstr ButtonGlyph = EGInput::Get().InputCmdToName( Cmd );
		eg_loc_text CmdHintText;
		if( ButtonGlyph && ButtonGlyph[0] != '\0' )
		{
			CmdHintText = EGFormat( L"|Glyph({0})| {1}", eg_loc_text(EGString_ToWide(ButtonGlyph)).GetString() , CmdHintCrc );
		}
		else
		{
			CmdHintText = eg_loc_text(CmdHintCrc);
		}

		HintText = EGFormat( L"{0}{1} " , HintText.GetString() , CmdHintText.GetString() );
	};

	AddHint( CMDA_MENU_BACK , eg_loc("ApplyAndBackOut","Apply and Back Out") );
	// AddHint( CMDA_MENU_BTN2 , eg_loc("RevertAndBackOut","Revert and Back Out") );
	if( m_HintTextWidget )
	{
		m_HintTextWidget->SetText( CT_Clear , HintText );
	}
}

void DeSettingsMenu::OnSettingsItemChanged( const egUIWidgetEventInfo& ItemInfo )
{
	ItemInfo.GridWidgetOwner->DefaultOnGridWidgetItemChanged( ItemInfo );

	if( m_SettingsItems.IsValidIndex( ItemInfo.GridIndex ) )
	{
		const deSettingsMenuItem& SettingItem = m_SettingsItems[ItemInfo.GridIndex];
		ItemInfo.Widget->SetText( eg_crc("NameText") , eg_loc_text( SettingItem.NameStr ) );

		if( SettingItem.Var )
		{
			eg_loc_text ValueText = SettingItem.Var->ToLocText();
			ItemInfo.Widget->SetText( eg_crc( "ValueText" ), ValueText );
		}
	}
}

void DeSettingsMenu::OnSettingsItemPressed( const egUIWidgetEventInfo& ItemInfo )
{
	//EGLogf( eg_log_t::General , "Clicked setting %u at <%f,%f>" , Info.Index , Info.HitPoint.x , Info.HitPoint.y );

	// We only care about clicking on the right half:
	if( m_SettingsItems.IsValidIndex( ItemInfo.GridIndex ) )
	{
		const deSettingsMenuItem& SettingItem = m_SettingsItems[ItemInfo.GridIndex];

		const eg_real MinClick = .5f;
		if( ItemInfo.HitPoint.x >= MinClick )
		{
			eg_real AdjClickPos = EGMath_GetMappedRangeValue( ItemInfo.HitPoint.x , eg_vec2(MinClick,1.f) , eg_vec2(-1.f,1.f) );
			ChangeAtIndex( ItemInfo.GridIndex , AdjClickPos >= 0.f );
			if( ItemInfo.GridWidgetOwner )
			{
				ItemInfo.GridWidgetOwner->RunEvent( eg_crc("ClickSound") );
				ItemInfo.GridWidgetOwner->RefreshGridWidget( m_SettingsItems.LenAs<eg_uint>() );
			}
		}
	}
}

void DeSettingsMenu::ChangeAtIndex( eg_uint Index, eg_bool bInc )
{
	if( m_SettingsItems.IsValidIndex( Index ) )
	{
		const deSettingsMenuItem& SettingItem = m_SettingsItems[Index];

		if( SettingItem.bNeedsVidRestart )
		{
			m_bNeedsVidRestart = true;
		}

		if( SettingItem.Var )
		{
			if( bInc )
			{
				SettingItem.Var->Inc();
			}
			else
			{
				SettingItem.Var->Dec();
			}
		}
	}
	else
	{
		assert( false ); // Index out of range!
	}
}

void DeSettingsMenu::SaveAndExit()
{
	Engine_QueMsg( "engine.SaveConfig()" );
	if( m_bNeedsVidRestart )
	{
		Engine_QueMsg( "engine.VidRestart()" );
	}
	MenuStack_Pop();
}
