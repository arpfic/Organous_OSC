/** PCA9955A constant current LED driver
 *
 *  An operation sample of PCA9955A 16-channel Fm+ I2C-bus 57mA/20V constant current LED driver.
 *  mbed accesses PCA9955A registers via I2C.
 *
 *  @class   PCA9955A
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.6
 *  @date    19-Mar-2015
 *
 *  Released under the Apache 2 license
 *
 *  About PCA9955A:
 *    http://www.nxp.com/products/interface_and_connectivity/i2c/i2c_led_display_control/PCA9955ATW.html
 */

#ifndef     MBED_PCA9955A
#define     MBED_PCA9955A

#include    "mbed.h"
#include    "PCA995xA.h"
#include    "LedPwmOutCC.h"

/** PCA9955A class
 *
 *  This is a driver code for the PCA9955A 16-channel Fm+ I2C-bus 57mA/20V constant current LED driver.
 *  This class provides interface for PCA9955A operation and accessing its registers.
 *  Detail information is available on next URL.
 *    http://www.nxp.com/products/interface_and_connectivity/i2c/i2c_led_display_control/PCA9955ATW.html
 *
 *  Next sample code shows operation based on low-level-API (operated by just device instane)
 *
 *  Example:
 *  @code
 *  //  PCA9955A operation sample using its device instance
 *  
 *  #include "mbed.h"
 *  #include "PCA9955A.h"
 *  PCA9955A    led_cntlr( p28, p27, 0x02 );    //  SDA, SCL, Slave_address(option)
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
 *  //  PCA9955A operation sample using high-level-API
 *  
 *  #include "mbed.h"
 *  #include "PCA9955A.h"
 *  
 *  PCA9955A    led_cntlr( p28, p27, 0x02 );    //  SDA, SCL, Slave_address(option)
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
class PCA9955A : public PCA995xA
{
public:

#if DOXYGEN_ONLY
    /** PCA9955A pin names high-level API i.e. LedPwmOutCC */
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
        L_NC = ~0x0L   /**< for when the pin is left no-connection */
    } LedPinName;
#endif // DOXYGEN_ONLY

    /** Name of the PCA9955A registers (for direct register access) */
    enum command_reg {
        MODE1   = 0x00,  /**< MODE1 register      */
        MODE2,          /**< MODE2 register      */
        LEDOUT0,        /**< LEDOUT0 register    */
        LEDOUT1,        /**< LEDOUT1 register    */
        LEDOUT2,        /**< LEDOUT2 register    */
        LEDOUT3,        /**< LEDOUT3 register    */
        GRPPWM  = 0x06, /**< GRPPWM register     */
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
        RAMP_RATE_GRP0,         /**< RAMP_RATE_GRP0 register */
        STEP_TIME_GRP0,         /**< STEP_TIME_GRP0 register */
        HOLD_CNTL_GRP0,         /**< HOLD_CNTL_GRP0 register */
        IREF_GRP0,              /**< IREF_GRP0 register      */
        RAMP_RATE_GRP1,         /**< RAMP_RATE_GRP1 register */
        STEP_TIME_GRP1,         /**< STEP_TIME_GRP1 register */
        HOLD_CNTL_GRP1,         /**< HOLD_CNTL_GRP1 register */
        IREF_GRP1,              /**< IREF_GRP1 register      */
        RAMP_RATE_GRP2,         /**< RAMP_RATE_GRP2 register */
        STEP_TIME_GRP2,         /**< STEP_TIME_GRP2 register */
        HOLD_CNTL_GRP2,         /**< HOLD_CNTL_GRP2 register */
        IREF_GRP2,              /**< IREF_GRP2 register      */
        RAMP_RATE_GRP3,         /**< RAMP_RATE_GRP3 register */
        STEP_TIME_GRP3,         /**< STEP_TIME_GRP3 register */
        HOLD_CNTL_GRP3,         /**< HOLD_CNTL_GRP3 register */
        IREF_GRP3,              /**< IREF_GRP3 register      */
        GRAD_MODE_SEL0  = 0x38, /**< GRAD_MODE_SEL0 register */
        GRAD_MODE_SEL1,         /**< GRAD_MODE_SEL1 register */
        GRAD_GRP_SEL0,          /**< GRAD_GRP_SEL0 register  */
        GRAD_GRP_SEL1,          /**< GRAD_GRP_SEL1 register  */
        GRAD_GRP_SEL2,          /**< GRAD_GRP_SEL2 register  */
        GRAD_GRP_SEL3,          /**< GRAD_GRP_SEL3 register  */
        GRAD_CNTL,              /**< GRAD_CNTL register      */
        OFFSET  = 0x3F, /**< OFFSET register     */
        SUBADR1,        /**< SUBADR1 register    */
        SUBADR2,        /**< SUBADR2 register    */
        SUBADR3,        /**< SUBADR3 register    */
        ALLCALLADR,     /**< ALLCALLADR register */
        PWMALL,         /**< PWMALL register     */
        IREFALL,        /**< IREFALL register    */
        EFLAG0,         /**< EFLAG0 register     */
        EFLAG1,         /**< EFLAG1 register     */

        REGISTER_START          = MODE1,
        LEDOUT_REGISTER_START   = LEDOUT0,
        PWM_REGISTER_START      = PWM0,
        IREF_REGISTER_START     = IREF0,
        GRAD_GROUP_OFFSET       = RAMP_RATE_GRP1 - RAMP_RATE_GRP0
    };

    /** Create a PCA9955A instance connected to specified I2C pins with specified address
     *
     * @param i2c_sda       I2C-bus SDA pin
     * @param i2c_sda       I2C-bus SCL pin
     * @param i2c_address   (option) I2C-bus address (default: 0xC0)
     */
    PCA9955A( PinName i2c_sda, PinName i2c_scl, char i2c_address = PCA995xA::DEFAULT_I2C_ADDR );

    /** Create a PCA9955A instance connected to specified I2C pins with specified address
     *
     * @param i2c_obj       I2C object (instance)
     * @param i2c_address   (option) I2C-bus address (default: 0xC0)
     */
    PCA9955A( I2C &i2c_obj, char i2c_address = PCA995xA::DEFAULT_I2C_ADDR );

    /** Destractor
     *
     */
    virtual         ~PCA9955A();

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
     *    0.0f (representing on 0%) and 1.0f (representing on 100%).
     *    Values outside this range will have undefined behavior.
     */
    void            pwm( int port, float v );

    /** Set all output port duty-cycle, specified as a percentage (array of float)
     *
     * @param vp    Aray to floating-point values representing the output duty-cycle,
     *    specified as a percentage. The value should lie between
     *    0.0f (representing on 0%) and 1.0f (representing on 100%).
     *
     *  @note
     *    The aray should have length of 16
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
     *    The aray should have length of 16
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

    /** Gradation control setting
     *
     * Auto calculation and register setting API for gradation control.
     * User can set the gradation by specifying cycle, hold-ON/OFF time and ramp-up/down enabling.
     * All register values are calcurated automatically to have fine gradation setting.
     * In this routine, the IREF (current) setting is fixed to maximum (i.e. 0xFF)
     *
     * @param group     Group selector
     * @param cycle     Ramp rate register (RAMP_RATE_GRPn) value
     * @param flag_on   Holding time for ON can be selected from available constants.
     *    Those are 0, 0.25, 0.5, 0.75, 1, 2, 4 and 6 seconds.
     *    Use one of next words: HOLD_0_00_SEC, HOLD_0_25_SEC, HOLD_0_50_SEC, HOLD_0_75_SEC, HOLD_1_00_SEC, HOLD_2_00_SEC, HOLD_4_00_SEC or HOLD_6_00_SEC
     * @param flag_off  Holding time for OFF can be selected from available constants.
     *    Those are 0, 0.25, 0.5, 0.75, 1, 2, 4 and 6 seconds.
     *    Use one of next words: HOLD_0_00_SEC, HOLD_0_25_SEC, HOLD_0_50_SEC, HOLD_0_75_SEC, HOLD_1_00_SEC, HOLD_2_00_SEC, HOLD_4_00_SEC or HOLD_6_00_SEC
     * @param ramp_flag Choose one from next words : NO_RAMP, RAMP_UP_ONLY, RAMP_DOWN_ONLY or RAMP_UP_DOWN
     *
     * @return  calculated actual time for one cycle (in seconds)
     *
     * @see gradation_setting()
     */
    float           gradation_ramp_setting( char group, float cycle, char flag_on, char flag_off, int ramp_flag );

    /** Gradation control: Port assignment to group
     *
     * Each ports which is going to use in gradation control should be assigned to group.
     * This function help to assign the port to 1 of 4 groups.
     *
     * @param group     Target group
     * @param port      A output port which is being assigned to the target group
     */
    void            gradation_group_setting( char group, char port );

    /** Gradation control: Start
     *
     * @param group             Group selector
     * @param continuous_flag   (option) Set false for one-shot operation
     */
    void            gradation_start( char group, char continuous_flag = true );

    /** Gradation control: Stop
     *
     */
    void            gradation_stop( void );


    /** Gradation control: Clear all group assignments
     *
     *  Set all output port operation to no-gradation control
     *
     */
    void            gradation_group_clear( void );

    /** Setting gradation control registers (low level setting API)
     *
     * @param group     Group selector
     * @param ramp_rate Ramp rate register (RAMP_RATE_GRPn) value
     * @param step_time Step time register (STEP_TIME_GRPn) value
     * @param hold_cntl Hold control register (HOLD_CNTL_GRPn) value
     * @param iref      IREF (current setting) register (IREF_GRPn) value
     *
     * @see gradation_ramp_setting()
     */
    void            gradation_setting( int group, char ramp_rate, char step_time, char hold_cntl, char iref );

    /** Setting gradation register dump (for debugging)
     *
     */
    void            dump_gradation_registers( void );

private:
    void            initialize( void );
    virtual char    pwm_register_access( int port );
    virtual char    current_register_access( int port );

    void            group_selector( short g0, short g1, short g2, short g3 );

    const int       n_of_ports;
    static char     gradation_group[];
}
;

/** Constants for Gradation control */
enum GradationControlConstants {
    CONTINUOUS  = 1,
    NOGROUP     = 0xFF
};

/** Flags for Gradation control (Ramp selector) */
enum GradationControlRampSelector {
    NO_RAMP = 0x0,
    RAMP_UP_ONLY,
    RAMP_DOWN_ONLY,
    RAMP_UP_DOWN
};

/** Flags for Gradation control (Hold time selector) */
enum GradationControlHoldTimeSelector {
    HOLD_0_00_SEC   = 0x0,
    HOLD_0_25_SEC,
    HOLD_0_50_SEC,
    HOLD_0_75_SEC,
    HOLD_1_00_SEC,
    HOLD_2_00_SEC,
    HOLD_4_00_SEC,
    HOLD_6_00_SEC
};

#endif  //  MBED_PCA9955A
