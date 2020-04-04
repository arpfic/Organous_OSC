// OSC message Codec Header
// 2013/10/28
#ifndef _OSCMSGCODEC_H
#define _OSCMSGCODEC_H

#include <string.h>

int encOSCmsg(char *packet , union OSCarg *msg);
// makes packet from OSC message and returns packet size

void decOSCmsg(char *packet , union OSCarg *msg); 
// makes OSC message from packet

union OSCarg {
  // char*, int and float are assumed four bytes
  char *address;
  char *typeTag;
  long int i; // int32 for Arduino(16bits)
  float f;
  char *s;
  struct {
    long int len; // is "int i"
    char *p;
  } blob;
  char m[4];  // for MIDI
  char _b[4]; // endian conversion temp variable
};

#endif // _OSCMSGCODEC_H