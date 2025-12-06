// (c) 2018 Beem Media

#include "DeInput.h"

struct deInputMapping
{
	eg_cmd_t Cmd;
	eg_cpstr Name;
	eg_cpstr GpButton;
	eg_cpstr KbButton;
};

static const deInputMapping INPUT_MAPPING[] =
{
	{ de_cmd_t::PAUSEMENU  , "PAUSEMENU"  , "GP_START"  , "KBM_ESCAPE"  },
	{ de_cmd_t::FORWARD1   , "FORWARD1"   , "GP_DUP"    , "KBM_W"       },
	{ de_cmd_t::FORWARD2   , "FORWARD2"   , "GP_LUP"    , "KBM_UP"      },
	{ de_cmd_t::BACK1      , "BACK1"      , "GP_DDOWN"  , "KBM_S"       },
	{ de_cmd_t::BACK2      , "BACK2"      , "GP_LDOWN"  , "KBM_DOWN"    },
	{ de_cmd_t::TURNRIGHT1 , "TURNRIGHT1" , ""          , ""            },
	{ de_cmd_t::TURNRIGHT2 , "TURNRIGHT2" , ""          , "KBM_RIGHT"   },
	{ de_cmd_t::TURNLEFT1  , "TURNLEFT1"  , ""          , ""            },
	{ de_cmd_t::TURNLEFT2  , "TURNLEFT2"  , ""          , "KBM_LEFT"    },
	{ de_cmd_t::MOVERIGHT1 , "MOVERIGHT1" , "GP_DRIGHT" , "KBM_D"       },
	{ de_cmd_t::MOVERIGHT2 , "MOVERIGHT2" , "GP_LRIGHT" , ""            },
	{ de_cmd_t::MOVELEFT1  , "MOVELEFT1"  , "GP_DLEFT"  , "KBM_A"       },
	{ de_cmd_t::MOVELEFT2  , "MOVELEFT2"  , "GP_LLEFT"  , ""            },
	{ de_cmd_t::SHOOT      , "SHOOT"      , "GP_R2"     , "KBM_MOUSE_1" },
};

void DeInput_RegisterInput( ISdkInputRegistrar* Registrar )
{
	for( const deInputMapping& Mapping : INPUT_MAPPING )
	{
		Registrar->SDK_RegisterInput( Mapping.Cmd, Mapping.Name, Mapping.GpButton, Mapping.KbButton );
	}
}
