#include    "mbed.h"
#include    "PCA9956A.h"

PCA9956A::PCA9956A( PinName i2c_sda, PinName i2c_scl, char i2c_address )
    : PCA995xA( i2c_sda, i2c_scl, i2c_address ), n_of_ports( 24 )
{
    initialize();
}

PCA9956A::PCA9956A( I2C &i2c_obj, char i2c_address )
    : PCA995xA( i2c_obj, i2c_address ), n_of_ports( 24 )
{
    initialize();
}

PCA9956A::~PCA9956A()
{
}

void PCA9956A::initialize( void )
{
    char init_array[] = {
        PCA995xA::AUTO_INCREMENT | REGISTER_START,  //  Command
        0x00, 0x00,                                 //  MODE1, MODE2
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,         //  LEDOUT[5:0]
        0x80, 0x00,                                 //  GRPPWM, GRPFREQ
    };

    pwm( ALLPORTS, 0.0 );    
    current( ALLPORTS, 0.1 );    

    write( init_array, sizeof( init_array ) );
}

char PCA9956A::pwm_register_access( int port )
{
    if ( port < n_of_ports )
        return ( PWM_REGISTER_START + port );

    return ( PWMALL );
}

char PCA9956A::current_register_access( int port )
{
    if ( port < n_of_ports )
        return ( IREF_REGISTER_START + port );

    return ( IREFALL );
}

int PCA9956A::number_of_ports( void )
{
    return ( n_of_ports );
}
