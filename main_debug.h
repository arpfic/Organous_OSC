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
#ifndef _MAIN_DEBUG_
#define _MAIN_DEBUG_

#include "mbed.h"
#include "config.h"
#include "tOSC.h"

void tohex(unsigned char * in, size_t insz, char * out, size_t outsz);
int debug_OSCwritemsg(char* incoming_msg, int length, char* outgoing_msg);
int debug_OSCwrite(char* incoming_msg, char* outgoing_msg);
static void debug_OSC(const char* incoming_msg);
static void debug_OSCmsg(char* incoming_msg, int in_length);

/* see https://stackoverflow.com/questions/6357031/how-do-you-convert-a-byte-array-to-a-hexadecimal-string-in-c
 * outsz = 3x insz
 */
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

/* Some helpers designed to debug over OSC
 * debug_OSCwritemsg() format and send the buffer to debug_OSCwrite()
 * Please note that tosc_parseMessage() checks the buffer length.
 */
int debug_OSCwritemsg(char* incoming_msg, int in_length, char* outgoing_msg)
{
    tosc_message osc; // declare the TinyOSC structure
    char buffer[MAX_PQT_LENGTH];
    int out_length = 0;

    if (!tosc_parseMessage(&osc, incoming_msg, in_length)) {
        sprintf(buffer, "[OSC msg (%ib)]: %s %s ",
                in_length, // the number of bytes in the OSC message
                tosc_getAddress(&osc), // the OSC address string, e.g. "/button1"
                tosc_getFormat(&osc)); // the OSC format string, e.g. "f"
        for (int i = 0; osc.format[i] != '\0'; i++) {
            switch (osc.format[i]) {
                case 'f':
                    sprintf(buffer + strlen(buffer), "%g ", tosc_getNextFloat(&osc));
                    break;
                case 'i':
                    sprintf(buffer + strlen(buffer), "%i ", tosc_getNextInt32(&osc));
                    break;
                // returns NULL if the incoming_msg in_length is exceeded
                case 's':
                    sprintf(buffer + strlen(buffer), "%s ", tosc_getNextString(&osc));
                    break;
                default:
                    continue;
            }
        }
        out_length = debug_OSCwrite(buffer, outgoing_msg);
    }
    return out_length;
}

int debug_OSCwrite(char* incoming_msg, char* outgoing_msg)
{
    // write the OSC packet to the buffer
    // returns the number of bytes written to the buffer, negative on error
    // note that tosc_write will clear the entire buffer before writing to it
    int len = tosc_writeMessage(
                  outgoing_msg, MAX_PQT_LENGTH,
                  "/debug", // the address
                  "ss",   // the format; 'f':32-bit float, 's':ascii string, 'i':32-bit integer
                  IF_NAME" ("SOFT_VER"):", incoming_msg);

    return len;
}

static void debug_OSCmsg(char* incoming_msg, int in_length)
{
    char buffer[MAX_PQT_LENGTH];
    int out_length = 0;

    if (in_length <= MAX_PQT_LENGTH) {
        out_length = debug_OSCwritemsg(incoming_msg, in_length, buffer);
        send_UDPmsg(buffer, out_length);
    }
}

static void debug_OSC(const char* incoming_msg)
{
    char buffer[MAX_PQT_LENGTH];
    int out_length = 0;

    if (strlen(incoming_msg) < MAX_PQT_LENGTH) {
        out_length = debug_OSCwrite((char *)incoming_msg, buffer);
        send_UDPmsg(buffer, out_length);
    }
}

#endif // _MAIN_DEBUG_
