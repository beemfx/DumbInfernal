// (c) 2018 Beem Media

#pragma once

#include "DeMenu.h"

class EGUiTextWidget;
class EGUiGridWidget;
class EGSettingsVar;

struct deSettingsMenuItem
{
	EGSettingsVar* Var;
	eg_string_crc  NameStr;
	eg_cpstr       ValueStr;
	eg_bool        bNeedsVidRestart:1;
	eg_bool        bApplyImmediately:1;
};


class DeSettingsMenu : public DeMenu
{
	EG_CLASS_BODY( DeSettingsMenu , DeMenu )

private:

	EGUiTextWidget*             m_HintTextWidget;
	EGUiGridWidget*             m_SettingsItemsWidget;
	EGArray<deSettingsMenuItem> m_SettingsItems;
	eg_bool                     m_bNeedsVidRestart = false;

private:

	virtual void OnInit() override;
	virtual eg_bool OnInput( eg_menuinput_t InputType ) override;
	virtual void OnInputCmds( const struct egLockstepCmds& Cmds ) override;

	void PopulateHintBar();

	void OnSettingsItemChanged( const egUIWidgetEventInfo& ItemInfo );
	void OnSettingsItemPressed( const egUIWidgetEventInfo& ItemInfo );

	void ChangeAtIndex( eg_uint Index , eg_bool bInc );
	void SaveAndExit();
};
