#include    "mbed.h"
#include    "PCA995xA.h"

PCA995xA::PCA995xA( PinName i2c_sda, PinName i2c_scl, event_callback_t cb_function, char i2c_address ) 
    : i2c_p( new I2C( i2c_sda, i2c_scl ) ), i2c( *i2c_p ), cb_function_p(cb_function), address( i2c_address )
{
}

PCA995xA::PCA995xA( I2C &i2c_, event_callback_t cb_function, char i2c_address ) 
    : i2c_p( NULL ), i2c( i2c_ ), cb_function_p(cb_function), address( i2c_address )
{
}

PCA995xA::~PCA995xA() 
{
    if ( NULL != i2c_p )
        delete  i2c_p;
}

void PCA995xA::reset( void )
{
    char    v   = 0x06;
    i2c.write( 0x00, &v, 1 );
}

void PCA995xA::pwm( int port, char v )
{
    char    reg_addr;
    
    reg_addr    = pwm_register_access( port );
    write( reg_addr, v );
}

void PCA995xA::pwm( char *vp )
{
    int     n_of_ports  = number_of_ports();
    char    data[ n_of_ports + 1 ];
    
    *data    = pwm_register_access( 0 );
    
    for ( int i = 1; i <= n_of_ports; i++ )
        data[ i ] = *vp++;
        
    write( data, sizeof( data ) );
}

void PCA995xA::current( int port, char v )
{
    char    reg_addr;
    
    reg_addr    = current_register_access( port );
    write( reg_addr, v );
}

void PCA995xA::current( char *vp )
{
    int     n_of_ports  = number_of_ports();
    char    data[ n_of_ports + 1 ];
    
    *data    = pwm_register_access( 0 );
    
    for ( int i = 1; i <= n_of_ports; i++ )
        data[ i ] = *vp++;
        
    write( data, sizeof( data ) );
}

void PCA995xA::write( char *data, int length )
{
//    int     r;

    *data   |= AUTO_INCREMENT;
/*    r = i2c.transfer(address, (char*)data, length, rx_buf, 1, event_callback_t(this, &PCA995xA::internal_cb_handler), I2C_EVENT_ALL );
    if(r != 0){
        cb_function_p.call(r);
    }*/
    i2c.write( address, data, length);
        //cb_function_p.call(-1);
}

void PCA995xA::write( char reg_addr, char data )
{
    char    c[2];
    c[0]    = reg_addr;
    c[1]    = data;

/*    r = i2c.transfer(address, (char*)c, 2, rx_buf, 1, event_callback_t(this, &PCA995xA::internal_cb_handler), I2C_EVENT_ALL );
    if(r != 0){
        cb_function_p.call(r);
    }*/
    i2c.write( address, c, 2 );
}

void PCA995xA::internal_cb_handler(int event) {
//    if (event & I2C_EVENT_ERROR_NO_SLAVE || event & I2C_EVENT_ERROR) {
        cb_function_p.call(event);
//    }
}

/*
void PCA995xA::read( char reg_addr, char *data, int length )
{
    reg_addr    |= 0x80;
    i2c.write( address, (char *)(&reg_addr), 1, true );
    i2c.read(  address, data, length );
}

char PCA995xA::read( char reg_addr )
{
    i2c.write( address, (char *)(&reg_addr), 1, true );
    i2c.read(  address, (char *)(&reg_addr), 1 );

    return ( reg_addr );
}
*/
