/** LedPwmOutCC class for LED driver component
 *
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.5
 *  @date    04-Mar-2015
 *
 *  Released under the Apache 2 license
 */

#ifndef     MBED_LedPwmOutCC
#define     MBED_LedPwmOutCC

#include    "mbed.h"
#include    "CompLedDvrCC.h"

/** LedPwmOutCC class
 *
 *  @class LedPwmOutCC
 *
 *  "LedPwmOutCC" class works like "PwmOut" class of mbed-SDK. 
 *  This class provides API on device's pin level with abstracting the LED controller. 
 *
 *  Example:
 *  @code
 *  #include "mbed.h"
 *  #include "PCA9956A.h"
 *  
 *  PCA9956A    led_cntlr( p28, p27, 0xC4 );  //  SDA, SCL, Slave_address(option)
 *  LedPwmOutCC led( led_cntlr, L0 );
 *  
 *  int main()
 *  {
 *      while( 1 ) {
 *          for( float p = 0.0f; p < 1.0f; p += 0.1f ) {
 *              led     = p;
 *              wait( 0.1 );
 *          }
 *      }
 *  }
 *  @endcode
 */
class LedPwmOutCC
{
public:

    /** Create a LedPwmOutCC instance connected to a pin on the LED driver
     *  A pin which performs PWM and constant current sink
     *
     * @param ledp          Instance of a device (LED driver)
     * @param pin_name      Specifying pin by LedPinName like 'L7'. 
     *
     * @note
     *   Pin names of LED driver are defined like L0, L1, L2.. It is not like "LED0". 
     *   Because we cannot use mbed reserved symbols. 
     */
    LedPwmOutCC( CompLedDvrCC &ledp, LedPinName pin_name );

    /** Destractor
     */
    virtual ~LedPwmOutCC();

    /** Set PWM duty-cycle
     *
     * @param v             Ratio of duty-cycle. '0.0' for 0 %. '1.0' for 99.6 % on PCA9956A and 100 % for PCA9955A.
     */
    virtual void    pwm( float v );

    /** Set output current
     *
     * @param v             Ratio of output current. 1.0 for 100 % output of hardware setting
     */
    virtual void    current( float v );
    
    /** A shorthand for pwm()
     */
    LedPwmOutCC&      operator= ( float rhs );

private:
    CompLedDvrCC    *leddvrp;
    LedPinName      pin;

    void    pwm( int pin, float value );
}
;

#endif  //  MBED_LedPwmOutCC
