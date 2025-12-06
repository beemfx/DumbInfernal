// (c) 2018 Beem Media

#include "DeMainMenu.h"
#include "EGEngine.h"
#include "EGClient.h"
#include "EGUiGridWidget.h"
#include "DeGame.h"
#include "EGWindowsAPI.h"

EG_CLASS_DECL( DeMainMenu )

void DeMainMenu::OnInit()
{
	Super::OnInit();

	m_ChoiceListWidget = GetWidget<EGUiGridWidget>( eg_crc("ChoiceList") );

	EGClient* OwnerClient = GetOwnerClient();
	if( OwnerClient )
	{
		OwnerClient->SDK_PlayBgMusic( "Music:MainMenu" );
	}

	m_Options.Append( de_main_menu_option_t::NewGame );
	m_Options.Append( de_main_menu_option_t::Settings );
	m_Options.Append( de_main_menu_option_t::Credits );
	m_Options.Append( de_main_menu_option_t::YouTubeVideo );
	m_Options.Append( de_main_menu_option_t::Quit );

	if( m_ChoiceListWidget )
	{
		m_ChoiceListWidget->OnItemChangedDelegate.Bind( this , &ThisClass::OnChoiceListItemChanged );
		m_ChoiceListWidget->OnItemPressedDelegate.Bind( this , &ThisClass::OnChoiceListItemPressed );

		m_ChoiceListWidget->RefreshGridWidget( m_Options.LenAs<eg_uint>() );
		SetFocusedWidget( m_ChoiceListWidget , 0 , false );
	}
}

eg_bool DeMainMenu::OnInput( eg_menuinput_t InputType )
{
	return Super::OnInput( InputType );
}

void DeMainMenu::OnChoiceListItemChanged( const egUIWidgetEventInfo& EventInfo )
{
	if( EventInfo.GridWidgetOwner && EventInfo.Widget && m_Options.IsValidIndex(EventInfo.GridIndex) )
	{
		EventInfo.GridWidgetOwner->DefaultOnGridWidgetItemChanged( EventInfo );
		eg_loc_text ItemText;

		switch( m_Options[EventInfo.GridIndex] )
		{
			case de_main_menu_option_t::NewGame  : ItemText = eg_loc_text(eg_loc("New Game","New Game"));   break;
			case de_main_menu_option_t::Settings : ItemText = eg_loc_text(eg_loc("Settings","Settings"));   break;
			case de_main_menu_option_t::Credits  : ItemText = eg_loc_text(eg_loc("CredtisText","Credits")); break;
			case de_main_menu_option_t::YouTubeVideo: ItemText = eg_loc_text(eg_loc("YouTubeVideoText","Original YouTube Video")); break;
			case de_main_menu_option_t::Quit     : ItemText = eg_loc_text(eg_loc("Quit","Quit"));           break;
		}

		EventInfo.Widget->SetText( eg_crc("Text") , ItemText );
	}
}

void DeMainMenu::OnChoiceListItemPressed( const egUIWidgetEventInfo& EventInfo )
{
	if( m_Options.IsValidIndex(EventInfo.GridIndex) )
	{
		switch( m_Options[EventInfo.GridIndex] )
		{
			case de_main_menu_option_t::NewGame:
			{
				DeGame* Game = GetGame();
				if( Game )
				{
					MenuStack_Clear();
					Game->CreateNewGameFromClient();
				}
			} break;
			case de_main_menu_option_t::Settings:
			{
				MenuStack_PushTo( eg_crc("SettingsMenu") );
			} break;
			case de_main_menu_option_t::Quit:
			{
				Engine_QueExit();
			} break;
			case de_main_menu_option_t::YouTubeVideo:
			{
				ShellExecuteW( NULL , L"open" , L"https://youtu.be/roJbmPTyrHo?t=196" , NULL , NULL , SW_SHOWNORMAL );
				Engine_QueExit();
			} break;
			case de_main_menu_option_t::Credits:
			{
				MenuStack_PushTo( eg_crc("CreditsMenu") );
			} break;
		}
	}
}
