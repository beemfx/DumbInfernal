// (c) 2018 Beem Media

#pragma once

#include "DeMenu.h"

enum class de_pause_menu_option_t
{
	ResumeGame,
	RestartGame,
	Settings,
	QuitToMainMenu,
	QuitToOS,
};

class DePauseMenu : public DeMenu
{
	EG_CLASS_BODY( DePauseMenu , DeMenu )

private:

	EGUiGridWidget*                 m_ChoiceListWidget;
	EGArray<de_pause_menu_option_t> m_Options;

public:

	virtual void OnInit() override;
	virtual void OnDeinit() override;
	virtual eg_bool OnInput( eg_menuinput_t InputType ) override;

protected:

	void OnChoiceListItemChanged( const egUIWidgetEventInfo& EventInfo );
	void OnChoiceListItemPressed( const egUIWidgetEventInfo& EventInfo );
};
