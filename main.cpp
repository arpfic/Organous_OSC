#include "main.h"

DigitalOut  led_green(LED_GREEN);
DigitalOut  led_blue(LED_BLUE);
DigitalOut  led_red(LED_RED);
InterruptIn button(USER_BUTTON);

EthernetInterface *eth;
UDPSocket *my_socket;
SocketAddress *source_addr;

EventQueue queue(256 * EVENTS_EVENT_SIZE);
Thread thrd;

tosc_message* p_osc;

// Menu
struct menu_cases {
    const char* menu_string;
    void (*menu_func)(void);
};

menu_cases cases [] = {
    { "/main", menu_main },
    { "/tools", menu_tools }
};

menu_cases tools_cases [] = {
    { "/tools/connect",     menu_tools_connect },
    { "/tools/debug",       menu_tools_debug },
    { "/tools/hardreset",   menu_tools_hardreset },
    { "/tools/softreset",   menu_tools_softreset }
};

menu_cases main_cases [] = {
    { "/main/coil",         menu_main_coil },
    { "/main/forceoff_all",  menu_main_forceoff_all },
    { "/main/pwm_all",      menu_main_pwm_all },
    { "/main/oe",           menu_main_oe },
    { "/main/tone",         menu_main_tone }
};

// Drivers
CoilDriver* driver_A;
#if B_SIDE == 1
CoilDriver* driver_B;
#endif

// Packet size limitation = MAX_PQT_LENGTH
char    mainpacket_buffer[MAX_PQT_LENGTH];
int     mainpacket_length = 0;
// Prévoir plusieurs master ? Ou bien broadcaster comme un bourrin ?
char*   master_address;
// Debug
char    debug_on = 0;

// Little sampler ticker/timer (beta)
volatile int k = 0;
float sinusoid_data[128];
Ticker sample_ticker;
float tone = 500.0;

static void handle_socket()
{
    queue.call(receive_message);
}

static void receive_message()
{
    // Read data from the socket
    memset(mainpacket_buffer, 0, sizeof(mainpacket_buffer));
    bool something_in_socket = true;
    // read all messages
    while (something_in_socket) {
        mainpacket_length = my_socket->recvfrom(source_addr,
                                                mainpacket_buffer, sizeof(mainpacket_buffer) - 1);
        if (mainpacket_length > 0) {
            handler_Packetevent();
        } else if (mainpacket_length!=NSAPI_ERROR_WOULD_BLOCK) {
            // Error while receiving
            led_red = !led_red;
            something_in_socket=false;
        } else {
            // there was nothing to read.
            something_in_socket=false;
        }
    }
}

void menu_tools_connect()
{
    strncpy(master_address, const_cast<char*>(source_addr->get_ip_address()), 16 * sizeof(char));

    // RE-assign socket with the new address
    delete source_addr;
    source_addr = new SocketAddress;

    debug_OSC("OK");
    if (debug_on)
        debug_OSC(master_address);
}

void menu_tools_debug()
{
    debug_on = 1;
    debug_OSC("DEBUG ON");
}

void menu_tools_hardreset()
{
    // On montre l'activité
    led_green = led_blue = led_red = 1;
    debug_OSC("REBOOTING...");
    //wait(1);
    // Ticker destruct
    sample_ticker.detach();
    // Delete classes
    delete driver_A;
#if B_SIDE == 1
    delete driver_B;
#endif
    delete eth;
    delete my_socket;
    delete source_addr;
    delete master_address;
    // Just reset the main board
    NVIC_SystemReset();
    //HAL_NVIC_SystemReset();
}

void menu_tools_softreset()
{
    // On montre l'activité
    led_green = led_blue = led_red = 1;
    debug_OSC("RESET STATES...");
    // Ticker destruct
    sample_ticker.detach();
    // Set All enables to 0
    driver_A->forceoff(ALLPORTS);
    // Deleting all objects for reninit
    delete driver_A;
    // Create new objects
    driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                              DRV_A_FAULT, driver_a_table, 0xD2);
#if B_SIDE == 1
    driver_B->forceoff(ALLPORTS);
    delete driver_B;
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, 0x2A);
#endif
    led_green = led_blue = led_red = 0;
}

void menu_main_coil()
{
    // On montre l'activité
    led_blue = !led_blue;
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int tone = tosc_getNextInt32(p_osc);
        int release = tosc_getNextInt32(p_osc);
        if (tone >= 0 && tone < 24 ) {
            if (release == 0) {
                if (debug_on) {char buf[64]; sprintf(buf, "COIL %i : %i use(s)", tone, (int)driver_A->OUTRegister[tone]); debug_OSC(buf);}
                driver_A->coilOff(tone);
            } else {
                driver_A->coilOn(tone);
            }
#if B_SIDE == 1
        } else if (tone >= 24 && tone < 48 ) {
            if (release == 0) {
                if (debug_on) {char buf[64]; sprintf(buf, "COIL %i : %i use(s)", tone, (int)driver_B->OUTRegister[tone - 24]); debug_OSC(buf);}
                driver_B->coilOff(tone - 24);
            } else {
                driver_B->coilOn(tone - 24);
            }
#endif
        }
    }
}

void menu_main_forceoff_all()
{
    // On montre l'activité
    led_blue = !led_blue;
    driver_A->forceoff(ALLPORTS);
    driver_A->oeCycle(0.0f);
    driver_A->oePeriod(1.0f);
#if B_SIDE == 1
    driver_B->forceoff(ALLPORTS);
    driver_B->oeCycle(0.0f);
    driver_B->oePeriod(1.0f);
    sample_ticker.detach();
#endif
}

void menu_main_pwm_all()
{
    // On montre l'activité
    led_blue = !led_blue;
    if (p_osc->format[0] == 'f') {
        float pwm = tosc_getNextFloat(p_osc);
        if (pwm > 0 && pwm <=1 ) {
            driver_A->pwmSet(ALLPORTS, pwm);
            driver_A->drvEnable(ALLPORTS, true);
#if B_SIDE == 1
            driver_B->pwmSet(ALLPORTS, pwm);
            driver_B->drvEnable(ALLPORTS, true);
#endif
        } else if (pwm == 0.0f) {
            driver_A->pwmSet(ALLPORTS, pwm);
            driver_A->drvEnable(ALLPORTS, false);
#if B_SIDE == 1
            driver_B->pwmSet(ALLPORTS, pwm);
            driver_B->drvEnable(ALLPORTS, false);
#endif
        }
    }
}

void menu_main_oe()
{
    // On montre l'activité
    led_blue = !led_blue;
    if (p_osc->format[0] == 'f') {
        float cycle = tosc_getNextFloat(p_osc);
        if (cycle > 0 && cycle <=1 ) {
            driver_A->oeCycle(cycle);
#if B_SIDE == 1
            driver_B->oeCycle(cycle);
#endif
        }
    }
    if (p_osc->format[1] == 'f') {
        float period = tosc_getNextFloat(p_osc);
        if (period > 0) {
            driver_A->oePeriod(period);
#if B_SIDE == 1
            driver_B->oePeriod(period);
#endif
        }
    }
}

void menu_main_tone()
{
    // On montre l'activité
    led_blue = !led_blue;
    if (p_osc->format[0] == 'f') {
        float tone = tosc_getNextFloat(p_osc);
        if (tone >0) {
            // Little sampler Period init
            driver_A->oePeriod(1.0/200000.0);
#if B_SIDE == 1
            driver_B->oePeriod(1.0/200000.0);
#endif
            sample_ticker.detach();
            sample_ticker.attach_us(&sampler_timer, (tone)*128);
        }
    }
}

void menu_main()
{
    for (menu_cases* p_case = main_cases;
            p_case != main_cases + sizeof(main_cases) / sizeof(main_cases[0]);
            p_case++) {
        if (strcmp(tosc_getAddress(p_osc), p_case->menu_string) == 0) {
            (*p_case->menu_func)();
        }
    }
}

void menu_tools()
{
    for (menu_cases* p_case = tools_cases;
            p_case != tools_cases + sizeof(tools_cases) / sizeof(tools_cases[0]);
            p_case++) {
        if (strcmp(tosc_getAddress(p_osc), p_case->menu_string) == 0) {
            (*p_case->menu_func)();
        }
    }
}

void handler_Packetevent()
{
    if (debug_on == 1)
        debug_OSCmsg(mainpacket_buffer, mainpacket_length);

    tosc_message osc;
    p_osc = &osc;

    if (!tosc_parseMessage(&osc, mainpacket_buffer, mainpacket_length)) {
        for (menu_cases* p_case = cases;
                p_case != cases + sizeof(cases) / sizeof(cases[0]);
                p_case++) {
            if (strncmp(tosc_getAddress(&osc), p_case->menu_string,
                        strlen(p_case->menu_string)) == 0) {
                (*p_case->menu_func)();
            }
        }
    } else {
        led_red = !led_red;
    }
}

void init_msgON()
{
    char buffer[MAX_PQT_LENGTH];

    sprintf(buffer, "UP ! My IP address is :");
    sprintf(buffer + strlen(buffer), " %s", const_cast<char*>(eth->get_ip_address()));
    sprintf(buffer + strlen(buffer), " PLEASE CONNECT !");

    debug_OSC(buffer);

    // Everything is OK
    led_green = !led_green;
}

static void send_UDPmsg(char* incoming_msg, int in_length)
{
    source_addr->set_ip_address(master_address);
    source_addr->set_port(OSC_CLIENT_PORT);
    my_socket->sendto(*(source_addr), incoming_msg, in_length);
}

void button_pressed()
{
    led_red = 1;
    queue.call(init_msgON);
}

void button_released()
{
    led_red = 0;
}

// Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void sampler_timer()
{
//    driver_A->drvEnable(ALLPORTS, 1);
//    driver_B->drvEnable(ALLPORTS, 1);
    // send next analog sample out to D to A
    driver_A->oeCycle(sinusoid_data[k]);
#if B_SIDE == 1
    driver_B->oeCycle(sinusoid_data[k]);
#endif
    // increment pointer and wrap around back to 0 at 128
    k = (k+2) & 0x07F;
}

void eth_status_callback(nsapi_event_t status, intptr_t param)
{
    if (param != NSAPI_STATUS_GLOBAL_UP && param != NSAPI_STATUS_LOCAL_UP)
        NVIC_SystemReset(); // Just reset the main board
}


void driver_A_error_handler()
{
    debug_OSC("OVERCURRENT ERROR on card A. Please reset");
    led_red = 1;
}

void driver_B_error_handler()
{
    debug_OSC("OVERCURRENT ERROR on card B. Please reset");
    led_red = 1;
}


int main()
{
    // Init ip to broadcast
    master_address = (char *)malloc(16 * sizeof(char));
    sprintf(master_address, "255.255.255.255");

    driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                              DRV_A_FAULT, driver_a_table, 0xD2);
    // Set-up driver_A & driver_B error feedbacks
    driver_A->drv_fault.fall(queue.event(driver_A_error_handler));

#if B_SIDE == 1
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, 0x2A);
    driver_B->drv_fault.fall(queue.event(driver_B_error_handler));
#endif

    // Set-up button
    button.fall(&button_released);
    button.rise(&button_pressed);

    eth = new EthernetInterface;
    nsapi_error_t status = eth->connect();
//    if (status < 0)
//        NVIC_SystemReset();        // Just reset the main board
    // In case of disconnection
    eth->attach(&eth_status_callback);

    my_socket = new UDPSocket(eth);
    my_socket->set_blocking(false);
    my_socket->open(eth);

    source_addr = new SocketAddress;

    while(my_socket->bind(OSC_CLIENT_PORT) != 0);
    my_socket->sigio(callback(handle_socket));

    // dispatch forever
    thrd.start(callback(&queue, &EventQueue::dispatch_forever));

    // Send Up message to broadcast
    init_msgON();

    // precompute 128 sample points on one sine wave cycle
    // used for continuous sine wave output later
    for(int i=0; i<128; i++) {
        sinusoid_data[i]=((1.0 + sin((float(i)/128.0*6.28318530717959)))/2.0);
    }

    while (true) {
        /*
        for ( int j = 0; j <= 23; j++ ) {
            driver_B->coilOn(j);
            wait(0.1);
        }
        */
    }
}