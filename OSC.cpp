/*
<----------------------------------------------------------------------------------

 OSC message Codec(encoder/decoder)
 
 version: 1.3 (2014/ 8/31) encoder bufix(encoder returns byte length)   
 version: 1.2 (2014/ 8/30) decoder bufix 
 version: 1.1 (2013/11/20) support BIG_ENDIAN by #define
 version: 1.0 (2013/11/10)

 Copyright (C) 2011,2013,2014 S.Komatsu
 released under the MIT License: http://mbed.org/license/mit

 please refer to: http://opensoundcontrol.org/introduction-osc for OSC(Open Sound Control)

  The followings are supported:

    Features: 
     Packet Parsing (Client)
     Packet Construction (Server)
     Bundle NOT Support
     Timetag NOT Support

    Type Support: 
     i: int32
     b: blob
     s: string
     f: float32
     m: MIDI message(port id, status byte, data1, data2) // I don't know the detail
     
  Change Log:
   Bug(string length is not correct in encoding) Fix on 2013/11/10 (v0.9 -> v1.0)

>----------------------------------------------------------------------------------
*/

#include "OSC.h"
//#define BIG_ENDIAN

int lenAlign4B(int len) {
    if ((len % 4) == 0)
        return len;
    else {return len + 4 - (len % 4);}
}


int encOSCmsg(char *packet , union OSCarg *msg){
  // *** important notice ***
  // output buffer must be cleared before call this
  char *p, *s, *d, *typeTag; 
  char c;

  p=packet;
  d=p;
  s=msg[0].address; // address
  for(int i=0; i<strlen(msg[0].address); i++) *d++ = *s++;
  *d=0; // terminator
//  p += 4*((strlen(msg[0].address)+1)/4+1);
  p += lenAlign4B(strlen(msg[0].address)+1);
  // 
  s=msg[1].typeTag;
  d=p;
  for(int i=0; i<strlen(msg[1].typeTag); i++) *d++ = *s++; 
  *d=0; // terminator   
//  p += 4*((strlen(msg[1].s)+1)/4+1);
  p += lenAlign4B(strlen(msg[1].typeTag)+1);
  //
  typeTag=msg[1].s+1; // skip ','
  for(int n=0; n<strlen(typeTag); n++){
    c = typeTag[n];
    if (('s'==c)) {
      s=msg[n+2].s;
      d=p;
      for(int i=0; i<strlen(msg[n+2].s); i++) *d++ = *s++;
      *d=0; // terminater    
//     p += 4*((strlen(msg[n+2].s)+1)/4+1);
      p += lenAlign4B(strlen(msg[n+2].s)+1);
    } 
    else if (('i'==c)||('f'==c)) {
#ifdef BIG_ENDIAN
      // no change endian (big to big)
      p[0]=msg[n+2]._b[0];
      p[1]=msg[n+2]._b[1];
      p[2]=msg[n+2]._b[2];
      p[3]=msg[n+2]._b[3];
#else
      // change endian (little to big)
      p[0]=msg[n+2]._b[3];
      p[1]=msg[n+2]._b[2];
      p[2]=msg[n+2]._b[1];
      p[3]=msg[n+2]._b[0];
#endif
        p +=4;  
    } 
    else if ('b'==c) {
      // put length of blog
#ifdef BIG_ENDIAN
      // no change endian (big to big)
      p[0]=msg[n+2]._b[0];
      p[1]=msg[n+2]._b[1];
      p[2]=msg[n+2]._b[2];
      
      p[3]=msg[n+2]._b[3];
#else
      // change endian (little to big)
      p[0]=msg[n+2]._b[3];
      p[1]=msg[n+2]._b[2];
      p[2]=msg[n+2]._b[1];
      p[3]=msg[n+2]._b[0];
#endif
      p +=4;  
      // get ponter of blob (copy to msg[n].blob.p)
      s=msg[n+2].blob.p;
      d=p;
      for(int i=0; i<msg[n+2].blob.len; i++) *d++ = *s++;    
      p += 4*(msg[n+2].blob.len/4+1);       
    } 
    else if ('m'==c) {
      // get midi data (copy to msg[n].m[])
      p[0]=msg[n+2].m[0]; 
      p[1]=msg[n+2].m[1]; 
      p[2]=msg[n+2].m[2];
      p[3]=msg[n+2].m[3];
      p +=4;
    } 
    else {
      //printf("*** Not Supported TypeTag:%s ****\n",typeTag);
    }
  };
  //return (p-packet); // return packet size
  // bugfix 2014/8/31
  return sizeof(char)*(p-packet);  // return byte length   
};
    
void decOSCmsg(char *packet , union OSCarg *msg){
  // Caution: the returned result points to packet as blobs or strings (not newly allocatd)
  char *p, *typeTag; 
  char c; int n;

  msg[0].address = packet; // address
  msg[1].typeTag = packet + 4 * ((strlen(msg[0].s) + 1) / 4 + 1);//typeTag
  typeTag=msg[1].s + 1; // skip ','

  // bugfix 2014/8/30
  // TODO: BugEDfix ?
/*  if (strlen(typeTag) % 2 == 0)
    p = msg[1].s + 4 * ((strlen(msg[1].s) + 1) / 4 ); 
  else*/
    p = msg[1].s + 4 * ((strlen(msg[1].s) + 1) / 4 + 1);

  for(n = 0; n < strlen(typeTag); n++){
    c = typeTag[n];
    if (('s'==c)) {
      msg[n+2].s=p;
      //p += 4*((strlen(msg[n+2].s)+1)/4+1);
      p += lenAlign4B(strlen(msg[n+2].s)+1);
    } 
    else if (('i'==c)||('f'==c)) {
#ifdef BIG_ENDIAN
      // no change endian (big to big)
      msg[n+2]._b[0]=p[0];
      msg[n+2]._b[1]=p[1];
      msg[n+2]._b[2]=p[2];
      msg[n+2]._b[3]=p[3];
#else
      // change endian (big to little)
      msg[n+2]._b[3]=p[0];
      msg[n+2]._b[2]=p[1];
      msg[n+2]._b[1]=p[2];
      msg[n+2]._b[0]=p[3];
#endif
      p += 4;
    } 
    else if ('b'==c) {
      // get lenth of blog (copy to msg[n].blog.len)
#ifdef BIG_ENDIAN
      // no change endian (big to big)
      msg[n+2]._b[0]=p[0];
      msg[n+2]._b[1]=p[1];
      msg[n+2]._b[2]=p[2];
      msg[n+2]._b[3]=p[3];
#else
      // change endian (big to little)
      msg[n+2]._b[3]=p[0];
      msg[n+2]._b[2]=p[1];
      msg[n+2]._b[1]=p[2];
      msg[n+2]._b[0]=p[3];
#endif
      p +=4;
      // get pointer of blob (copy to msg[n].blob.p)
      msg[n+2].blob.p=p;
      //p += 4*(msg[n+2].blob.len/4+1);       
      p += lenAlign4B(msg[n+2].blob.len+1);
    } 
    else if ('m'==c) {
      // get midi data (copy to msg[n].m[])
      msg[n+2].m[0]=p[0]; 
      msg[n+2].m[1]=p[1]; 
      msg[n+2].m[2]=p[2];
      msg[n+2].m[3]=p[3];
      p +=4;  
    } 
    else {
      //printf("*** Not Supported TypeTag:%s ****\n",typeTag);
    }
 };
};