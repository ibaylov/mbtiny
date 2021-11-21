#include <MBusTiny.h>
#include <MBRTUAdapter.h>
#include <MAXTransport.h>
#include "mbt.h"

MAXTransport transMax( 9600, 9 );
MBRTUAdapter rtu( &transMax );
MBT mb( &rtu, 42 );

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  mb.TranscievePDU();
}
