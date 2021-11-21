#ifndef _MAXTRANSPORT_H_
#define _MAXTRANSPORT_H_

#include <MBusTiny.h>
#include <MBRTUAdapter.h>
#include <MAX3100-485.h>

class MAXTransport: public IRTUTransport
{
protected:
    MAX3100485 m_serialMax;
public:
    MAXTransport( uint32_t nRate, int nCSPin );
    virtual bool AvailableIn();
    virtual bool ReceiveBuffer(
                          uint8_t* pbBuffer,
                          size_t nBuffer);
    virtual bool TransmitBuffer(
                          uint8_t* pbBuffer,
                          size_t nBuffer) ;
    virtual uint16_t CharUS();

};


#endif
