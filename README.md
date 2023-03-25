# mbtiny

Tiny Modbus for Arduino / embedded systems

Note: before you consider using this library please take a look at the *Limitations* section.

# Your feedback matters

I provide this code to you in hope you'll find it useful without seeking any profit or other benefits. So if you find it useful please drop a comment or hit the star button above.


## Motivation

A few years ago I had to do a side project for building automation, centered around Modbus-operated ATmega328p actuator controllers.
I did some research on the publicly available Modbus implementations, but none of them suited the small memory footprint that the library has to maintain.

To reduce the RAM requirements I had to move as much as possible data structures into the code.  This includes implementing the Modbus register logic as series of conditional operators and CRC calculation as code-based finite-state machine.

Since the initial application didn't require runtime registers allocation or register address change, this approach was sound.

## How to use it

The library is designed with extensibility in mind, it is not tailored to specific message transport. It was used in practice with Maxim MAX 3100 UART, so I provide the MAX code as implementation example. You can find the implementation in the `ibaylov/max3100-485`

### Outline

To use `mbtiny` you have to 

* subclass `IPDUAdapter` for your specific serial/network transport 
or
* use the supplied `MBRTUAdapter` and subclass `IRTUTransport`. You can see a specific implementation for MAX 3100 in `MAXTransport` files
* subclass `MBUSTiny` to implement register routine callback methods
* instantiate your `IPDUAdapter` subclass 
* use the `IPDUAdapter` subclass instance with your `MBUSTiny` subclass instance

For our MAX 3100 transport this may look like:

~~~
MAXTransport transMax( 9600 /* bps */, 9 /* CS pin index */ );
MBRTUAdapter rtu( &transMax );
MBT mb( &rtu, 42 /* our device address */ );

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  mb.TranscievePDU(); // we are pumping the incoming messages here
}
~~~

The macro `MB_DECLARE( )` should be used in subclass declaration to declare specific dispatch routines

~~~
class MBT : public MBUSTiny
{
  public:
...  
    MB_DECLARE( )
};
~~~

* add callback mapping macros in your class implementation:

~~~
MB_BEGIN_INPUTS( MBT, MBUSTiny )
  MB_INPUT( 42, readLed )
MB_END_INPUTS( MBT, MBUSTiny )

MB_BEGIN_COILS( MBT, MBUSTiny )
  MB_COIL( 44, ioPIN, ioPIN )
  MB_COIL_WO( 48, ioPIN2 )
MB_END_COILS( MBT, MBUSTiny )

MB_BEGIN_REGISTERS( MBT, MBUSTiny )
  MB_REGISTER( 381, ioReg )
MB_END_REGISTERS( MBT, MBUSTiny )

MB_BEGIN_HOLDINGREGS( MBT, MBUSTiny )
  MB_HOLDINGREG( 1381, ioHoldReg, ioHoldReg)
MB_END_HOLDINGREGS( MBT, MBUSTiny )
~~~

### Callback methods

#### Inputs and coils

The input and coil callbacks are implemented as two argument class methods returning MBUSTiny::exCode 

~~~
class MBT : public MBUSTiny
{
...
    MBUSTiny::exCode readLed( int nIndex, int&  nValue )
    { 
        int nMagic = 0;
        // do some magic here
        nValue = nMagic;
        return exOK;
    }  
~~~

In our implementation code we add a mapping for the callback

~~~
MB_BEGIN_INPUTS( MBT, MBUSTiny )
  MB_INPUT( 42 /* Input index */, readLed  /* Method name */ )
MB_END_INPUTS( MBT, MBUSTiny )
~~~

The method takes an index as first argument so a single routine can be used to serve multiple inputs or coils.

The coil mapping takes an index ant two routines - for getting and setting the value. A single routine can be used, in this case
one should check if the second argument is less than zero meaning 'get'

~~~
MB_BEGIN_COILS( MBT, MBUSTiny )
  MB_COIL( 44, ioPIN, ioPIN )
  MB_COIL_WO( 48, ioPIN2 )
MB_END_COILS( MBT, MBUSTiny )
~~~

The macro `MB_COIL_WO` is used for write-only coils, it has only two arguments - an index and a setter routine

#### Registers and hold registers

The register and hold register routines are implemented as two-argument methods returning `MBUSTiny::exCode`

~~~
class MBT : public MBUSTiny
{
...
    MBUSTiny::exCode ioReg( int nIndex, uint8_t* pbData )
    { 
        // read some data into pbData
        return exOK;
    }  
~~~

The Modbus register is 16bit Big-Endian, we pass the pointer to the corresponding transfer buffer data.

Mapping of a register follows the same pattern as for inputs and coils:

~~~
MB_BEGIN_REGISTERS( MBT, MBUSTiny )
  MB_REGISTER( 381, ioReg )
MB_END_REGISTERS( MBT, MBUSTiny )
~~~

As for the holding registers, their handling is similar to coils:

~~~
MB_BEGIN_HOLDINGREGS( MBT, MBUSTiny )
  MB_HOLDINGREG( 1381 /* Holding register address */ , ioHoldReg /* Getter */, ioHoldReg /* Setter */ )
MB_END_HOLDINGREGS( MBT, MBUSTiny )
~~~

The `MB_HOLDINGREG_WO` macro is used to map a write-only holding register

~~~
MB_BEGIN_HOLDINGREGS( MBT, MBUSTiny )
  MB_HOLDINGREG( 1381 /* Holding register address */ , ioHoldReg /* Getter */, ioHoldReg /* Setter */ )
  MB_HOLDINGREG_WO( 2381 /* Holding register address */ , ioHoldRegSet /* Setter */ )
MB_END_HOLDINGREGS( MBT, MBUSTiny )
~~~

* Note that the holding register output operations are not implemented yet, see *Limitations*

### Code documentation

Go to `doc` directory and run `doxygen` to generate the code documentation

### Limitations

1. Because of using a series of conditional operators for the address-to-callback mapping it is not possible to modify the inputs, coils and register addresses at run time.
Although it is a rare requirement, it should be mentioned that `mbtiny` is not capable of doing that.

2. The Modbus functions that were not needed for the initial task were not implemented. The list of missing functions follows:

* Write Single Holding Register (function code 6)
* Write Multiple Coils (function code 15 )
* Write Multiple Holding Registers (function code 16)

I'll implement them for the sake of completeness as soon as I have some time slot for doing that.  If you manage to implement them meanwhile, I'll happily merge your code. Be my guest :) 

3. Inputs, coils and registers indexes used as macro arguments are PDU-aware zero-based; one may prefer traditional Modbus location representation, please let me know if you indeed prefer this.
