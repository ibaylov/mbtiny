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

#include <MAXTransport.h>
#include <Arduino.h>

MAXTransport::MAXTransport( uint32_t nRate, int nCSPin )
    : m_serialMax( 2, nCSPin, /* rev5 true */ false )
{
    m_serialMax.begin( nRate, 8, false, 1);
}

bool
MAXTransport::AvailableIn()
{
    return m_serialMax.available();
}
bool
MAXTransport::ReceiveBuffer(
                      uint8_t* pbBuffer,
                      size_t nBuffer)
{
    return m_serialMax.read( pbBuffer, nBuffer );
}

bool
MAXTransport::TransmitBuffer(
                      const uint8_t* pbBuffer,
                      size_t nBuffer)
{
  size_t nRV = m_serialMax.write(pbBuffer,nBuffer);
  return nRV == nBuffer;
}

uint16_t
MAXTransport::CharUS()
{
    return m_serialMax.GetCharUS();
}
