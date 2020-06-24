/******************************************************************************************************/
/*  Pulse Density Modulation driver.                                                                  */
/*  FILE: PDM.cpp                                                                                     */
/*                                                                                                    */
/*  Code based on an article and source code released by Ken Wada from Aurium Technologies Inc.       */
/*  http://www.embedded.com/electronics-blogs/embedded-round-table/4414405/Pulse-density-modulation   */
/*                                                                                                    */
/******************************************************************************************************/

#include "SoftPdmOut.h"

SoftPdmOut::SoftPdmOut(PinName pdm, uint32_t PulseWidth, uint32_t Dmax, float StartLevel) : _pdm(pdm), _PulseWidth(PulseWidth), _Dmax(Dmax)
{
    _pdm = 0;
    _running = 0;
    if(StartLevel >= 0.0f && StartLevel <= 1.0f) _Level = (uint32_t)(StartLevel * (float)_Dmax);
    else _Level = 0;
    start();
}

void SoftPdmOut::start(void)
{
    if (!_running) {
        // enable ONLY if state has changed
        _accumulator  = 0;                                              // zero out the error accumulator
        _running      = 1;                                              // NOW ... enable the controller
        _pdmTicker.attach_us(callback(this,&SoftPdmOut::pdmTick),_PulseWidth);    // Start the PDM.
        //wait_ms(5);                                                     // Wait for output to settle
    }
}

void SoftPdmOut::stop(bool idleState)
{
    if (_running) {
        // disable ONLY if state has changed
        _running      = 0;          // disable the controller
        _pdmTicker.detach();        // Stop the PDM.
        _pdm = idleState;           // Set output (default = 0)
    }
}

void SoftPdmOut::PulseWidth(uint32_t level)
{
    if(!_running)                // When pdm is inactive, only change the pulse width and don't start the pdm
        _PulseWidth = level;
    else
    {
        stop();
        _PulseWidth = level;
        start();
    }
}

uint32_t SoftPdmOut::getPulseWidth(void)
{
    return _PulseWidth;
}

void SoftPdmOut::Dmax(uint32_t level)
{
    _Level = (uint32_t)((float)_Level * ((float)level / (float)_Dmax));  // Rescale level : level = level * (new level / current DMAX)
    _Dmax = level;
    _accumulator  = 0;                  // zero out the error accumulator
}

uint32_t SoftPdmOut::getDmax(void)
{
    return _Dmax;
}

void SoftPdmOut::write(float level)
{
    // Only change the level when the desired level is within range
    if(level <= 0.0f) _Level = 0;
    else if(level >= 1.0f) _Level = _Dmax;
    else if (level > 0.0f && level < 1.0f) _Level = (uint32_t)(level * (float)_Dmax);
}

float SoftPdmOut::read(void)
{
    return ((float)_Level / (float)_Dmax);
}

SoftPdmOut::operator float()
{
    return read();
}

SoftPdmOut & SoftPdmOut::operator= (float value)
{
    write(value);
    return(*this);
}

void SoftPdmOut::pdmTick(void)
{
    if(_Level == 0) {
        _pdm = 0;
        return;
    }
    if(_Level == _Dmax) {
        _pdm = 1;
        return;
    }
    _accumulator += _Level;         // accumulate error integral
    if (_accumulator > _Dmax) {
        _accumulator -= _Dmax;
        _pdm = 1;
    } else
        _pdm = 0;
}
