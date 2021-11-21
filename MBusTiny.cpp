#include "MBusTiny.h"
#include <string.h>
#include <Arduino.h>

MBUSTiny::MBUSTiny()
{

}

MBUSTiny::MBUSTiny(IPDUAdapter* pAdapter , uint8_t uDevID)
{
    m_pAdapter = pAdapter;
    m_uDeviceID = uDevID;
}

MBUSTiny::~MBUSTiny()
{

}


MBUSTiny::exCode
MBUSTiny::DIInputs( int nIndex, int& rbValue )
{
    rbValue = 0;
    return exOK;
}

MBUSTiny::exCode
MBUSTiny::DIOCoils( int nIndex, int& rbValue, eDataDir dirData )
{
    return exOK;
}

MBUSTiny::exCode
MBUSTiny::DIRegisters( int nIndex, uint8_t* pbValue)
{
    return exOK;
}

MBUSTiny::exCode
MBUSTiny::DIOHoldingRegs( int nIndex, uint8_t* pbValue, eDataDir dirData )
{
    return exOK;
}


bool
MBUSTiny::HasTrailing( uint8_t uFC )
{
    switch( static_cast<eFunctionCode>(uFC))
    {
     case   fcWriteMultipleCoils:
     case   fcWriteMultipleHoldingRegisters:
        return true;
     }
    return false;
}


const size_t cbDevAddr = 0;
const size_t cbFC = 1;
const size_t cbRqArrdess = 2;
const size_t cbRqCount = 4;

//////////////////////////////////////////////////////////////////////////////////////////
/// \brief PMNtoH16 - Poor man's Neto to host 16bit
/// \param pbBase
/// \return
///
static uint16_t
PMNtoH16( const uint8_t* pbBase )
{
    uint16_t uRV = pbBase[  0 ];
    uRV *= 256;
    uRV += pbBase[  1 ];
    return uRV;
}
///////////////////////////////////////////////////////////////////////////////////////////
/// \brief MBRPushBit
/// \param pbMBR
/// \param nIndex
/// \param bSet
///
///
static void
MBRPushBit( uint8_t* pbMBR, size_t nIndex, bool bSet )
{
    size_t nBase = nIndex / 8;
    if( bSet )
        pbMBR[ nBase ] |= 1U << ( nIndex % 8 );
    else
        pbMBR[ nBase ] &= ~(1U << ( nIndex % 8 ));
}


bool
MBUSTiny::TranscievePDU()
{
  // verify presence
  bool bRV = false;
  bRV  = m_pAdapter->AvailableIn();
  if( ! bRV )
      return bRV;

  size_t nRX = 0;
  bRV = m_pAdapter->ReceivePDU( m_uDeviceID, m_arrbPDU, nPDU, nRX );
  if( ! bRV )
      return bRV;
   // now, decode PDU
  exCode exRV = exOK;
  size_t cbRPDU = 0;
  switch( static_cast<eFunctionCode>(m_arrbPDU[ cbFC ]))
  {
   case   fcReadCoils:
      exRV = DoReadCoils( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcReadDiscreteInputs:
      exRV = DoReadDiscreteInputs( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcReadMultipleHoldingRegisters:
      exRV = DoReadMultipleHoldingRegisters( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcReadInputRegisters:
      exRV = DoReadInputRegisters( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcWriteSingleCoil:
      exRV = DoWriteSingleCoil( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcWriteSingleHoldingRegister:
      exRV = DoWriteSingleHoldingRegister( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcWriteMultipleCoils:
      exRV = DoWriteMultipleCoils( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   case   fcWriteMultipleHoldingRegisters:
      exRV = DoWriteMultipleHoldingRegisters( m_arrbPDU + cbRqArrdess, cbRPDU );
      break;
   default:
       exRV = exIllegalFunction;
   }


  if( exRV == exOK )
      return m_pAdapter->TransmitPDU( m_uDeviceID, m_arrbPDU, cbRPDU + 2 );

  // error case
  m_arrbPDU[cbFC] |= 0x80;
  m_arrbPDU[cbFC + 1 ] = static_cast<uint8_t>(exRV);
  return m_pAdapter->TransmitPDU( m_uDeviceID, m_arrbPDU, 3 );

}

static size_t
BytesFromBitCount( size_t nBits )
{
    return nBits / 8 + !! ( nBits %  8 );
}

MBUSTiny::exCode
MBUSTiny::DoReadCoils( uint8_t* pbRV, size_t& rcbOut )
{
    uint16_t uBase   = PMNtoH16( pbRV );
    uint16_t uExtent = PMNtoH16( pbRV + 2 );

    size_t nExtentBytes = BytesFromBitCount(uExtent );
    memset( pbRV, 0, nExtentBytes + 1);
    pbRV[ 0 ] = nExtentBytes;

    exCode exRV = exOK;

    int cElem;
    for( cElem = 0; cElem < uExtent; cElem ++ )
    {
        int bVal = 0;
        exRV = DIOCoils( uBase + cElem, bVal, eDataDir::dataRead );
        if( exRV != exOK )
            return exRV;
        MBRPushBit( pbRV + 1, cElem, bVal );
    }
    rcbOut  = nExtentBytes + 1;
    return exRV;
}



MBUSTiny::exCode
MBUSTiny::DoReadDiscreteInputs(uint8_t *pbRV, size_t& rcbOut )
{
    uint16_t uBase   = PMNtoH16( pbRV );
    uint16_t uExtent = PMNtoH16( pbRV + 2 );

    size_t nExtentBytes = BytesFromBitCount(uExtent );
    memset( pbRV, 0, nExtentBytes + 1);
    pbRV[ 0 ] = nExtentBytes;

    exCode exRV = exOK;

    int cElem;
    for( cElem = 0; cElem < uExtent; cElem ++ )
    {
        int bVal = 0;
        exRV = DIInputs( uBase + cElem, bVal );
        if( exRV != exOK )
            return exRV;
        MBRPushBit( pbRV + 1, cElem, bVal );
    }
    rcbOut  = nExtentBytes + 1;
    return exRV;
}

MBUSTiny::exCode
MBUSTiny::DoReadMultipleHoldingRegisters(uint8_t *pbRV, size_t& rcbOut )
{
    return exIllegalFunction;
}

MBUSTiny::exCode
MBUSTiny::DoReadInputRegisters(uint8_t *pbRV, size_t& rcbOut )
{
    uint16_t uBase   = PMNtoH16( pbRV );
    uint16_t uExtent = PMNtoH16( pbRV + 2 );

    size_t nExtentBytes = uExtent * sizeof( uint16_t );
    memset( pbRV, 0, nExtentBytes + 1);
    pbRV[ 0 ] = nExtentBytes;
    uint8_t* pbOut = pbRV + 1;

    exCode exRV = exOK;

    int cElem;
    for( cElem = 0; cElem < uExtent; cElem ++, pbOut += sizeof( uint16_t))
    {
        exRV = DIRegisters( uBase + cElem, pbOut );
        if( exRV != exOK )
            return exRV;
    }
    rcbOut  = nExtentBytes + 1;
    return exRV;
}

MBUSTiny::exCode
MBUSTiny::DoWriteSingleCoil(uint8_t *pbRV, size_t& rcbOut )
{
    uint16_t uAddress   = PMNtoH16( pbRV );
    uint16_t uValue     = PMNtoH16( pbRV + 2 );
    rcbOut = 2 * sizeof( uint16_t );
    int bVal = uValue > 0 ;
    return DIOCoils( uAddress, bVal, eDataDir::dataWrite );
}

MBUSTiny::exCode
MBUSTiny::DoWriteSingleHoldingRegister(uint8_t *pbRV, size_t& rcbOut )

{
    return exIllegalFunction;
}

MBUSTiny::exCode
MBUSTiny::DoWriteMultipleCoils(uint8_t *pbRV, size_t& rcbOut )

{
    return exIllegalFunction;
}

MBUSTiny::exCode
MBUSTiny::DoWriteMultipleHoldingRegisters(uint8_t *pbRV, size_t& rcbOut )

{
    return exIllegalFunction;
}
