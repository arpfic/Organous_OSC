#include    "mbed.h"
#include    "LedPwmOutCC.h"

LedPwmOutCC::LedPwmOutCC( CompLedDvrCC &ledp, LedPinName pin_name )
    : leddvrp( &ledp ), pin( pin_name )
{
    pwm( 0.0 );
}

LedPwmOutCC::~LedPwmOutCC()
{
}

void LedPwmOutCC::pwm( float v )
{
    leddvrp->pwm( pin, v );
}

void LedPwmOutCC::current( float v )
{
    leddvrp->current( pin, v );
}
LedPwmOutCC& LedPwmOutCC::operator=( float rhs )
{
    pwm( rhs );
    return ( *this );
}