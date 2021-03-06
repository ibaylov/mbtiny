/*
* MIT License
* 
* Copyright (c) 2021 Ivaylo Baylov, ibaylov@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "MBusTiny.h"
#include "MBRTUAdapter.h"
#include "CrcFsm.h"
#include <Arduino.h>

MBRTUAdapter::MBRTUAdapter()
{
    m_pTransport = NULL;
}

MBRTUAdapter::MBRTUAdapter( IRTUTransport* pTrans )
{
    m_pTransport =  pTrans ;
}

bool
MBRTUAdapter::AvailableIn()
{
    return m_pTransport->AvailableIn();
}


bool
MBRTUAdapter::ReceivePDU(
                      uint8_t uSDeviceID,
                      uint8_t* pbBuffer,
                      size_t nBuffer,
                      size_t& nBufferOut )
{
    bool bRV = false;
    bRV = m_pTransport->ReceiveBuffer( pbBuffer, 6 ); // station ID + function code + ARG 0 + ARG 1
    if( ! bRV )
        return false;

    nBufferOut = 6;

    if( MBUSTiny::HasTrailing(pbBuffer[1]) )
    {
        nBufferOut += 1;
        bRV = m_pTransport->ReceiveBuffer( pbBuffer + 6, 1 ); // Trailing payload in bytes
        if( ! bRV )
            return false;

        nBufferOut += pbBuffer[ 6 ];

        bRV = m_pTransport->ReceiveBuffer( pbBuffer + 7, pbBuffer[ 6 ]  + 2  ); // Trailing payload + CRC
        if( ! bRV )
            return false;

    }
    else
        bRV = m_pTransport->ReceiveBuffer( pbBuffer + 6,  2  ); // CRC

    uint16_t uCRC = CRC16Fsm( pbBuffer, nBufferOut + 2 );

    if( uCRC != 0 )
        return false;

    if( pbBuffer[0] != uSDeviceID )
        return false;
    // wait for the transmission gap
    delayMicroseconds( ((uint32_t)36 * m_pTransport->CharUS())/10);
    return true;

}

bool
MBRTUAdapter::TransmitPDU(
                      uint8_t uSDeviceID,
                      uint8_t* pbBuffer,
                      size_t nBuffer)
{
    uint16_t uCRCLocal  = CRC16Fsm(pbBuffer, nBuffer);
    pbBuffer[ nBuffer ] = uCRCLocal % 256;
    pbBuffer[ nBuffer + 1 ] = uCRCLocal / 256;
    // test wrong CRC pbBuffer[ nBuffer + 1 ] ++;
    return m_pTransport->TransmitBuffer( pbBuffer, nBuffer + 2 );
}
