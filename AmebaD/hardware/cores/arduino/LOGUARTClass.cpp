/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LOGUARTClass.h"

#define LOG_UART_MODIFIABLE_BAUD_RATE 0

#ifdef __cplusplus
extern "C" {
#endif

#include "serial_api.h"
#include "rtl8721d_vector.h"

serial_t log_uart_obj;

#ifdef __cplusplus
}
#endif

RingBuffer rx_buffer0;

void arduino_loguart_irq_handler(uint32_t id, SerialIrq event)
{
    char c;
    RingBuffer *pRxBuffer = (RingBuffer *)id;

    if (event == RxIrq) {
        c = char(serial_getc(&log_uart_obj));
        pRxBuffer->store_char(c);
    }
}

LOGUARTClass::LOGUARTClass(int dwIrq, RingBuffer* pRx_buffer )
{
    _rx_buffer = pRx_buffer;
    _dwIrq = dwIrq;
}

// Protected Methods //////////////////////////////////////////////////////////////




// Public Methods //////////////////////////////////////////////////////////////


void LOGUARTClass::IrqHandler( void )
{

    uint8_t     data = 0;
    BOOL    	PullMode = _FALSE;

    //UART_TypeDef * pLOG_UART = UART2_DEV;
    uint32_t 	IrqEn;
	IrqEn = UART_IntStatus(UART2_DEV);

    //DiagSetIsrEnReg(0);

	serial_irq_set(&log_uart_obj, RxIrq, 0);
	//serial_irq_set(&log_uart_obj, TxIrq, 0);

    data = DiagGetChar(PullMode);
	if ( data > 0 ) 
		_rx_buffer->store_char(data);

    //DiagSetIsrEnReg(IrqEn);
	serial_irq_set(&log_uart_obj, RxIrq, IrqEn);

}


void LOGUARTClass::begin( const uint32_t dwBaudRate )
{

    serial_init(&log_uart_obj, PA_7, PA_8);
    serial_format(&log_uart_obj, 8, ParityNone, 1);

#if LOG_UART_MODIFIABLE_BAUD_RATE
    /* log uart initialize in 38400 baud rate.
     * If we change baud rate here, Serail Monitor would not detect this change and show nothing on screen.
     */
    serial_baud(&log_uart_obj, dwBaudRate);
#else
    serial_baud(&log_uart_obj, 38400);
#endif
    serial_irq_set(&log_uart_obj, RxIrq, 1);
    serial_irq_handler(&log_uart_obj, arduino_loguart_irq_handler, (uint32_t)_rx_buffer);
}

void LOGUARTClass::end( void )
{
    // clear any received data
    _rx_buffer->_iHead = _rx_buffer->_iTail ;

    serial_free(&log_uart_obj);
}

int LOGUARTClass::available( void )
{
  return (uint32_t)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
}

int LOGUARTClass::peek( void )
{

  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
    return -1 ;

  return _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;

}

int LOGUARTClass::read( void )
{
  // if the head isn't ahead of the tail, we don't have any characters
  if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
    return -1 ;

  uint8_t uc = _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;
  _rx_buffer->_iTail = (unsigned int)(_rx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;
  return uc ;

}

void LOGUARTClass::flush( void )
{
// TODO: 
// while ( serial_writable(&(this->sobj)) != 1 );
/*
  // Wait for transmission to complete
  while ((_pUart->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY)
    ;
*/
}

size_t LOGUARTClass::write( const uint8_t uc_data )
{
    serial_putc(&log_uart_obj, uc_data);
  	return 1;
}

LOGUARTClass Serial(UART_LOG_IRQ, &rx_buffer0);

bool Serial_available() {
    return Serial.available() > 0;
}
