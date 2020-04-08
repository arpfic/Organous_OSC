/** Abstract class for 'constant-current (CC)' LED driver component
 *
 *  Abstract class for CC-LED driver family
 *  No instance can be made from this class
 *
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.6
 *  @date    19-Mar-2015
 *
 *  Released under the Apache 2 license License
 */

#ifndef     MBED_CompLedDvrCC
#define     MBED_CompLedDvrCC

#include    "mbed.h"

typedef enum {
    /** Pin names of LED driver. Those are L0 .. L3, not like "LED0" to avoid mbed board LED names  */
    L0,            /**< LED0  pin                              */
    L1,            /**< LED1  pin                              */
    L2,            /**< LED2  pin                              */
    L3,            /**< LED3  pin                              */
    L4,            /**< LED4  pin                              */
    L5,            /**< LED5  pin                              */
    L6,            /**< LED6  pin                              */
    L7,            /**< LED7  pin                              */
    L8,            /**< LED8  pin                              */
    L9,            /**< LED9  pin                              */
    L10,           /**< LED10 pin                              */
    L11,           /**< LED11 pin                              */
    L12,           /**< LED12 pin                              */
    L13,           /**< LED13 pin                              */
    L14,           /**< LED14 pin                              */
    L15,           /**< LED15 pin                              */
    L16,           /**< LED16 pin                              */
    L17,           /**< LED17 pin                              */
    L18,           /**< LED18 pin                              */
    L19,           /**< LED19 pin                              */
    L20,           /**< LED20 pin                              */
    L21,           /**< LED21 pin                              */
    L22,           /**< LED22 pin                              */
    L23,           /**< LED23 pin                              */
    L_NC = ~0x0L   /**< for when the pin is left no-connection */
} LedPinName;


/** Abstract class for CC-LED driver component
 *
 *  @class CompLedDvrCC
 *
 *  Abstract class for LED driver family
 *  No instance can be made from this class
 */
class CompLedDvrCC
{
public:
    /** Default constructor */
    CompLedDvrCC();

    /** Destructor */
    virtual ~CompLedDvrCC();

    /** Virtual function to define standard function of the component   */
    virtual void    pwm( int port, float v )    = 0;

    /** Virtual function to define standard function of the component   */
    virtual void    current( int port, float v )    = 0;
}
;

#endif  //  MBED_CompLedDvrCC


