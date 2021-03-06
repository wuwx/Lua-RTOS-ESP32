/*
 * Lua RTOS, PWM Lua module
 *
 * Copyright (C) 2015 - 2017
 * IBEROXARXA SERVICIOS INTEGRALES, S.L. & CSS IBÉRICA, S.L.
 * 
 * Author: Jaume Olivé (jolive@iberoxarxa.com / jolive@whitecatboard.org)
 * 
 * All rights reserved.  
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#include "luartos.h"

#if CONFIG_LUA_RTOS_LUA_USE_PWM

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "pwm.h"
#include "error.h"
#include "modules.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <drivers/cpu.h>
#include <drivers/pwm.h>

static int lpwm_attach( lua_State* L ) {
	driver_error_t *error;
    int8_t pin;
    int32_t freq;
    double duty;

    pwm_userdata *pwm = (pwm_userdata *)lua_newuserdata(L, sizeof(pwm_userdata));

    pwm->unit = 0;

    pin = luaL_checkinteger( L, 1 );
    freq = luaL_checkinteger( L, 2 );
    duty = luaL_checknumber(L, 3);

    if ((error = pwm_setup(pwm->unit, -1, pin, freq, duty, &pwm->channel))) {
    	return luaL_driver_error(L, error);
    }

    luaL_getmetatable(L, "pwm.inst");
    lua_setmetatable(L, -2);

    return 1;
}

static int lpwm_setupchan( lua_State* L ) {
	driver_error_t *error;
    int8_t pin;
    int32_t freq;
    double duty;

	luaL_deprecated(L, "pwm.setupchan", "pwm.attach");

    pwm_userdata *pwm = (pwm_userdata *)lua_newuserdata(L, sizeof(pwm_userdata));

    pwm->unit = luaL_checkinteger( L, 1 );
    pin = luaL_checkinteger( L, 2 );
    freq = luaL_checkinteger( L, 3 );
    duty = luaL_checknumber(L, 4);

    if ((error = pwm_setup(pwm->unit, -1, pin, freq, duty, &pwm->channel))) {
    	return luaL_driver_error(L, error);
    }

    luaL_getmetatable(L, "pwm.inst");
    lua_setmetatable(L, -2);

    return 1;
}

static int lpwm_setfreq(lua_State* L) {
    pwm_userdata *pwm = NULL;
	driver_error_t *error;
	int32_t freq;

    pwm = (pwm_userdata *)luaL_checkudata(L, 1, "pwm.inst");
    luaL_argcheck(L, pwm, 1, "pwm expected");

    freq = luaL_checknumber(L, 2);

    if ((error = pwm_set_freq(pwm->unit, pwm->channel, freq))) {
    	return luaL_driver_error(L, error);
    }

    return 0;
}

static int lpwm_setduty(lua_State* L) {
    pwm_userdata *pwm = NULL;
	driver_error_t *error;
	double duty;

    pwm = (pwm_userdata *)luaL_checkudata(L, 1, "pwm.inst");
    luaL_argcheck(L, pwm, 1, "pwm expected");

    duty = luaL_checknumber(L, 2);

    if ((error = pwm_set_duty(pwm->unit, pwm->channel, duty))) {
    	return luaL_driver_error(L, error);
    }

    return 0;
}

static int lpwm_start(lua_State* L) {
    pwm_userdata *pwm = NULL;
	driver_error_t *error;

    pwm = (pwm_userdata *)luaL_checkudata(L, 1, "pwm.inst");
    luaL_argcheck(L, pwm, 1, "pwm expected");

    if ((error = pwm_start(pwm->unit, pwm->channel))) {
    	return luaL_driver_error(L, error);
    }

    return 0;
}

static int lpwm_stop(lua_State* L) {
    pwm_userdata *pwm = NULL;
	driver_error_t *error;

    pwm = (pwm_userdata *)luaL_checkudata(L, 1, "pwm.inst");
    luaL_argcheck(L, pwm, 1, "pwm expected");

    if ((error = pwm_stop(pwm->unit, pwm->channel))) {
    	return luaL_driver_error(L, error);
    }

    return 0;
}

static const LUA_REG_TYPE lpwm_map[] = {
    { LSTRKEY("setupchan" ),LFUNCVAL(lpwm_setupchan) },
    { LSTRKEY("attach" ),	LFUNCVAL(lpwm_attach)    },
	PWM_PWM0
	PWM_PWM1
	PWM_PWM_CH0
	PWM_PWM_CH1
	PWM_PWM_CH2
	PWM_PWM_CH3
	PWM_PWM_CH4
	PWM_PWM_CH5
	PWM_PWM_CH6
	PWM_PWM_CH7
	PWM_PWM_CH8
	PWM_PWM_CH9
	PWM_PWM_CH10
	PWM_PWM_CH11
	PWM_PWM_CH12
	PWM_PWM_CH13
	PWM_PWM_CH14
	PWM_PWM_CH15
	DRIVER_REGISTER_LUA_ERRORS(pwm)
  	{ LNILKEY, LNILVAL }
};

static const LUA_REG_TYPE lpwm_channel_map[] = {
  { LSTRKEY( "setduty"        ),	 LFUNCVAL( lpwm_setduty                ) },
  { LSTRKEY( "setfreq"        ),	 LFUNCVAL( lpwm_setfreq                ) },
  { LSTRKEY( "start"          ),	 LFUNCVAL( lpwm_start                  ) },
  { LSTRKEY( "stop"           ),	 LFUNCVAL( lpwm_stop                   ) },
  { LSTRKEY( "__metatable"    ),	 LROVAL  ( lpwm_channel_map            ) },
  { LSTRKEY( "__index"        ),   	 LROVAL  ( lpwm_channel_map            ) },
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_pwm( lua_State *L ) {
    luaL_newmetarotable(L,"pwm.inst", (void *)lpwm_channel_map);
    return 0;
}

MODULE_REGISTER_MAPPED(PWM, pwm, lpwm_map, luaopen_pwm);

#endif
