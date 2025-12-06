// (c) 2018 Beem Media

#pragma once

#include "DeMenu.h"

enum class de_main_menu_option_t
{
	NewGame,
	Settings,
	Credits,
	YouTubeVideo,
	Quit,
};

class DeMainMenu : public DeMenu
{
	EG_CLASS_BODY( DeMainMenu , DeMenu )

private:

	EGUiGridWidget*                m_ChoiceListWidget;
	EGArray<de_main_menu_option_t> m_Options;

public:

	virtual void OnInit() override;
	virtual eg_bool OnInput( eg_menuinput_t InputType ) override;

protected:

	void OnChoiceListItemChanged( const egUIWidgetEventInfo& EventInfo );
	void OnChoiceListItemPressed( const egUIWidgetEventInfo& EventInfo );
};
