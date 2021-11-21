#ifndef _MBRTUARAPTER_H_
#define _MBRTUARAPTER_H_

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

/*! @brief class IRTUTransport - the base class for PDU data transport, used by MBRTUAdapter */
class IRTUTransport
{
public:
    virtual bool AvailableIn() = 0; //!< returns true on data available for read

    /*!  @brief ReceiveBuffer - receives a block of data from transport
    *    @param pbBuffer   - the buffer that should receive data
    *    @param nBuffer    - the size of the buffer
    *    @returns          - true on success, false o.w.
    */     
    virtual bool ReceiveBuffer(
                          uint8_t* pbBuffer,
                          size_t nBuffer) = 0;

    /*!  @brief TransmitBuffer - transmits a block of data via transport
    *    @param pbBuffer   - the buffer that should be sent
    *    @param nBuffer    - the size of the buffer
    *    @returns          - true on success, false o.w.
    */     
    virtual bool TransmitBuffer(
                          const uint8_t* pbBuffer,
                          size_t nBuffer) = 0;

    /*!  @brief CharUS - retrieves a character transmition time 
    *    @returns          - single character transmissio in microseconds
    */     
    virtual uint16_t CharUS() = 0;

};

/*! @brief class MBRTUAdapter - Modbus RTU implementation that uses an IRTUTransport implementation for data trancieve */
class MBRTUAdapter : public IPDUAdapter
{
protected:
    IRTUTransport* m_pTransport;    //!< transport instance pointer
public:
    MBRTUAdapter();                 //!< defaut constructor
    MBRTUAdapter( IRTUTransport* ); //!< transport instance pointer constructor
    virtual bool AvailableIn();     //!< overides base class method
    virtual bool ReceivePDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer,
                          size_t& nBufferOut ); //!< overides base class method

    virtual bool TransmitPDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer) ; //!< overides base class method
};


#endif
