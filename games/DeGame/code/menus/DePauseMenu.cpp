// (c) 2018 Beem Media

#include "DePauseMenu.h"
#include "EGClient.h"
#include "EGUiGridWidget.h"
#include "DeGame.h"
#include "EGEngine.h"
#include "DePlayerEnt.h"

EG_CLASS_DECL( DePauseMenu )

void DePauseMenu::OnInit()
{
	Super::OnInit();

	m_ChoiceListWidget = GetWidget<EGUiGridWidget>( eg_crc("ChoiceList") );

	DeGame* Game = GetGame();
	const eg_bool bHasAlivePlayers = Game && Game->ArePlayersAlive();
	if( bHasAlivePlayers )
	{
		m_Options.Append( de_pause_menu_option_t::ResumeGame );
	}
	m_Options.Append( de_pause_menu_option_t::RestartGame );
	m_Options.Append( de_pause_menu_option_t::Settings );
	m_Options.Append( de_pause_menu_option_t::QuitToMainMenu );
	m_Options.Append( de_pause_menu_option_t::QuitToOS );

	if( m_ChoiceListWidget )
	{
		m_ChoiceListWidget->OnItemChangedDelegate.Bind( this , &ThisClass::OnChoiceListItemChanged );
		m_ChoiceListWidget->OnItemPressedDelegate.Bind( this , &ThisClass::OnChoiceListItemPressed );

		m_ChoiceListWidget->RefreshGridWidget( m_Options.LenAs<eg_uint>() );
		SetFocusedWidget( m_ChoiceListWidget , 0 , false );
	}

	if( Game )
	{
		Game->SetGamePaused( true );
	}
}

void DePauseMenu::OnDeinit()
{
	DeGame* Game = GetGame();
	if( Game )
	{
		Game->SetGamePaused( false );
	}

	Super::OnDeinit();
}

eg_bool DePauseMenu::OnInput( eg_menuinput_t InputType )
{
	if( InputType == eg_menuinput_t::BUTTON_BACK )
	{
		MenuStack_Pop();
		return true;
	}

	return Super::OnInput( InputType );
}

void DePauseMenu::OnChoiceListItemChanged( const egUIWidgetEventInfo& EventInfo )
{
	if( EventInfo.GridWidgetOwner && EventInfo.Widget && m_Options.IsValidIndex(EventInfo.GridIndex) )
	{
		EventInfo.GridWidgetOwner->DefaultOnGridWidgetItemChanged( EventInfo );
		eg_loc_text ItemText;

		switch( m_Options[EventInfo.GridIndex] )
		{
		case de_pause_menu_option_t::RestartGame    : ItemText = eg_loc_text(eg_loc("RestartGame","Restart Game")); break;
		case de_pause_menu_option_t::ResumeGame     : ItemText = eg_loc_text(eg_loc("ResumeGame","Resume")); break;
		case de_pause_menu_option_t::Settings       : ItemText = eg_loc_text(eg_loc("Settings","Settings")); break;
		case de_pause_menu_option_t::QuitToMainMenu : ItemText = eg_loc_text(eg_loc("QuitToMainMenu","Quit Game")); break;
		case de_pause_menu_option_t::QuitToOS       : ItemText = eg_loc_text(eg_loc("QuitToOS","Exit to Windows")); break;
		}

		EventInfo.Widget->SetText( eg_crc("Text") , ItemText );
	}
}

void DePauseMenu::OnChoiceListItemPressed( const egUIWidgetEventInfo& EventInfo )
{
	if( m_Options.IsValidIndex(EventInfo.GridIndex) )
	{
		switch( m_Options[EventInfo.GridIndex] )
		{
		case de_pause_menu_option_t::RestartGame:
		{
			DeGame* Game = GetGame();
			if( Game )
			{
				MenuStack_Clear();
				Game->RestartGame();
			}
		} break;
		case de_pause_menu_option_t::ResumeGame:
		{
			MenuStack_Pop();
		} break;
		case de_pause_menu_option_t::Settings:
		{
			MenuStack_PushTo( eg_crc("SettingsMenu") );
		} break;
		case de_pause_menu_option_t::QuitToMainMenu:
		{
			DeGame* Game = GetGame();
			if( Game )
			{
				MenuStack_Clear();
				Game->QuitGameFromClient();
			}
		} break;
		case de_pause_menu_option_t::QuitToOS:
		{
			Engine_QueExit();
		} break;
		}
	}
}
