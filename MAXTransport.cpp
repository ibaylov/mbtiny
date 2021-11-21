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
                      uint8_t* pbBuffer,
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
