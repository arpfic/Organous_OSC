## USAGE -- OSC Commands

#### OSC msg  : /tools/connect NONE (Bang)
 * Purpose  : connect NUCLEO_F767ZI to client (set IP address)
 * Function	: menu_tools_connect()

#### OSC msg    : /tools/debug NONE (Bang)
 * Purpose  : set debug ON to send many messages to client
 * Function	: menu_tools_debug()

#### OSC msg    : /tools/hardreset NONE (Bang)
 * Purpose  : Hard reset the board
 * Function	: menu_tools_hardreset()

#### OSC msg    : /tools/softreset NONE (Bang)
 * Purpose  : "Soft reset" the board : set to init as possible
 * Function	: menu_tools_softreset()

#### OSC msg    : /main/coil ii PORT INTENSITY
 * Purpose  : drive coilOn/coilOff functions
 * Note     : coilOff if INTENSITY == 0
 * Function	: menu_main_coil()

#### OSC msg    : /main/forceoff_all NONE (Bang)
 * Purpose  : re-init OE and call forceoff ALLPORTS
 * Function	: menu_main_forceoff_all()

#### OSC msg    : /main/pwm_all f RATIO
 * Purpose  : control blinking of all LEDS at the same time
 * Note     : Can be used in conjunction with OE
 * Function	: menu_main_pwm_all()

#### OSC msg    : /main/oe ff CYCLE_RATIO PERIOD_SEC
 * Purpose  : set OE FastPWM config and control blinking of all LEDS at the same time
 * Note     : can be used in conjunction with other functions -- currently we DON'T
              touch ENABLE table
 * Function	: menu_main_oe()

#### OSC msg    : /main/tone f FREQ
 * Purpose  : Play a "note" in class-D style OUT through all DRV8844 and with OE setting
 * Note     : COMPLETELY BROKEN at this state and to be debbuged (with the new mbed-os)

### OSC msg		: /main/count i COUNT
 * Purpose  : Just ping pong from client for network reliability test
 * Function : menu_main_count()
