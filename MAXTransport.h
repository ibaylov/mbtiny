#ifndef _MAXTRANSPORT_H_
#define _MAXTRANSPORT_H_

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

#include <MBusTiny.h>
#include <MBRTUAdapter.h>
#include <MAX3100-485.h>

/*! @brief class MAXTransport - implements IRTUTransport for Maxim MAX 3100 UART */
class MAXTransport: public IRTUTransport
{
protected:
    MAX3100485 m_serialMax;     //!< MAX 3100 driver class instance
public:
    MAXTransport( uint32_t nRate, int nCSPin ); //!< Constructor with bps rate and Chip Select Pin index arguments 
    virtual bool AvailableIn(); //!< overrides base class method
    virtual bool ReceiveBuffer(
                          uint8_t* pbBuffer,
                          size_t nBuffer); //!< overrides base class method
    virtual bool TransmitBuffer(
                          const uint8_t* pbBuffer,
                          size_t nBuffer) ; //!< overrides base class method
    virtual uint16_t CharUS(); //!< overrides base class method

};


#endif
