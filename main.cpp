/*
    Copyright (c) 2020 Damien Leblois
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/
#include "main.h"
#include "MIDIMessage.h"
#include "ThisThread.h"
#include "main_debug.h"
#include "menu.h"
#include <cstdlib>

// Handlers
static void handle_udp_socket()
{
    queue_io.call(receive_udp_message);
}

static void handle_tcp_socket()
{
    return;
}

// Read data from the socket
static void receive_udp_message()
{
    // Zeroing the buffer. Bof
    // memset(mainpacket_buffer, 0, sizeof(mainpacket_buffer));
    // Read all messages
    bool something_in_socket = true;
    while (something_in_socket) {
        // Copy to the temporary buffer
        mainpacket_length = udp_socket->recvfrom(client_addr, mainpacket_buffer, sizeof(mainpacket_buffer) - 1);
        if (mainpacket_length > 0) {
            // Allocate a new packet in the buffer table :
            socketpacket* new_packet = create_buffer(mainpacket_length);
            if (new_packet == NULL) {
                led_red = !led_red;
                delete_array(new_packet);
            } else {
		// Copy to the packet...
                memcpy(new_packet->data, mainpacket_buffer, mainpacket_length);
                new_packet->size = mainpacket_length;
                // ... then inject it to Circularbuffer
                socketpacket_buf.push(new_packet);
                // The Thread needs to wake up !
                oscTask.flags_set(0x1);
            }
        } else if (mainpacket_length != NSAPI_ERROR_WOULD_BLOCK) {
            // Error while receiving
            led_red = !led_red;
            something_in_socket = false;
        } else {
            // Or there was nothing to read.
            something_in_socket = false;
        }
    }
}

// Read data from the socket
static void receive_tcp_message()
{
    return ;
}

/* Init NUCLEO_F767ZI message. See main.h
 */
void init_msgON()
{
    char buffer[MAX_PQT_SENDLENGTH];

    sprintf(buffer, "UP ! My IP address is :");
    sprintf(buffer + strlen(buffer), " %s", const_cast<char*>(ip->get_ip_address()));
    sprintf(buffer + strlen(buffer), " Please connect");

    debug_OSC(buffer);

    // Everything is OK
    led_green = !led_green;
}

/* Send UDP packet through UDPSocket. Call by debug*()
 */
static void send_UDPmsg(char* incoming_msg, int in_length)
{
    client_addr->set_ip_address(master_address);
    client_addr->set_port(OSC_CLIENT_PORT);
    udp_socket->sendto(*(client_addr), incoming_msg, in_length);
}

/* Button basic functions : we just resend init_msgON() when pressed
 */
void button_pressed()
{
    led_red = 1;
    if (eth->get_connection_status() == NSAPI_STATUS_GLOBAL_UP)
        queue_msg.call(init_msgON);
}

void button_released()
{
    led_red = 0;
}

// TONE experimental BROKEN function -- Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void sampler_timer()
{
    // send next analog sample out to D to A
    driver_A->oeCycle(sinusoid_data[k]);
#if B_SIDE == 1
    driver_B->oeCycle(sinusoid_data[k]);
#endif
    // increment pointer and wrap around back to 0 at 128
    k = (k+2) & 0x07F;
}

/* (Very) basic implementation of EthernetInterface Status callback : it just
 * reset the board when disconnected. TODO: let's DO it better.
 */
void eth_status_callback(nsapi_event_t status, intptr_t param)
{
    if (param != NSAPI_STATUS_GLOBAL_UP && param != NSAPI_STATUS_LOCAL_UP)
        NVIC_SystemReset(); // Just reset the main board and voilà !
}

/* Callbacks to DRV8844 error PINS PinDetect : means OVERCURRENT or OVERTEMP, etc.
 * ---> We send an OSC Message
 */
void driver_A_error_handler()
{
    if (debug_on) debug_OSC("ERROR (temp/voltage/current) on card A. Please reset");
    led_red = 1;
}

void driver_B_error_handler()
{
    if (debug_on) debug_OSC("ERROR (temp/voltage/current) on card B. Please reset");
    led_red = 1;
}

void osc_task(){
    /* Here we realy decode OSC messages -- and we parse addr to menu subfunctions
    */
    while (1) {
        ThisThread::flags_wait_any(0x1);
        while (!socketpacket_buf.empty()) {
            // Test if we are full
            if (socketpacket_buf.full() && debug_on)
                debug_OSC("Circularbuffer full : please slow down the flow !");

            socketpacket* packet;
            socketpacket_buf.pop(packet);

            if (debug_on) debug_OSCmsg(packet->data, packet->size);

            tosc_message osc;
            p_osc = &osc;

            if (tosc_isBundle(packet->data)) {
                // Blink for fun
                led_blue = !led_blue;

                tosc_bundle bundle;
                tosc_parseBundle(&bundle, packet->data, packet->size);

                while (tosc_getNextMessage(&bundle, &osc)) {
                    for (menu_cases* p_case = cases;
                            p_case != cases + sizeof(cases) / sizeof(cases[0]);
                            p_case++) {
                        if (strncmp(tosc_getAddress(&osc), p_case->menu_string,
                                    strlen(p_case->menu_string)) == 0) {
                            (*p_case->menu_func)();
                        }
                    }
                }
            } else if (!tosc_parseMessage(&osc, packet->data, packet->size)) {
                // Blink for fun
                led_blue = !led_blue;

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
    // Delete memory allocation
        delete_array(packet);
        }
    }
}

// unsigned char debug_serial = 0; // debug v1
/* -- debug v2*/
// int debug_midicount = 0;

void on_rx_interrupt() {
    uint8_t c;

    while ((midi_din.readable()) && rx_midi_outbox->midiPacketlenght < MIDI_MAX_MSG_LENGTH) {
        // let's poop
        c = midi_din.getc();

        /* -- debug v1
        debug_serial = c;
        midiTask.flags_set(0x1);*/

        // go elsewhere
        if(c == 254) return ;

        if (packetbox_full != 1) {
            // CASE1 : STATUT BYTE
            if (((c >> 4) & 0xF) == 0x8 || ((c >> 4) & 0xF) == 0x9 || ((c >> 4) & 0xF) == 0xB) {
                // Maybe we'll use it later with Running Status
                rx_midi_Statusbyte = c;

                // Put the Statut byte
                rx_midi_outbox->midiPacket[0] = c;
                rx_midi_outbox->midiPacketlenght = 1;
            // CASE2 : NoteON or NoteOFF, 3 bytes packets in every cases
            } else if (rx_midi_outbox->midiPacketlenght == 2){
                rx_midi_outbox->midiPacket[2] = c;
                rx_midi_outbox->midiPacketlenght = 3;

                // send the packet to mailbox
                rx_midiPacket_box.put(rx_midi_outbox);
/*                debug_midicount ++; */ 
                // create the next packet
                rx_midi_outbox = rx_midiPacket_box.alloc();
                if (rx_midi_outbox != NULL) {
                    rx_midi_outbox->midiPacketlenght = 0;
                } else {
                    packetbox_full = 1;
                }
                // The Thread needs to wake up !
                midiTask.flags_set(0x1);
            // CASE3 : AllNoteOff
            } else if (rx_midi_outbox->midiPacketlenght == 1 && ((c & 0x7F) == 123)){
                rx_midi_outbox->midiPacket[1] = c;
                rx_midi_outbox->midiPacketlenght = 2;

                // send the packet to mailbox
/*                debug_midicount ++; */
                rx_midiPacket_box.put(rx_midi_outbox);
                // create the next packet
                rx_midi_outbox = rx_midiPacket_box.alloc();
                if (rx_midi_outbox != NULL) {
                    rx_midi_outbox->midiPacketlenght = 0;
                } else {
                    packetbox_full = 1;
                }
                // The Thread needs to wake up !
                midiTask.flags_set(0x1);
            // CASE4 : Running state in case on non Statut byte (we check)
            } else if (rx_midi_outbox->midiPacketlenght == 0 && ((c >> 7) & 0x1) != 1){
                // First, write the previous Status byte
                rx_midi_outbox->midiPacket[0] = rx_midi_Statusbyte;
                // Then write the actual byte
                rx_midi_outbox->midiPacket[1] = c;
                rx_midi_outbox->midiPacketlenght = 2;
            } else if (rx_midi_outbox->midiPacketlenght > 0){
                // First, write the previous Status byte
                rx_midi_outbox->midiPacket[rx_midi_outbox->midiPacketlenght] = c;
                rx_midi_outbox->midiPacketlenght++;
            }
        }
    }
}

void midi_task() {
    midi_din.baud(31250);

    // create the first packet
    rx_midi_outbox = rx_midiPacket_box.alloc();
    if (rx_midi_outbox != NULL) {
        rx_midi_outbox->midiPacketlenght = 0;
    } else {
        packetbox_full = 1;
    }

    // Register a callback to process a Rx (receive) interrupt.
    midi_din.attach(&on_rx_interrupt, SerialBase::RxIrq);

    while (1) {
        ThisThread::flags_wait_any(0x1);

        /* -- debug v1
        char buffer[MAX_PQT_SENDLENGTH];
        tohex(&debug_serial, 1, &buffer[0], 3);
        if (debug_on) debug_OSC(buffer);*/

        osEvent midi_evt = rx_midiPacket_box.get(0);
        if (packetbox_full == 1){
            if (debug_on) debug_OSC("Mailbox full : please slow down the flow !");
            rx_midi_outbox = rx_midiPacket_box.alloc();
            if (rx_midi_outbox != NULL) {
                rx_midi_outbox->midiPacketlenght = 0;
                packetbox_full = 0;
            } else {
                packetbox_full = 1;
            }
        }

        if (midi_evt.status == osEventMail) {
            midiPacket_t *midi_inbox = (midiPacket_t *)midi_evt.value.p;

            MIDIMessage MIDIMsg;
            MIDIMsg.from_raw(midi_inbox->midiPacket, midi_inbox->midiPacketlenght);

            if (MIDIMsg.channel() == MIDI_CHANNEL_A - 1) {
                switch (MIDIMsg.type()) {
                    case MIDIMessage::NoteOffType:
                        menu_main_midi_noteOff_chA(MIDIMsg.key());
                        break;
                    case MIDIMessage::NoteOnType:
                        if (MIDIMsg.length > 3) {
                            if (MIDIMsg.velocity() > 0) {
                                menu_main_midi_noteOn_chA(MIDIMsg.key(), MIDIMsg.velocity());
                            } else {
                                menu_main_midi_noteOff_chA(MIDIMsg.key());
                            }
                        } else {
                            menu_main_midi_noteOn_chA_min(MIDIMsg.key());
                        }
                        break;
                    case MIDIMessage::AllNotesOffType:
                        menu_main_midi_allnoteOff();
                        break;
                    case MIDIMessage::ResetAllControllersType:
                        menu_tools_softreset();
                        break;
                    case MIDIMessage::ControlChangeType:
                        break;
                    case MIDIMessage::PitchWheelType:
                        break;
                    case MIDIMessage::SysExType:
                        break;
                    case MIDIMessage::ErrorType:
                        led_red = !led_red;
                        break;
                    default:
                        break;
                }
            }
#if OSC_BOARD == 1
              else if (MIDIMsg.channel() == MIDI_CHANNEL_B - 1) {
                switch (MIDIMsg.type()) {
                    case MIDIMessage::NoteOffType:
                        menu_main_midi_noteOff_chB(MIDIMsg.key());
                        break;
                    case MIDIMessage::NoteOnType:
                        if (MIDIMsg.length > 3) {
                            if (MIDIMsg.velocity() > 0) {
                                menu_main_midi_noteOn_chB(MIDIMsg.key(), MIDIMsg.velocity());
                            } else {
                                menu_main_midi_noteOff_chB(MIDIMsg.key());
                            }
                        } else {
                            menu_main_midi_noteOn_chB_min(MIDIMsg.key());
                        }
                        break;
                    case MIDIMessage::AllNotesOffType:
                        menu_main_midi_allnoteOff();
                        break;
                    case MIDIMessage::ResetAllControllersType:
                        menu_tools_softreset();
                        break;
                    case MIDIMessage::ControlChangeType:
                        break;
                    case MIDIMessage::PitchWheelType:
                        break;
                    case MIDIMessage::SysExType:
                        break;
                    case MIDIMessage::ErrorType:
                        break;
                    default:
                        break;
                }
            }
#endif
            rx_midiPacket_box.free(midi_inbox);
            packetbox_full = 0;
        }
    }
}

/* MAIN function
 */
int main()
{
    // Init master_address IP client to broadcast for convenience
    master_address = (char *)malloc(16 * sizeof(char));
    sprintf(master_address, "255.255.255.255");

    // Init homemade CoilDriver class
    driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                              DRV_A_FAULT, driver_a_table, i2c_err_callback, A_SIDE_I2C_TAG);
    // Set-up driver_A error feedbacks with PinDetect
    driver_A->drv_fault.attach_asserted_held(queue_msg.event(driver_A_error_handler));
    driver_A->drv_fault.setSamplesTillHeld(20);
    driver_A->drv_fault.setAssertValue(0);
    driver_A->drv_fault.setSampleFrequency();
#if B_SIDE == 1
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, i2c_err_callback, B_SIDE_I2C_TAG);
    driver_B->drv_fault.attach_asserted_held(queue_msg.event(driver_B_error_handler));
    driver_B->drv_fault.setSamplesTillHeld(20);
    driver_B->drv_fault.setAssertValue(0);
    driver_B->drv_fault.setSampleFrequency();
#endif

    // Set-up button
    button.fall(&button_released);
    button.rise(&button_pressed);

    // Launch MIDI stuf
    midiTask.start(midi_task);
    midiTask.set_priority(osPriorityAboveNormal2);

    // Set-up EthernetInterface in DHCP mode
    nsapi_error_t status;
    eth = new EthernetInterface;
    eth->set_blocking(false);
    eth->connect();
    ThisThread::sleep_for(1000);

    while (eth->get_connection_status() != NSAPI_STATUS_GLOBAL_UP) {
        status = eth->connect();
        led_red = 1;
        ThisThread::sleep_for(1000);
    }

    //OK
    led_red = 0;

    // Attach to status callback
    eth->attach(&eth_status_callback);

    // Set-up UDPSocket udp_socket because OSC IS in UDP.
    udp_socket = new UDPSocket;
    udp_socket->set_blocking(false);
    status = -1;
    status = udp_socket->open(eth);

    if (status != NSAPI_ERROR_OK) {
        led_red = 1;
    }
    // Bind UDPSocket to the OSC_CLIENT_PORT.
    while(udp_socket->bind(OSC_CLIENT_PORT) != 0);

    // Callback ANY packet to handle_socket() -- here is the main magic function.
    udp_socket->sigio(callback(handle_udp_socket));

    // Set-up SocketAddresses
    ip = new SocketAddress;
    client_addr = new SocketAddress;

    /* At this step we are "On the Air", so we can dend up a welcome message to
    * broadcast. We can communicate in both sides with BROADCAST address, but it's
    * important to connect each others with more intimity, because :
    * more intimity == more efficiency
    */
    eth->get_ip_address(ip);

    init_msgON();

    // Dispatch forever the queue in a thread :
    thrd_io.start(callback(&queue_io, &EventQueue::dispatch_forever));
    thrd_msg.start(callback(&queue_msg, &EventQueue::dispatch_forever));
    // Later, we have to find the best priority
    thrd_io.set_priority(osPriorityAboveNormal1);

    // Launch OSC stuf
    oscTask.start(osc_task);
    oscTask.set_priority(osPriorityAboveNormal);

    // (not so) BROKEN tone function : precompute 128 sample points on one sine wave cycle
    // used for continuous sine wave output later
    for(int i=0; i<128; i++) {
        sinusoid_data[i]=((1.0 + sin((float(i)/128.0*6.28318530717959)))/2.0);
    }

    // DEBUG
    // sys_state.report_state();

    while(true) {
        // queue.dispatch_forever();
        // Not even sleep !
        ThisThread::sleep_for(1000);
/* -- debug v2
        char buffer[MAX_PQT_SENDLENGTH];
        sprintf(buffer, "%d midi/sec", debug_midicount);
        debug_OSC(buffer);        
        debug_midicount = 0;*/
    }
}