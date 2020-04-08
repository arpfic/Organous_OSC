#include    "mbed.h"
#include    "PCA9955A.h"

PCA9955A::PCA9955A( PinName i2c_sda, PinName i2c_scl, char i2c_address )
    : PCA995xA( i2c_sda, i2c_scl, i2c_address ), n_of_ports( 16 )
{
    initialize();
}

PCA9955A::PCA9955A( I2C &i2c_obj, char i2c_address )
    : PCA995xA( i2c_obj, i2c_address ), n_of_ports( 16 )
{
    initialize();
}

PCA9955A::~PCA9955A()
{
}

void PCA9955A::initialize( void )
{
    char init_array[] = {
        PCA995xA::AUTO_INCREMENT | REGISTER_START,  //  Command
        0x00, 0x00,                                 //  MODE1, MODE2
        0xAA, 0xAA, 0xAA, 0xAA,                     //  LEDOUT[3:0]
        0x80, 0x00,                                 //  GRPPWM, GRPFREQ
    };

    pwm( ALLPORTS, 0.0 );    
    current( ALLPORTS, 0.1 );    

    write( init_array, sizeof( init_array ) );
//    gradation_group_clear();
}

char PCA9955A::pwm_register_access( int port )
{
    if ( port < n_of_ports )
        return ( PWM_REGISTER_START + port );

    return ( PWMALL );
}

char PCA9955A::current_register_access( int port )
{
    if ( port < n_of_ports )
        return ( IREF_REGISTER_START + port );

    return ( IREFALL );
}

int PCA9955A::number_of_ports( void )
{
    return ( n_of_ports );
}


/*char  PCA9955A::gradation_group[ 16 ] = { 0xFF };

void PCA9955A::gradation_setting( int group, char ramp_rate, char step_time, char hold_cntl, char iref )
{
    char    data[ 5 ]   = { (RAMP_RATE_GRP0 + (group * GRAD_GROUP_OFFSET)) | AUTO_INCREMENT,
                            ramp_rate,
                            step_time,
                            hold_cntl,
                            iref
                          };

    write( data, sizeof( data ) );
}

void PCA9955A::gradation_start( char group, char continuous_flag )
{
    static char v[]   = { GRAD_CNTL, 0 };
    char        rd;
    
    rd      = read( GRAD_CNTL ) & ~(0x3 << (group * 2));
    
    printf( "rd = 0x%02X\r\n", rd );
    v[ 1 ]  = rd | ((0x02 | continuous_flag) << (group * 2));
//    v[ 1 ]  = ((0x02 | continuous_flag) << (group * 2));
    write( v, sizeof( v ) );
}

void PCA9955A::gradation_stop( void )
{
    char    v[]   = { GRAD_CNTL, 0 };

    write( v, sizeof( v ) );
}

void PCA9955A::group_selector( short g0, short g1, short g2, short g3 )
{
    unsigned int    pattern = 0x00000000;
    short           src[ 4 ];
    short           port_select;
    char            v[ 7 ];
    int             i;
    int             grp;

    v[ 0 ]  = GRAD_MODE_SEL0 | AUTO_INCREMENT;

    port_select = g0 | g1 | g2 | g3;
    v[ 1 ]  = (port_select >> 0) & 0xFF;
    v[ 2 ]  = (port_select >> 8) & 0xFF;

    if ( 0x0000 == port_select ) {
        write( v, 3 );
        return;
    }

    src[ 0 ]    = g0;
    src[ 1 ]    = g1;
    src[ 2 ]    = g2;
    src[ 3 ]    = g3;

    for ( i = 0; i < 16; i++ ) {
        for ( grp = 0; grp < 4; grp++ ) {
            if ( src[ grp ] & (0x1 << i) )
                pattern |= grp << (i * 2);
        }
    }

    for ( i = 0; i < 4; i++ )
        v[ i + 3 ]  = 0xFF & (pattern >>  (i * 8));

    write( v, sizeof( v ) );
}

void PCA9955A::gradation_group_setting( char group, char port )
{
    short   g[ 4 ]  = { 0x0000 };

    port    &= 0xF;
    gradation_group[ port ] = (group < 4) ? group : NOGROUP;

    for ( int i = 0; i < 16; i++ ) {
        if ( gradation_group[ i ] != NOGROUP )
            g[ gradation_group[ i ] ]  |= 0x0001 << i;
    }

    group_selector( g[ 0 ], g[ 1 ], g[ 2 ], g[ 3 ] );
//    pwm( port, 1.0 );
    pwm( port, 0.3 );
}

void PCA9955A::gradation_group_clear( void )
{
    for ( int i = 0; i < 16; i++ ) {
        gradation_group[ i ] = NOGROUP;
    }

    group_selector( 0, 0, 0, 0 );
}

float PCA9955A::gradation_ramp_setting( char group, float cycle, char flag_on, char flag_off, int ramp_flag )
{
    int     ramp_time;
    char    prescaler;
    int     required_steps;
    char    mult_factor;
    char    iref_inc;
    int     iref        = 255;

#define     RAMP_RATE_GRP   1
#define     STEP_TIME_GRP   2
#define     HOLD_CNTL_GRP   3
#define     IREF_GRP        4

    char    regs[ 5 ];

    float hold_time[]   = {
        0.00,
        0.25,
        0.50,
        0.75,
        1.00,
        2.00,
        4.00,
        6.00
    };


    if ( ramp_flag == NO_RAMP ) {
        iref_inc        = 1;
        ramp_time       = 0;
        prescaler       = 0;
        required_steps  = 0;
        mult_factor     = 0;
    } else {
        ramp_time   = (int)((cycle - (hold_time[ flag_on ] + hold_time[ flag_off ])) / ((ramp_flag == RAMP_UP_DOWN) ? 2.0 : 1.0) * 1000);

        prescaler       = ( ramp_time <= 32 * (iref + 1)) ? 0 : 1;

        if ( prescaler )
            required_steps  = ramp_time / 8;
        else
            required_steps  = ramp_time * 2;

        if ( iref < required_steps ) {
            iref_inc    = 1;
            mult_factor = required_steps / (iref + 1);
            mult_factor = (mult_factor <= 0) ?  1 : mult_factor;
            mult_factor = (64 < mult_factor) ? 64 : mult_factor;
        } else {
            iref_inc    = (iref + 1) / required_steps;
            iref_inc    = (iref_inc <= 0) ?  1 : iref_inc;
            iref_inc    = (64 < iref_inc) ? 64 : iref_inc;
            mult_factor = 1;
        }
    }

//    printf( "ramp_time=%d, required_steps=%d, iref=%d\r\n", ramp_time, required_steps, iref );
//    printf( "prescaler = %d, mult_factor=%d, iref_inc=%d\r\n", prescaler, mult_factor, iref_inc );
//    printf( "\r\n" );

    regs[ 0             ]   = 0x80 | (RAMP_RATE_GRP0 + (group * 4));
    regs[ RAMP_RATE_GRP ]   = (ramp_flag << 6) | (iref_inc - 1);
    regs[ STEP_TIME_GRP ]   = (prescaler << 6) | (mult_factor - 1);
    regs[ HOLD_CNTL_GRP ]   = 0xC0 | (flag_on << 3) | flag_off;
    regs[ IREF_GRP      ]   = iref;

    write( regs, sizeof( regs ) );
    
#if 0
    float   n_of_steps;
    float   single_ramp_time;
    float   total_ramp_time;
    
    n_of_steps          = (float)(((iref + 1) / iref_inc) * mult_factor);
    single_ramp_time    = n_of_steps * (prescaler ? 8.0 : 0.5);
    total_ramp_time     = single_ramp_time * ((ramp_flag == RAMP_UP_DOWN) ? 2.0 : 1.0);
    printf( "n_of_steps = %f, single_ramp_time=%f, total_ramp_time=%f\r\n", n_of_steps, single_ramp_time, total_ramp_time );
    printf( "hold_time[ flag_on ] = %f, hold_time[ flag_off ]=%f\r\n", hold_time[ flag_on ], hold_time[ flag_off ] );
#endif

    return ( (((((float)(((iref + 1) / iref_inc) * mult_factor)) * (prescaler ? 8.0 : 0.5)) * ((ramp_flag == RAMP_UP_DOWN) ? 2.0 : 1.0)) ) * 0.001 + (hold_time[ flag_on ] + hold_time[ flag_off ]) );
}


void PCA9955A::dump_gradation_registers( void )
{
    char    data[ 23 ];

    read( RAMP_RATE_GRP0, data, sizeof( data ) );

    printf( "GRAD_MODE_SEL[1|0]     = 0x%02X%02X\r\n", data[ 17 ], data[ 16 ] );
    printf( "GRAD_GRP_SEL[3|2|1|0]  = 0x%02X%02X%02X%02X\r\n", data[ 21 ], data[ 20 ], data[ 19 ], data[ 18 ] );
    printf( "GRAD_CNTL      = 0x%02X\r\n", data[ 22 ] );
    printf( "RAMP_RATE  =  0x%02X  0x%02X  0x%02X  0x%02X\r\n", data[ 0 ], data[ 4 ], data[ 8 ], data[ 12 ] );
    printf( "STEP_TIME  =  0x%02X  0x%02X  0x%02X  0x%02X\r\n", data[ 1 ], data[ 5 ], data[ 9 ], data[ 13 ] );
    printf( "HOLD_CNTL  =  0x%02X  0x%02X  0x%02X  0x%02X\r\n", data[ 2 ], data[ 6 ], data[ 10 ], data[ 14 ] );
    printf( "IREF       =  0x%02X  0x%02X  0x%02X  0x%02X\r\n", data[ 3 ], data[ 7 ], data[ 11 ], data[ 15 ] );
    printf( "\r\n" );
}
*/