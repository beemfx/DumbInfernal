// (c) 2018 Beem Media

#pragma once

#include "EGDataAsset.h"
#include "DeCreditsData.reflection.h"

egreflect enum class de_credits_line_t
{
	Normal,
	FuturePop,
	HeaderPop,
	FutureHeaderPop,
	Header,
	Center,
	Blob,
	Delay,
};

egreflect struct deCreditsName
{
	egprop eg_string_crc Name;
	egprop eg_d_string   Name_enus;
};

egreflect struct deCreditsLine
{
	egprop eg_string_crc     Comment; // This is just so when collapsed you can see what it is in the editor.
	egprop de_credits_line_t Type;
	egprop deCreditsName     Title;
	egprop deCreditsName     Name;
	egprop eg_real           DelayTime;
};

egreflect class DeCreditsData : public egprop EGDataAsset
{
	EG_CLASS_BODY( DeCreditsData , EGDataAsset )

public:

	egprop EGArray<deCreditsLine> m_Lines = eg_mem_pool::DefaultHi;

public:

	virtual void OnPropChanged( const egRflEditor& ChangedProperty , egRflEditor& RootEditor , eg_bool& bNeedsRebuildOut ) override;
	virtual void PostLoad( eg_cpstr16 Filename , eg_bool bForEditor , egRflEditor& RflEditor ) override;

private:

	void RefreshVisibleProperties( egRflEditor& RootEditor );
};
