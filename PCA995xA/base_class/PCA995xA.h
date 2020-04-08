/** PCA995xA constant current LED driver family
 *
 *  Abstract class for PCA995xA family
 *  No instance can be made from this class
 *
 *  @class   PCA9956A
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.5
 *  @date    25-Feb-2015
 *
 *  Released under the Apache 2 license
 */

#ifndef     MBED_PCA995xA
#define     MBED_PCA995xA

#include    "mbed.h"
#include    "CompLedDvrCC.h"

#define     ALLPORTS        0xFF
#define     DEFAULT_PWM     1.0
#define     DEFAULT_CURRENT 0.1

/** Abstract class for PCA995xA family
 *  
 *  No instance can be made from this class
 */
class PCA995xA : public CompLedDvrCC
{
public:
    PCA995xA( PinName i2c_sda, PinName i2c_scl, char i2c_address = DEFAULT_I2C_ADDR );
    PCA995xA( I2C &i2c_obj, char i2c_address = DEFAULT_I2C_ADDR );
    virtual ~PCA995xA();

    void            reset( void );

    virtual void    pwm( int port, float v );
    virtual void    current( int port, float vp );

    void            pwm( float *vp );
    void            current( float *vP );
    
    virtual int     number_of_ports( void )             = 0;

    void            write( char reg_addr, char data );
    void            write( char *data, int length );
    char            read( char reg_addr );
    void            read( char reg_addr, char *data, int length );

protected:
    enum {
        DEFAULT_I2C_ADDR    = 0xC0,
        AUTO_INCREMENT      = 0x80
    };

private:
    virtual char    pwm_register_access( int port )     = 0;
    virtual char    current_register_access( int port ) = 0;

    I2C             *i2c_p;
    I2C             &i2c;
    char            address;    //  I2C slave address
}
;

#endif  //  MBED_PCA995xA
