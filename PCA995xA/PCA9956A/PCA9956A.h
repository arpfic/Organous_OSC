/** PCA9956A constant current LED driver
 *
 *  An operation sample of PCA9956A 24-channel Fm+ I2C-bus 57mA/20V constant current LED driver.
 *  mbed accesses the PCA9956A registers through I2C.
 *
 *  @class   PCA9956A
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.6
 *  @date    19-Mar-2015
 *
 *  Released under the Apache 2 license
 *
 *  About PCA9956A:
 *    http://www.nxp.com/products/interface_and_connectivity/i2c/i2c_led_display_control/PCA9956ATW.html
 */

#ifndef     MBED_PCA9956A
#define     MBED_PCA9956A

#include    "mbed.h"
#include    "PCA995xA.h"
#include    "LedPwmOutCC.h"

/** PCA9956A class
 *
 *  This is a driver code for the PCA9956A 24-channel Fm+ I2C-bus 57mA/20V constant current LED driver.
 *  This class provides interface for PCA9956A operation and accessing its registers.
 *  Detail information is available on next URL.
 *    http://www.nxp.com/products/interface_and_connectivity/i2c/i2c_led_display_control/PCA9956ATW.html
 *
 *  Next sample code shows operation based on low-level-API (operated by just device instane)
 *
 *  Example:
 *  @code
 *  //  PCA9956A operation sample using its device instance
 *  
 *  #include "mbed.h"
 *  #include "PCA9956A.h"
 *  
 *  PCA9956A    led_cntlr( p28, p27, 0x02 );    //  SDA, SCL, Slave_address(option)
 *  
 *  int main()
 *  {
 *      led_cntlr.current( ALLPORTS, 1.0 ); //  Set all ports output current 100%
 *      
 *      while(1) {
 *          for ( int port = 0; port < led_cntlr.number_of_ports(); port++ ) {
 *              for ( int i = 1; i <= 100; i++ ) {
 *                  led_cntlr.pwm(  port, (float)i / 100.0 );
 *                  wait( 0.01 );
 *              }
 *          }
 *          led_cntlr.pwm( ALLPORTS, 0.0 );
 *      }
 *  }
 *  @endcode
 *  
 *  The high-level-API:LedPwmOutCC is also available.
 *  It can be used like next sample code.
 *  
 *  @code
 *  //  PCA9956A operation sample using high-level-API
 *  
 *  #include "mbed.h"
 *  #include "PCA9956A.h"
 *  
 *  PCA9956A    led_cntlr( p28, p27, 0x02 );    //  SDA, SCL, Slave_address(option)
 *  LedPwmOutCC led0( led_cntlr, L0  );         //  Instance for LED0 pin
 *  LedPwmOutCC led1( led_cntlr, L1  );         //  Instance for LED1 pin
 *  LedPwmOutCC led2( led_cntlr, L2  );         //  Instance for LED2 pin
 *  
 *  int main()
 *  {
 *      led0.current( 0.5 );    //  LED0 pin current output setting to 50%
 *      led1.current( 0.5 );    //  LED1 pin current output setting to 50%
 *      led2.current( 0.5 );    //  LED2 pin current output setting to 50%
 *  
 *      while(1) {
 *          
 *          for ( float p = 1.0; p >= 0.0; p -= 0.01 ) {
 *              led0    = p;    //  Set LED0 output PWM dutycycle as 'p'
 *              wait( 0.01 );
 *          }
 *          
 *          for ( float p = 1.0; p >= 0.0; p -= 0.01 ) {
 *              led1    = p;    //  Set LED1 output PWM dutycycle as 'p'
 *              wait( 0.01 );
 *          }
 *          
 *          for ( float p = 1.0; p >= 0.0; p -= 0.01 ) {
 *              led2    = p;    //  Set LED2 output PWM dutycycle as 'p'
 *              wait( 0.01 );
 *          }
 *      }
 *  }
 *  @endcode
 */
class PCA9956A : public PCA995xA
{
public:

#if DOXYGEN_ONLY
    /** PCA9956A pin names high-level API i.e. LedPwmOutCC */
    typedef enum {
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
#endif // DOXYGEN_ONLY

    /** Name of the PCA9956A registers (for direct register access) */
    enum command_reg {
        MODE1,          /**< MODE1 register      */
        MODE2,          /**< MODE2 register      */
        LEDOUT0,        /**< LEDOUT0 register    */
        LEDOUT1,        /**< LEDOUT1 register    */
        LEDOUT2,        /**< LEDOUT2 register    */
        LEDOUT3,        /**< LEDOUT3 register    */
        LEDOUT4,        /**< LEDOUT4 register    */
        LEDOUT5,        /**< LEDOUT5 register    */
        GRPPWM,         /**< GRPPWM register     */
        GRPFREQ,        /**< GRPFREQ register    */
        PWM0,           /**< PWM0 register       */
        PWM1,           /**< PWM1 register       */
        PWM2,           /**< PWM2 register       */
        PWM3,           /**< PWM3 register       */
        PWM4,           /**< PWM4 register       */
        PWM5,           /**< PWM5 register       */
        PWM6,           /**< PWM6 register       */
        PWM7,           /**< PWM7 register       */
        PWM8,           /**< PWM8 register       */
        PWM9,           /**< PWM9 register       */
        PWM10,          /**< PWM10 register      */
        PWM11,          /**< PWM11 register      */
        PWM12,          /**< PWM12 register      */
        PWM13,          /**< PWM13 register      */
        PWM14,          /**< PWM14 register      */
        PWM15,          /**< PWM15 register      */
        PWM16,          /**< PWM16 register      */
        PWM17,          /**< PWM17 register      */
        PWM18,          /**< PWM18 register      */
        PWM19,          /**< PWM19 register      */
        PWM20,          /**< PWM20 register      */
        PWM21,          /**< PWM21 register      */
        PWM22,          /**< PWM22 register      */
        PWM23,          /**< PWM23 register      */
        IREF0,          /**< IREF0 register      */
        IREF1,          /**< IREF1 register      */
        IREF2,          /**< IREF2 register      */
        IREF3,          /**< IREF3 register      */
        IREF4,          /**< IREF4 register      */
        IREF5,          /**< IREF5 register      */
        IREF6,          /**< IREF6 register      */
        IREF7,          /**< IREF7 register      */
        IREF8,          /**< IREF8 register      */
        IREF9,          /**< IREF9 register      */
        IREF10,         /**< IREF10 register     */
        IREF11,         /**< IREF11 register     */
        IREF12,         /**< IREF12 register     */
        IREF13,         /**< IREF13 register     */
        IREF14,         /**< IREF14 register     */
        IREF15,         /**< IREF15 register     */
        IREF16,         /**< IREF16 register     */
        IREF17,         /**< IREF17 register     */
        IREF18,         /**< IREF18 register     */
        IREF19,         /**< IREF19 register     */
        IREF20,         /**< IREF20 register     */
        IREF21,         /**< IREF21 register     */
        IREF22,         /**< IREF22 register     */
        IREF23,         /**< IREF23 register     */
        OFFSET  = 0x3A, /**< OFFSET register     */
        SUBADR1,        /**< SUBADR1 register    */
        SUBADR2,        /**< SUBADR2 register    */
        SUBADR3,        /**< SUBADR3 register    */
        ALLCALLADR,     /**< ALLCALLADR register */
        PWMALL,         /**< PWMALL register     */
        IREFALL,        /**< IREFALL register    */
        EFLAG0,         /**< EFLAG0 register     */
        EFLAG1,         /**< EFLAG1 register     */
        EFLAG2,         /**< EFLAG2 register     */
        EFLAG3,         /**< EFLAG3 register     */
        EFLAG4,         /**< EFLAG4 register     */
        EFLAG5,         /**< EFLAG5 register     */

        REGISTER_START          = MODE1,
        LEDOUT_REGISTER_START   = LEDOUT0,
        PWM_REGISTER_START      = PWM0,
        IREF_REGISTER_START     = IREF0,
    };

    /** Create a PCA9956A instance connected to specified I2C pins with specified address
     *
     * @param i2c_sda       I2C-bus SDA pin
     * @param i2c_sda       I2C-bus SCL pin
     * @param i2c_address   I2C-bus address (default: 0xC0)
     */
    PCA9956A( PinName i2c_sda, PinName i2c_scl, char i2c_address = PCA995xA::DEFAULT_I2C_ADDR );

    /** Create a PCA9956A instance connected to specified I2C pins with specified address
     *
     * @param i2c_obj       I2C object (instance)
     * @param i2c_address   I2C-bus address (default: 0xC0)
     */
    PCA9956A( I2C &i2c_obj, char i2c_address = PCA995xA::DEFAULT_I2C_ADDR );

    /** Destractor
     *
     */
    virtual         ~PCA9956A();
    
    /** Returns the number of output ports
     *
     *  @returns
     *    The number of output ports
     */
     virtual int     number_of_ports( void );

#if DOXYGEN_ONLY
    /** Set the output duty-cycle, specified as a percentage (float)
     *
     * @param port  Selecting output port
     *    'ALLPORTS' can be used to set all port duty-cycle same value.
     * @param v     A floating-point value representing the output duty-cycle,
     *    specified as a percentage. The value should lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 99.6%).
     *    Values outside this range will have undefined behavior.
     */
    void            pwm( int port, float v );
    
    /** Set all output port duty-cycle, specified as a percentage (array of float)
     *
     * @param vp    Aray to floating-point values representing the output duty-cycle,
     *    specified as a percentage. The value should lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 99.6%).
     *
     *  @note
     *    The aray should have length of 24
     */
    void            pwm( float *vp );
    
    /** Set the output current, specified as a percentage (float)
     *
     * @param port  Selecting output port
     *    'ALLPORTS' can be used to set all port duty-cycle same value.
     * @param v     A floating-point value representing the output current,
     *    specified as a percentage. The value should lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 100%).
     *    Values outside this range will have undefined behavior.
     */    
    void            current( int port, float vp );

    /** Set all output port curent, specified as a percentage (array of float)
     *
     * @param vp    Aray to floating-point values representing the output current,
     *    specified as a percentage. The value should lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 100%).
     *
     *  @note
     *    The aray should have length of 24
     */
    void            current( float *vP );
    
    /** Register write (single byte) : Low level access to device register
     *
     * @param reg_addr  Register address
     * @param data      Value for setting into the register
     */    
    void            write( char reg_addr, char data );

    /** Register write (multiple bytes) : Low level access to device register
     *
     * @param data      Pointer to an array. First 1 byte should be the writing start register address
     * @param length    Length of data
     */    
    void            write( char *data, int length );

    /** Register read (single byte) : Low level access to device register
     *
     * @param reg_addr  Register address
     * @return          Read value from register
     */    
    char            read( char reg_addr );

    /** Register write (multiple bytes) : Low level access to device register
     *
     * @param reg_addr  Register address
     * @param data      Pointer to an array. The values are stored in this array.
     * @param length    Length of data
     */    
    void            read( char reg_addr, char *data, int length );
#endif

private:
    void            initialize( void );
    virtual char    pwm_register_access( int port );
    virtual char    current_register_access( int port );

    const int       n_of_ports;
}
;

#endif  //  MBED_PCA9956A
