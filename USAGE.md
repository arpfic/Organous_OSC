## USAGE -- OSC Commands

### MAIN commands

#### OSC msg  : /main/coil ii PORT INTENSITY
 * Purpose   : drive coilOn/coilOff functions
 * Note      : For now, INTENSITY is almost useless : we just launch coilOff if == 0
 * Function  : *menu_main_coil()*

#### OSC msg  : /main/oe ff CYCLE_RATIO PERIOD_SEC
 * Purpose   : set OE FastPWM config and control blinking of all LEDS at the same time
 * Note      : can be used in conjunction with other functions -- currently we DON'T touch ENABLE table
 * Function  : *menu_main_oe()*

#### OSC msg  : /main/tone f FREQ
 * Purpose   : Play a "note" in class-D style through all DRV8844 OUT and with OE setting
 * Note      : Very experimental and ugly. But promising.
 * Function  : *menu_main_tone()*

### LOWLEVEL commands

#### OSC msg  : /lowlevel/output ii PORT 0/1
 * Purpose   : set/unset OUTPUT with ENABLE pin on DRV8844 (see datasheet)
 * Function  : *menu_lowlevel_output()*

#### OSC msg  : /lowlevel/output_all i 0/1
 * Purpose   : set/unset ALL OUTPUTS with ENABLE pin on DRV8844 (see datasheet)
 * Function  : *menu_lowlevel_output_all()*

#### OSC msg  : /lowlevel/output_state i PORT
 * Purpose   : just return OUTPUT state
 * Function  : *menu_lowlevel_output_state()*

#### OSC msg  : /lowlevel/pwm if PORT RATIO
 * Purpose   : control blinking of LEDS with RATIO[0:1]
 * Function  : *menu_lowlevel_pwm()*

#### OSC msg  : /lowlevel/pwm_all f RATIO
 * Purpose   : control blinking of all LEDS at the same time with RATIO[0:1]
 * Note      : Can be used in conjunction with OE
 * Function  : *menu_lowlevel_pwm_all()*

#### OSC msg  : /lowlevel/pwm_state i PORT
 * Purpose   : read the PWM state of PORT
 * Note      : Nothing for now.
 * Function  : *menu_lowlevel_pwm_state()*

### Tools commands

#### OSC msg  : /tools/connect NONE (Bang)
 * Purpose   : connect NUCLEO_F767ZI to client (set IP address)
 * Function  : *menu_tools_connect()*

#### OSC msg  : /tools/debug NONE (Bang)
 * Purpose   : set debug ON to send many messages to client
 * Function  : *menu_tools_debug()*

#### OSC msg  : /tools/hardreset NONE (Bang)
 * Purpose   : Hard reset the board
 * Function  : *menu_tools_hardreset()*

#### OSC msg  : /tools/softreset NONE (Bang)
 * Purpose   : "Soft reset" the board : set to init as possible
 * Function  : *menu_tools_softreset()*

#### OSC msg  : /tools/forceoff_all NONE (Bang)
 * Purpose   : re-init OE and call forceoff ALLPORTS
 * Function  : *menu_tools_forceoff_all()*

#### OSC msg  : /tools/count i COUNT
 * Purpose   : Just ping pong from client for network reliability test
 * Function  : *menu_tools_count()*
