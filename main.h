#if !FEATURE_LWIP
    #error [NOT_SUPPORTED] LWIP not supported for this target
#endif

#include "mbed.h"
#include "config.h"
#include "BufferedSerial.h"
#include "EthernetInterface.h"
#include "UDPSocket.h"
#include "OSC.h"
#include "FastPWM.h"
#include "main_driver_hal.h"
#include "PCA9956A.h"

DigitalOut driver_a_table[ENABLE_PINS] = {
    DRV_EN1,  DRV_EN2,  DRV_EN3,  DRV_EN4,  DRV_EN5,  DRV_EN6,  DRV_EN7,  DRV_EN8,
DRV_EN9,  DRV_EN10, DRV_EN11, DRV_EN12, DRV_EN13, DRV_EN14, DRV_EN15, DRV_EN16,
DRV_EN17, DRV_EN18, DRV_EN19, DRV_EN20, DRV_EN21, DRV_EN22, DRV_EN23, DRV_EN24
    };
    
DigitalOut driver_b_table[ENABLE_PINS] = {
    DRV_EN25, DRV_EN26, DRV_EN27, DRV_EN28, DRV_EN29, DRV_EN30, DRV_EN31, DRV_EN32,
DRV_EN33, DRV_EN34, DRV_EN35, DRV_EN36, DRV_EN37, DRV_EN38, DRV_EN39, DRV_EN40,
DRV_EN41, DRV_EN42, DRV_EN43, DRV_EN44, DRV_EN45, DRV_EN46, DRV_EN47, DRV_EN48
    };

static void handle_socket();
static void send_message();
static void receive_message();
void onOSCReceiveEvent();
void debugOSCmsgON();
void debugOSC(char* debug_msg);
static void debugUDPmsg(char*, int);
void pressed();
void released();
int main();
void sampler_timer(void);