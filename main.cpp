#include "main.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
InterruptIn button(USER_BUTTON);

EthernetInterface *eth;
UDPSocket *my_socket;
SocketAddress source_addr;

EventQueue queue;
Thread thrd;

// Drivers
CoilDriver* driver_A;
CoilDriver* driver_B;

// Packet size limitation = RECEIVE_LENGTH
char receive_buffer[RECEIVE_LENGTH];
char     debug_msg[1024];
int    packet_length = 0;
// Prévoir plusieurs master ? Ou bien broadcaster comme un bourrin ?
char*     master_address;

// Debug
char        debug_on = 0;

// Little sampler ticker/timer
volatile int k=0;
float sinusoid_data[128];
Ticker sample_ticker;
float tone = 500.0;

// outsz = 3x insz
void tohex(unsigned char * in, size_t insz, char * out, size_t outsz)
{
    unsigned char * pin = in;
    const char * hex = "0123456789ABCDEF";
    char * pout = out;
    for(; pin < in+insz; pout +=3, pin++) {
        pout[0] = hex[(*pin>>4) & 0xF];
        pout[1] = hex[ *pin     & 0xF];
        pout[2] = ':';
        if (pout + 3 - out > outsz) {
            /* Better to truncate output string than overflow buffer */
            /* it would be still better to either return a status */
            /* or ensure the target buffer is large enough and it never happen */
            break;
        }
    }
    pout[-1] = 0;
}

static void handle_socket()
{
    queue.call(receive_message);
}

static void send_message()
{
    debugOSCmsgON();
}

static void receive_message()
{
    // Read data from the socket
    memset(receive_buffer, 0, sizeof(receive_buffer));
    bool something_in_socket = true;
    // read all messages
    while (something_in_socket) {
        packet_length = my_socket->recvfrom(&source_addr,
                                            receive_buffer, sizeof(receive_buffer) - 1);
        if (packet_length > 0) {
            onOSCReceiveEvent();
        } else if (packet_length!=NSAPI_ERROR_WOULD_BLOCK) {
//            dprintf("Error happened when receiving %d", packet_length);
            something_in_socket=false;
        } else {
            // there was nothing to read.
            something_in_socket=false;
        }
    }
}

void onOSCReceiveEvent()
{
    // On montre l'activité
    led1 = !led1;

    union OSCarg msg[10];
    decOSCmsg(receive_buffer, msg);

    if (debug_on == 1)
        debugUDPmsg(receive_buffer, RECEIVE_LENGTH);

    if (strcmp(msg[0].address, "/tools/connect") == 0) {
        strncpy(master_address, const_cast<char*>(source_addr.get_ip_address()), 16 * sizeof(char));
        debugOSC("OK");
        if (debug_on == 1)
            debugOSC(master_address);
    }
    if (strcmp(msg[0].address, "/tools/debug") == 0) {
        if ((unsigned int)msg[2].i == 1) {
            debug_on = 1;
            debugOSC("DEBUG on");
        }
    }
    if (strcmp(msg[0].address, "/tools/hardreset") == 0) {
        // On montre l'activité
        led1 = led2 = led3 = 1;
        debugOSC("REBOOTING...");
        wait(1);
        // Ticker destruct
        sample_ticker.detach();
        // Delete classes
        delete driver_A;
        delete driver_B;
        delete eth;
        delete my_socket;
        delete master_address;
        // Just reset the main board
        NVIC_SystemReset();
        //HAL_NVIC_SystemReset();
    }
    if (strcmp(msg[0].address, "/tools/softreset") == 0) {
        // On montre l'activité
        led1 = led2 = led3 = 1;
        // Ticker destruct
        sample_ticker.detach();
        // Set All enables to 0
        driver_A->off(ALLPORTS);
        driver_B->off(ALLPORTS);
        // Deleting all objects for reninit
        delete driver_A;
        delete driver_B;
        // Create new objects
        driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                                  DRV_A_FAULT, driver_a_table, 0xD2);
        driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                                  DRV_B_FAULT, driver_b_table, 0x2A);
        led1 = led2 = led3 = 0;
    }
    if (strcmp(msg[0].address, "/main/coil") == 0) {
        // On montre l'activité
        led2 = !led2;
        if ((unsigned int)msg[3].i < 24) {
            if ((unsigned int)msg[4].i != 0) {
                driver_A->coilOn((unsigned int)msg[3].i);
            } else {
                driver_A->coilOff((unsigned int)msg[3].i);
            }
        } else if ((unsigned int)msg[3].i < 48) {
            if ((unsigned int)msg[4].i != 0) {
                driver_B->coilOn((unsigned int)msg[3].i - 24);
            } else {
                driver_B->coilOff((unsigned int)msg[3].i - 24);
            }
        }
    }
    if (strcmp(msg[0].address, "/main/off_all") == 0) {
        // On montre l'activité
        led2 = !led2;
        driver_A->off(ALLPORTS);
        driver_B->off(ALLPORTS);
        driver_A->oeCycle(0.0f);
        driver_A->oePeriod(1.0f);
        driver_B->oeCycle(0.0f);
        driver_B->oePeriod(1.0f);
        sample_ticker.detach();
    }
    if (strcmp(msg[0].address, "/main/pwm_all") == 0) {
        // On montre l'activité
        led2 = !led2;
        driver_A->on(ALLPORTS, ((float)msg[2].f));
        driver_B->on(ALLPORTS, ((float)msg[2].f));
    }
    if (strcmp(msg[0].address, "/main/oe") == 0) {
        // On montre l'activité
        led2 = !led2;
//        driver_A->on(ALLPORTS, 1.0);
//        char* buf;
//        sprintf(buf, "%f %f", msg[2].f, msg[3].f);
//        debugOSC(buf);
        driver_A->oeCycle((float)msg[2].f);
        driver_A->oePeriod((float)msg[3].f);
        driver_B->oeCycle((float)msg[2].f);
        driver_B->oePeriod((float)msg[3].f);
    }
    if (strcmp(msg[0].address, "/main/tone") == 0) {
        // On montre l'activité
        led2 = !led2;
        /* Little sampler Period init */
        driver_A->oePeriod(1.0/200000.0);
        driver_B->oePeriod(1.0/200000.0);
        sample_ticker.attach(&sampler_timer, 1.0/(((float)msg[2].f)*64));
    }
}

void debugOSCmsgON()
{
    char packet[RECEIVE_LENGTH];
    int len;

    union OSCarg outmsg[10];

    // setup OSC message
    outmsg[0].address="/debug";
    outmsg[1].typeTag=",sss";
    outmsg[2].s=IF_NAME" ("IF_VER"): UP ! My IP address is :";
    outmsg[3].s=const_cast<char*>(eth->get_ip_address());
    outmsg[4].s=" PLEASE CONNECT !";

    // make packet from OSC message
    memset(packet,0,sizeof(packet)); // clear send buff for OSC msg
    len = encOSCmsg(packet, outmsg);

    // Send packet
    source_addr.set_ip_address("255.255.255.255");
    source_addr.set_port(OSC_CLIENT_PORT);
    my_socket->sendto(source_addr, packet, len);
    source_addr.set_ip_address(master_address);
}

void debugOSC(char* debug_msg)
{
    char packet[RECEIVE_LENGTH];
    int len;

    union OSCarg outmsg[10];

    // setup OSC message
    outmsg[0].address="/debug";
    outmsg[1].typeTag=",ss";
    outmsg[2].s=IF_NAME" ("IF_VER"):";
    outmsg[3].s=debug_msg;

    // make packet from OSC message
    memset(packet,0,sizeof(packet)); // clear send buff for OSC msg
    len = encOSCmsg(packet, outmsg);

    // Send packet
    source_addr.set_ip_address(master_address);
    source_addr.set_port(OSC_CLIENT_PORT);
    my_socket->sendto(source_addr, packet, len);
}

static void debugUDPmsg(char* debug_msg, int length)
{
    source_addr.set_ip_address(master_address);
    source_addr.set_port(OSC_CLIENT_PORT);
    my_socket->sendto(source_addr, debug_msg, length);
}

void pressed()
{
    led3 = 1;
    queue.call(send_message);
}

void released()
{
    led3 = 0;
}

// Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void sampler_timer(void)
{
    driver_A->drvEnable(ALLPORTS, 1);
    driver_B->drvEnable(ALLPORTS, 1);
    // send next analog sample out to D to A
    driver_A->oeCycle(sinusoid_data[k]);
    driver_B->oeCycle(sinusoid_data[k]);
    // increment pointer and wrap around back to 0 at 128
    k = (k+2) & 0x07F;
}

int main()
{
    // Init ip to broadcast
    master_address = (char *)malloc(16 * sizeof(char));
    sprintf(master_address, "255.255.255.255");

    driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                              DRV_A_FAULT, driver_a_table, 0xD2);
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, 0x2A);

    // Set-up button
    button.fall(&released);
    button.rise(&pressed);

    eth = new EthernetInterface;
    eth->connect();

    my_socket = new UDPSocket(eth);
    my_socket->set_blocking(false);
    my_socket->open(eth);

    while(my_socket->bind(OSC_CLIENT_PORT) != 0);
    my_socket->sigio(callback(handle_socket));

    // dispatch forever
    thrd.start(callback(&queue, &EventQueue::dispatch_forever));

    // Send Up message to broadcast
    debugOSCmsgON();

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