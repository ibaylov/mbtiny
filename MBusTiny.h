#ifndef _MBUS_TINY_H_
#define _MBUS_TINY_H_

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


#include <stdlib.h>
#include <stdint.h>

const size_t nPDU = 256;

/*!  @brief class IPDUAdapter - the base class for PDU transfer */ 
class IPDUAdapter
{
public:
    virtual bool AvailableIn() = 0; //!< Returns true if data input is available

    /*!  @brief ReceivePDU - receives a PDU
    *    @param uSDeviceID - the device Modbus ID/Address
    *    @param pbBuffer   - the buffer that should receive PDU
    *    @param nBuffer    - the size of the buffer
    *    @param nBufferOut - the actual size ot data read
    *    @returns          - true on success, false o.w.
    */     
    virtual bool ReceivePDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer,
                          size_t nBuffer,
                          size_t& nBufferOut ) = 0;

    /*! @brief TransmitPDU - transmitns a PDU
    *   @param pbBuffer   - the buffer that contains prepared PDU; note that the buffer is not const, comments below
    *   @param nBuffer    - the size of the buffer
    *   @returns          - true on success, false o.w.
    */    
    virtual bool TransmitPDU(
                          uint8_t uSDeviceID,
                          uint8_t* pbBuffer, // the actual implementation may add CRC here, so the pointer is not const and the buffer should be at least nBuffer + 2 bytes 
                          size_t nBuffer) = 0;
};

/*! @brief class MBUSTiny - the base class for Modbus device implementation */
class MBUSTiny
{
public:
    /*! @brief exCode - exception codes enumeration */
    enum exCode
    {
        exOK                   =  0,//!< OK, no error
        exIllegalFunction      =  1,//!< Illegal Function	Function code received in the query is not recognized or allowed by slave
        exIllegalDataAddress   =  2,//!< Illegal Data Address	Data address of some or all the required entities are not allowed or do not exist in slave
        exIllegalDataValue     =  3,//!< Illegal Data Value          Value is not accepted by slave
        exSlaveDeviceFailure   =  4,//!< Slave Device Failure         Unrecoverable error occurred while slave was attempting to perform requested action
        exAcknowledge          =  5,//!< Acknowledge                  Slave has accepted request and is processing it, but a long duration of time is required. This response is returned to prevent a timeout error from occurring in the master. Master can next issue a Poll Program Complete message to determine whether processing is completed
        exSlaveDeviceBusy      =  6,//!< Slave Device Busy	Slave is engaged in processing a long-duration command. Master should retry later
        exNegativeAcknowledge  =  7,//!< Negative Acknowledge	Slave cannot perform the programming functions. Master should request diagnostic or error information from slave
        exMemoryParityError    =  8,//!< Memory Parity Error	Slave detected a parity error in memory. Master can retry the request, but service may be required on the slave device
        exGWPathUnavailable    = 10,//!< Gateway Path Unavailable	Specialized for Modbus gateways. Indicates a misconfigured gateway
        exGWTargetUnresponsive = 11 //!< Gateway Target Device Failed to Respond	Specialized for Modbus gateways. Sent when slave fails to respond
    };

    /*! @brief eDataDir - data transfer direction enumeration */
    enum eDataDir
    {
        dataRead,
        dataWrite
    };

    /*! @brief class exRuntime - runtime exception class */
    class exRuntime
    {
    private:
        exCode m_codeEx;
    public:
        exRuntime( exCode codeEx ) : m_codeEx( codeEx ){} //!< Constructor from exception code
        exCode GetCode( ) const { return m_codeEx; }      //!< Exception code retrieve 
    };

    /*! @brief eFunctionCode - Modbus function codes */
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
    uint8_t         m_arrbPDU[nPDU]; //!< the PDU transfer buffer
    IPDUAdapter*    m_pAdapter;      //!< the PDU adapter to use
    uint8_t         m_uDeviceID;     //!< the Modbus device address associated with our device
public:
    MBUSTiny();                      //!< default constructor; shound not be used, bt we avoid using delete keyword here
    MBUSTiny(IPDUAdapter*, uint8_t); //!< adapter and device address constructor
    virtual ~MBUSTiny();

    bool TranscievePDU();            //!< sends and receives PDUs and dispatches the callabck calls; shoud be called in loop()    

protected:
    // default get/set processing; the methods are overrided in derived classes
    virtual exCode DIInputs( int nIndex, int& rbValue );
    virtual exCode DIOCoils( int nIndex, int& rbValue, eDataDir dirData );
    virtual exCode DIRegisters( int nIndex, uint8_t* pbValue);
    virtual exCode DIOHoldingRegs( int nIndex, uint8_t* pbValue, eDataDir dirData );
private:
    // Modbus functions implementation
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
    rbValue = -1;\
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
