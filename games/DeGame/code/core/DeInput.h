// (c) 2018 Beem Media

#pragma once

#include "EGInputTypes.h"

namespace de_cmd_t
{
	static const eg_cmd_t PAUSEMENU  = 1;
	static const eg_cmd_t FORWARD1   = 2;
	static const eg_cmd_t FORWARD2   = 3;
	static const eg_cmd_t BACK1      = 4;
	static const eg_cmd_t BACK2      = 5;
	static const eg_cmd_t TURNLEFT1  = 6;
	static const eg_cmd_t TURNLEFT2  = 7;
	static const eg_cmd_t TURNRIGHT1 = 8;
	static const eg_cmd_t TURNRIGHT2 = 9;
	static const eg_cmd_t MOVELEFT1  = 10;
	static const eg_cmd_t MOVELEFT2  = 11;
	static const eg_cmd_t MOVERIGHT1 = 12;
	static const eg_cmd_t MOVERIGHT2 = 13;
	static const eg_cmd_t SHOOT      = 14;
}

void DeInput_RegisterInput( ISdkInputRegistrar* Registrar );

struct deInput
{
private:

	egLockstepCmds Cmds;

public:

	deInput( const egLockstepCmds& CmdsIn )
	: Cmds( CmdsIn )
	{

	}

	eg_real GetAxisX() const { return Cmds.GetAxisX(); }
	eg_real GetAxisY() const { return Cmds.GetAxisY(); }

	eg_bool IsMoveForwardActive() const
	{
		return Cmds.IsActive( de_cmd_t::FORWARD1 ) || Cmds.IsActive( de_cmd_t::FORWARD2 );
	}

	eg_bool IsMoveBackActive() const
	{
		return Cmds.IsActive( de_cmd_t::BACK1 ) || Cmds.IsActive( de_cmd_t::BACK2 );
	}

	eg_bool IsTurnRightActive() const
	{
		return Cmds.IsActive( de_cmd_t::TURNRIGHT1 ) || Cmds.IsActive( de_cmd_t::TURNRIGHT2 );
	}

	eg_bool IsTurnLeftActive() const
	{
		return Cmds.IsActive( de_cmd_t::TURNLEFT1 ) || Cmds.IsActive( de_cmd_t::TURNLEFT2 );
	}

	eg_bool IsMoveRightActive() const
	{
		return Cmds.IsActive( de_cmd_t::MOVERIGHT1 ) || Cmds.IsActive( de_cmd_t::MOVERIGHT2 );
	}

	eg_bool IsMoveLeftActive() const
	{
		return Cmds.IsActive( de_cmd_t::MOVELEFT1 ) || Cmds.IsActive( de_cmd_t::MOVELEFT2 );
	}

	eg_bool WasPausePressed() const
	{
		return Cmds.WasMenuPressed( de_cmd_t::PAUSEMENU );
	}

	eg_vec3 GetMoveVector() const
	{
		eg_vec3 Out = CT_Clear;

		if( IsMoveRightActive() )
		{
			Out.x += 1.f;
		}
		if( IsMoveLeftActive() )
		{
			Out.x -= 1.f;
		}
		if( IsMoveForwardActive() )
		{
			Out.z += 1.f;
		}
		if( IsMoveBackActive() )
		{
			Out.z -= 1.f;
		}

		Out.x = EG_Clamp( Out.x , -1.f , 1.f );
		Out.y = EG_Clamp( Out.y , -1.f , 1.f );
		Out.z = EG_Clamp( Out.z , -1.f , 1.f );

		return Out;
	}

	eg_bool WasShootPressed() const { return Cmds.WasPressed( de_cmd_t::SHOOT ); }
	eg_bool IsShootHeld() const { return Cmds.IsActive( de_cmd_t::SHOOT ); }
};
