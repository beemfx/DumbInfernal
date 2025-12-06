// (c) 2020 Beem Media. All Rights Reserved.

#pragma once

#include "EGDataAsset.h"
#include "DeProfileData.reflection.h"

class DeClient;

egreflect class DeProfileData : public egprop EGDataAsset
{
	EG_CLASS_BODY( DeProfileData , EGDataAsset )
	EG_FRIEND_RFL( DeProfileData )

private:

	DeClient* m_OwningClient;

	egprop eg_int m_HighScore;

public:

	void Init( DeClient* InOwningClient );
	void SaveProfile();
	eg_int GetHighScore() const { return m_HighScore; }
	void AddNewScore( eg_int NewScoreIn );
};
