#include <inttypes.h>
#include "hardware/ButtonDebouncer.h"
#include "hardware/ButtonDeltaDetector.h"
#include "device/HardwareController.h"
#include "comms/MasterNotifier.h"
#include "comms/SlaveNotifier.h"
#include "comms/I2CCommunicator.h"
#include "comms/USBCommunicator.h"
#include "keys/KeyMapper.h"
extern "C"{
#include <avr/io.h> 
#include <util/delay.h>
}

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

int main()
{
	CPU_PRESCALE(0);
    _delay_ms(5); //Give everything time to power up
    
    I2CCommunicator i2c_communicator; //Start I2C in slave mode
    USBCommunicator usb_communicator; //Start USB hardware
    
    //Holds information from the master about the state of the indicator LEDs
    uint8_t                 led_status = 0;
    //Manages hardware functions, including scanning keys. Constructor configures hardware.
    HardwareController      hardware;
    //Manages debouncing buttons. The argument is the number of samples needed to detect a change in state.
    ButtonDebouncer         debouncer(3);
    //Detects changes in button states.
    ButtonDeltaDetector     push_detector;
    //Maps physical button deltas to USB key deltas. This is where to look if you want to change the key layout.
    KeyMapper               key_mapper;
    //Talks to the computer over USB or the masterboard over I2C. Constructor hangs until a master is found (either USB or I2C)
    MasterNotifier          master(usb_communicator, i2c_communicator);
    //Represents the slave, if one exists. Acts as a dummy slave otherwise. Tries to connect to slave during construction.
    SlaveNotifier           slave(i2c_communicator);
    
    
    for(;;){
        ButtonsState raw_state = hardware.update(led_status);
        ButtonsState debounced_state = debouncer.update(raw_state);
        ButtonsDelta button_changes = push_detector.update(debounced_state);
        KeysDelta key_changes = key_mapper.resolve(button_changes);
        
        KeysDelta slave_key_changes = slave.update(led_status); //If we don't have a slave, this returns a KeysDelta full of zeros
        
        led_status = master.notify(key_changes, slave_key_changes); //Sends all key press/releases to the USB or I2C master
    }
    return 0;
}
