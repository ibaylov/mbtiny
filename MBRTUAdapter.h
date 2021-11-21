#ifndef _MBRTUARAPTER_H_
#define _MBRTUARAPTER_H_

#include "MBusTiny.h"

class IRTUTransport
{
public:
    virtual bool AvailableIn() = 0;
    virtual bool ReceiveBuffer(
                          uint8_t* pbBuffer,
                          size_t nBuffer) = 0;
    virtual bool TransmitBuffer(
                          uint8_t* pbBuffer,
                          size_t nBuffer) = 0;

    virtual uint16_t CharUS() = 0;

};

class MBRTUAdapter : public IPDUAdapter
{
protected:
    IRTUTransport* m_pTransport;
public:
    MBRTUAdapter();
    MBRTUAdapter( IRTUTransport* );
    virtual bool AvailableIn();
    virtual bool ReceivePDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer,
                          size_t& nBufferOut );

    virtual bool TransmitPDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer) ;
};


#endif
