/******************************************************************************************************/
/*  Pulse Density Modulation driver.                                                                  */
/*  FILE: PDM.h                                                                                       */
/*                                                                                                    */
/*  Code based on an article and source code released by Ken Wada from Aurium Technologies Inc.       */
/*  http://www.embedded.com/electronics-blogs/embedded-round-table/4414405/Pulse-density-modulation   */
/*                                                                                                    */
/******************************************************************************************************/

#ifndef   __PDM_BB_H
#define   __PDM_BB_H

#include "mbed.h"

/** Class to use software defined Pulse Density Modulation (PDM).
*
* 'pdm' pin can be any digital pin.
* 
\code
//Example
//-------
#include "mbed.h"
#include "SoftPdmOut.h"

SoftPdmOut pdm(LED1);

int main()
{
    float pdmSet = 0.0f;
    float pdmAdd = 0.01f;

    // Continuously cycle the output
    while(1)
    {
        pdm = pdmSet;
        wait_ms(10);
        if(pdmSet >= 1.0f)
            pdmAdd = -0.01f;
        if(pdmSet <= 0.0f)
            pdmAdd = 0.01f;
        pdmSet += pdmAdd;
    }
}
\endcode
*/ 

class SoftPdmOut {
public:
    /** Create a PDM object connected to a digital pin.
    *
    * @param pdm        : Digital pin.
    * @param PulseWidth : Optional - The desired pulse width in microseconds (default = 500us).
    * @param Dmax       : Optional - This is the total number of states in the DAC output (default = 64).
    * @param StartLevel : Optional - The DAC percentage (0.0 to 1.0) to be preprogrammed upon initialization (default = 0).
    * @return none
    */
    SoftPdmOut(PinName pdm, uint32_t PulseWidth = 500, uint32_t Dmax = 64, float StartLevel = 0);

    /** Start the PDM.
    * @param none
    * @return none
    */
    void start(void);

    /** Stop the PDM.
    * @param idleState (optional, allows the user to define the idle state - default = 0)
    * @return none
    */
    void stop(bool idleState = 0);

    /** Change the pulse width.
    * @param level : The desired pulse width in microseconds.
    * @return none
    */
    void PulseWidth(uint32_t level);

    /** Read the pulse width.
    * @param none.
    * @return current PulseWidth value
    */
    uint32_t getPulseWidth(void);

    /** Change the total number of states in the DAC output (DMAX).
    * @param level : The desired max. level.
    * @return none.
    */
    void Dmax(uint32_t level);

    /** Read the total number of states in the DAC output (DMAX).
    * @param none.
    * @return current DMAX value.
    */
    uint32_t getDmax(void);

    /** Set the PDM level, specified as a percentage (float).
    * @param level
    * @return none
    */
    void write(float level);

    /** Return the current PDM level as a percentage (float).
    * @param none
    * @return level
    */
    float read(void);

    /**
    * An operator shorthand for read()
    */
    operator float();

    /**
    * An operator shorthand for write()
    */
    SoftPdmOut& operator= (float value);

private:
    Ticker _pdmTicker;
    DigitalOut _pdm;
    uint32_t _PulseWidth;
    uint32_t _Level;
    uint32_t _Dmax;
    uint32_t _accumulator;
    bool _running;
    void pdmTick(void);
};
#endif
