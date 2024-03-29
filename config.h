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

/* -----------------------------------------------------------------------------
 * MAIN CONFIG : Here we can set the main properties
 *
 * BOARD :
 * 1 : suraig
 * 2 : respi
 */
#define OSC_BOARD                                   1
#define SOFT_VER                                    "v0.3"

#if OSC_BOARD == 1
    #define IF_NAME                                 "SURAIGU board"
    #define IF_OSC_NAME                             "suraig"

    // Is the B_SIDE card present or only A_SIDE ?
    #define B_SIDE                                  1
    #define A_SIDE_OUTS                             24
    #define B_SIDE_OUTS                             18
    // Midi compatibility, OFFSET, i.e. from wich octave we begin
    #define IF_BASENOTE                             79

    /* -----------------------------------------------------------------------------
    * I2C address of PCA9956B
    * (Please refer to https://www.nxp.com/docs/en/data-sheet/PCA9956B.pdf)
    */
    #define A_SIDE_I2C_TAG                         0xD2 // Vdd-GND-Vdd
    #define B_SIDE_I2C_TAG                         0xD2 // GND-Vdd-GND
    #define DEFAULT_I2C_TAG                        0xD2 // GND-Vdd-GND

    /* -----------------------------------------------------------------------------
    * DRV8844 DRIVERS ENABLE TABLES (see the datasheet and Hardware/)
    */
    #define ENABLE_PINS                             24
    #define MAX_OUT_ENABLED                         OUT_ENABLED4
    // If > 4, need to rewrite code associated to OutRegister
    #define OUT_MAXUSERS                            1
    #define OFF                                     255
    #define ON                                      0

    /* -----------------------------------------------------------------------------
    * COIL DEFAULTS SET'S
    * - ATTACK_DELAY before sustain (millisec)
    * - ATTACK PWM value  (0-255)
    * - SUSTAIN PWM value (0-255)
    */
    #define COIL_ATTACK_DELAY                       20
    #define COIL_ATTACK                             255
    //#define COIL_SUSTAIN                            (uint8_t)153
    #define COIL_SUSTAIN                            (uint8_t)230


    /* ---------------------------------------------
    * A_SIDE (right) : OUT 1-24
    * We only have to change this with hardware changes.
    */
    #define DRV_EN1                                 PC_10
    #define DRV_EN2                                 PC_12
    #define DRV_EN3                                 PD_2
    #define DRV_EN4                                 PF_6
    #define DRV_EN5                                 PF_7
    #define DRV_EN6                                 PA_15
    #define DRV_EN7                                 PA_0
    #define DRV_EN8                                 PE_4
    #define DRV_EN9                                 PC_2
    #define DRV_EN10                                PC_0
    #define DRV_EN11                                PC_3
    #define DRV_EN12                                PD_3
    #define DRV_EN13                                PD_4
    #define DRV_EN14                                PG_3
    #define DRV_EN15                                PD_5
    #define DRV_EN16                                PD_6
    #define DRV_EN17                                PE_3
    #define DRV_EN18                                PF_2
    #define DRV_EN19                                PG_1
    #define DRV_EN20                                PD_0
    #define DRV_EN21                                PG_0
    #define DRV_EN22                                PE_1
    #define DRV_EN23                                PG_15
    #define DRV_EN24                                PG_9

    #define PCA_A_SCL                               PF_1
    #define PCA_A_SDA                               PF_0
    #define PCA_A_OE                                PE_5
    #define PCA_A_RST                               PD_1

    #define DRV_A_RST                               PG_12
    #define DRV_A_FAULT                             PC_11

    /* ---------------------------------------------
    * B_SIDE (left) : OUT 25-48 
    */
    #define DRV_EN25                                PC_8
    #define DRV_EN26                                PC_6
    #define DRV_EN27                                PB_8
    #define DRV_EN28                                PB_9
    #define DRV_EN29                                PA_5
    #define DRV_EN30                                PB_12
    #define DRV_EN31                                PB_11
    #define DRV_EN32                                PB_6
    #define DRV_EN33                                PC_7
    #define DRV_EN34                                PB_2
    #define DRV_EN35                                PB_1
    #define DRV_EN36                                PB_10
    #define DRV_EN37                                PD_15
    #define DRV_EN38                                PE_15
    #define DRV_EN39                                PE_13
    #define DRV_EN40                                PE_11
    #define DRV_EN41                                PF_13
    #define DRV_EN42                                PF_3
    #define DRV_EN43                                PF_12
    #define DRV_EN44                                PG_14
    #define DRV_EN45                                PF_11
    #define DRV_EN46                                PE_0
    #define DRV_EN47                                PD_10
    #define DRV_EN48                                PG_8

    #define PCA_B_SCL                               PF_14
    #define PCA_B_SDA                               PF_15
    #define PCA_B_OE                                PD_14
    #define PCA_B_RST                               PE_9

    #define DRV_B_RST                               PG_5
    #define DRV_B_FAULT                             PC_9

    #define MIDI_CHANNEL_A                          16
    #define MIDI_CHANNEL_B                          8
    // Number of notes on the first channel ; KISS, the second channel start here
    #define MIDI_CHANNEL_A_SIZE                     11
    #define MIDI_CHANNEL_B_OFFSET                   0

#elif OSC_BOARD == 2
    // NOTE: On the RESPIRATOR, side A is a B card
    #define IF_NAME                                 "RESPIRATOR board"
    #define IF_OSC_NAME                             "respi"

    // Is the B_SIDE card present or only A_SIDE ?
    #define B_SIDE                                  0
    #define A_SIDE_OUTS                             24
    // Midi compatibility, OFFSET, i.e. from wich octave we begin
    #define IF_BASENOTE                             67

    /* -----------------------------------------------------------------------------
    * I2C address of PCA9956B
    * (Please refer to https://www.nxp.com/docs/en/data-sheet/PCA9956B.pdf)
    */
    #define A_SIDE_I2C_TAG                         0xD2 // Vdd-GND-Vdd
    #define DEFAULT_I2C_TAG                        0xD2 // GND-Vdd-GND

    /* -----------------------------------------------------------------------------
    * DRV8844 DRIVERS ENABLE TABLES (see the datasheet and Hardware/)
    */
    #define ENABLE_PINS                             24
    #define MAX_OUT_ENABLED                         OUT_ENABLED4
    // If > 4, need to rewrite code associated to OutRegister
    #define OUT_MAXUSERS                            4
    #define OFF                                     255
    #define ON                                      0

    /* -----------------------------------------------------------------------------
    * COIL DEFAULTS SET'S
    * - ATTACK_DELAY before sustain (millisec)
    * - ATTACK PWM value  (0-255)
    * - SUSTAIN PWM value (0-255)
    */
    #define COIL_ATTACK_DELAY                       20
    #define COIL_ATTACK                             255
    //#define COIL_SUSTAIN                            (uint8_t)153
    #define COIL_SUSTAIN                            (uint8_t)230

    /* ---------------------------------------------
    * A_SIDE (left) : OUT 1-24
    * We only have to change this with hardware changes.
    */
    #define DRV_EN1                                 PC_8
    #define DRV_EN2                                 PC_6
    #define DRV_EN3                                 PB_8
    #define DRV_EN4                                 PB_9
    #define DRV_EN5                                 PA_5
    #define DRV_EN6                                 PB_12
    #define DRV_EN7                                 PB_11
    #define DRV_EN8                                 PB_6
    #define DRV_EN9                                 PC_7
    #define DRV_EN10                                PB_2
    #define DRV_EN11                                PB_1
    #define DRV_EN12                                PB_10
    #define DRV_EN13                                PD_15
    #define DRV_EN14                                PE_15
    #define DRV_EN15                                PE_13
    #define DRV_EN16                                PE_11
    #define DRV_EN17                                PF_13
    #define DRV_EN18                                PF_3
    #define DRV_EN19                                PF_12
    #define DRV_EN20                                PG_14
    #define DRV_EN21                                PF_11
    #define DRV_EN22                                PE_0
    #define DRV_EN23                                PD_10
    #define DRV_EN24                                PG_8

    #define PCA_A_SCL                               PF_14
    #define PCA_A_SDA                               PF_15
    #define PCA_A_OE                                PD_14
    #define PCA_A_RST                               PE_9

    #define DRV_A_RST                               PG_5
    #define DRV_A_FAULT                             PC_9

    #define MIDI_CHANNEL_A                          1
    #define MIDI_CHANNEL_B                          NULL
    #define MIDI_CHANNEL_A_SIZE                     24
    #define MIDI_CHANNEL_B_OFFSET                   0

#endif

/* -----------------------------------------------------------------------------
 * MIDI stuf
 */
#define MIDI_UART_TX                            PB_4
#define MIDI_UART_RX                            PE_7
#define MIDIMAIL_SIZE                           64
#define MIDI_MAX_MSG_LENGTH                     4 // Max message size. SysEx can be up to 65536.

/* -----------------------------------------------------------------------------
 * NUCLEO_F767ZI LEDS
 */
#define LED_GREEN                               LED1
#define LED_BLUE                                LED2
#define LED_RED                                 LED3

/* -----------------------------------------------------------------------------
 * OSC and BUFFER STUFF :
 * OSC_CLIENT_PORT      : the port of the client (e.g. puredata)
 * MAX_PQT_LENGTH       : MAXIMUM UDP packet length
 * MAX_PQT_SENDLENGTH   : MAXIMUM UDP send packet length
 * PACKETS_TABLE_SIZE   : INTERNAL RX TABLE size
 * QUEUE_EVENTS         : SIZE of the incoming queue
 */
#define OSC_CLIENT_PORT                         9000
#define MAX_PQT_LENGTH                          4096
#define MAX_PQT_SENDLENGTH                      512
#define PACKETS_TABLE_SIZE                      32
#define QUEUE_MSG_EVENTS                        32
#define QUEUE_IO_EVENTS                         1024