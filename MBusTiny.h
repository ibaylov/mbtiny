#ifndef _MBUS_TINY_H_
#define _MBUS_TINY_H_
#include <stdlib.h>
#include <stdint.h>

const size_t nPDU = 256;

class IPDUAdapter
{
public:
    virtual bool AvailableIn() = 0;
    virtual bool ReceivePDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer,
                          size_t& nBufferOut ) = 0;

    virtual bool TransmitPDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer) = 0;
};

class MBUSTiny
{
public:
    enum exCode
    {
        exOK                   =  0,
        exIllegalFunction      =  1,// Illegal Function	Function code received in the query is not recognized or allowed by slave
        exIllegalDataAddress   =  2,// Illegal Data Address	Data address of some or all the required entities are not allowed or do not exist in slave
        exIllegalDataValue     =  3,// Illegal Data Value          Value is not accepted by slave
        exSlaveDeviceFailure   =  4,// Slave Device Failure         Unrecoverable error occurred while slave was attempting to perform requested action
        exAcknowledge          =  5,// Acknowledge                  Slave has accepted request and is processing it, but a long duration of time is required. This response is returned to prevent a timeout error from occurring in the master. Master can next issue a Poll Program Complete message to determine whether processing is completed
        exSlaveDeviceBusy      =  6,// Slave Device Busy	Slave is engaged in processing a long-duration command. Master should retry later
        exNegativeAcknowledge  =  7,// Negative Acknowledge	Slave cannot perform the programming functions. Master should request diagnostic or error information from slave
        exMemoryParityError    =  8,// Memory Parity Error	Slave detected a parity error in memory. Master can retry the request, but service may be required on the slave device
        exGWPathUnavailable    = 10, // Gateway Path Unavailable	Specialized for Modbus gateways. Indicates a misconfigured gateway
        exGWTargetUnresponsive = 11 //	Gateway Target Device Failed to Respond	Specialized for Modbus gateways. Sent when slave fails to respond
    };

    enum eDataDir
    {
        dataRead,
        dataWrite
    };

    class exRuntime
    {
    private:
        exCode m_codeEx;
    public:
        exRuntime( exCode codeEx ) : m_codeEx( codeEx ){}
        exCode GetCode( ) const { return m_codeEx; }
    };

    enum eFunctionCode
    {
        fcReadCoils = 1,
        fcReadDiscreteInputs = 2,
        fcReadMultipleHoldingRegisters = 3,
        fcReadInputRegisters  = 4,
        fcWriteSingleCoil = 5,
        fcWriteSingleHoldingRegister	= 6,
        fcWriteMultipleCoils  = 15,
        fcWriteMultipleHoldingRegisters	= 16
    };

protected:
    uint8_t         m_arrbPDU[nPDU];
    IPDUAdapter*     m_pAdapter;
    uint8_t         m_uDeviceID;
public:
    MBUSTiny();
    MBUSTiny(IPDUAdapter*, uint8_t);
    virtual ~MBUSTiny();

    bool TranscievePDU();

protected:
    // default get/set processing
    virtual exCode DIInputs( int nIndex, int& rbValue );
    virtual exCode DIOCoils( int nIndex, int& rbValue, eDataDir dirData );
    virtual exCode DIRegisters( int nIndex, uint8_t* pbValue);
    virtual exCode DIOHoldingRegs( int nIndex, uint8_t* pbValue, eDataDir dirData );
private:
    exCode DoReadCoils( uint8_t* pbRV, size_t& rcbOut );
    exCode DoReadDiscreteInputs( uint8_t* pbRV, size_t& rcbOut );
    exCode DoReadMultipleHoldingRegisters( uint8_t* pbRV, size_t& rcbOut );
    exCode DoReadInputRegisters( uint8_t* pbRV, size_t& rcbOut );
    exCode DoWriteSingleCoil( uint8_t* pbRV, size_t& rcbOut );
    exCode DoWriteSingleHoldingRegister( uint8_t* pbRV, size_t& rcbOut );
    exCode DoWriteMultipleCoils( uint8_t* pbRV, size_t& rcbOut );
    exCode DoWriteMultipleHoldingRegisters( uint8_t* pbRV, size_t& rcbOut );
public:
    static bool HasTrailing( uint8_t uFC ) ;
};

// macros that do the magic
#define MB_DECLARE( )\
    virtual exCode DIInputs( int nIndex, int& rbValue ); \
    virtual exCode DIOCoils( int nIndex, int& rbValue, eDataDir dirData );\
    virtual exCode DIRegisters( int nIndex, uint8_t* pbValue );\
    virtual exCode DIOHoldingRegs( int nIndex, uint8_t* pbValue, eDataDir dirData );

#define MB_BEGIN_INPUTS( name, basename )\
MBUSTiny::exCode name::DIInputs( int nIndex, int& rbValue ){\
  rbValue = 0;\
  switch ( nIndex ){

#define MB_END_INPUTS( name, basename )\
 default:\
      return basename::DIInputs( nIndex, rbValue );\
  }\
  return exOK;\
}

#define MB_INPUT( index, fname )\
    case index:\
        return this->fname( index, rbValue );

#define MB_BEGIN_COILS( name, basename )\
MBUSTiny::exCode name::DIOCoils( int nIndex, int& rbValue, eDataDir dirData ){\
  if( dirData  == dataRead )\
    rbValue = 0;\
  switch ( nIndex ){

#define MB_END_COILS( name, basename )\
 default:\
      return basename::DIOCoils( nIndex, rbValue, dirData );\
  }\
  return exOK;\
}

#define MB_COIL( index, fnameGet, fnameSet )\
    case index:\
        if( dirData == dataRead )\
            return this->fnameGet( index, rbValue );\
        return this->fnameSet( index, rbValue );

#define MB_COIL_WO( index, fnameSet )\
    case index:\
        if( dirData == dataRead )\
            return exOK;\
        return this->fnameSet( index, rbValue );

#define MB_BEGIN_REGISTERS( name, basename )\
MBUSTiny::exCode name::DIRegisters( int nIndex, uint8_t* pbValue ){\
  switch ( nIndex ){

#define MB_END_REGISTERS( name, basename )\
 default:\
      return basename::DIRegisters( nIndex, pbValue );\
  }\
  return exOK;\
}

#define MB_REGISTER( index, fname )\
    case index:\
        return this->fname( index, pbValue );

#define MB_BEGIN_HOLDINGREGS( name, basename )\
MBUSTiny::exCode name::DIOHoldingRegs( int nIndex, uint8_t* pbValue, eDataDir dirData ){\
  switch ( nIndex ){

#define MB_END_HOLDINGREGS( name, basename )\
 default:\
      return basename::DIOHoldingRegs( nIndex, pbValue, dirData );\
  }\
  return exOK;\
}

#define MB_HOLDINGREG( index, fnameGet, fnameSet )\
    case index:\
        if( dirData == dataRead )\
            return this->fnameGet( index, pbValue );\
        return this->fnameSet( index, pbValue );

#define MB_HOLDINGREG_WO( index, fnameSet )\
    case index:\
        if( dirData == dataRead )\
            return exOK;\
        return this->fnameSet( index, pbValue );


#endif
