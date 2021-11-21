#ifndef _MBT_H_
#define _MBT_H_
#include <MBusTiny.h>

class MBT : public MBUSTiny
{
  public:

    MBT( IPDUAdapter* pAdapter , uint8_t uDevID ) : MBUSTiny( pAdapter , uDevID ) {}
  
    MB_DECLARE( )

    // callbacks
    MBUSTiny::exCode readLed( int, int& ){ return exOK;}  
    MBUSTiny::exCode ioPIN( int, int& ){ return exOK;}  
    MBUSTiny::exCode ioPIN2( int, int& ){ return exOK;}  

    MBUSTiny::exCode ioReg( int, uint8_t* ){ return exOK;}  
    MBUSTiny::exCode ioHoldReg( int, uint8_t* ){ return exOK;}  
};
#endif
