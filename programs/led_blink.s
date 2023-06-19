// Get GPIO out of reset
// IO_BANK in reset
// Clear

/*
Define address
Load
Move
Store

Load
Load
Compare
Branch with Eqaul
*/


// Assign led pin to SIO
//

// r0 as mailbox register
// r3 as status

/*
Time delay:

Create a loop.
Subtract 1 from a large number.
Compare with 0.
Jump (branch).
Keep looping until it is zero.
*/

/*
Send a message to mailbox
Containing address of request buffer
And tag to power on LED
*/

/*
Wait (aka loop)
*/

/*
Send a message to mailbox
Containing address of request buffer
And tag to power on LED
*/

/*
Wait (aka loop)
*/

/* todo: load as 32 bits */
/*
peripheral base register = 0x3f000000
mailbox register offset  = 0xb880
read = 0x00
write = 0x20
status = 0x38
E = read-empty
F = write-full
LED channel = 0b1000

When do you have to to read from memory
Where to store requests in memory

1 Turn on light
  1 Wait for write flag to be clear
  2 Copy the on request to the address
  3 Store the on message and address to the write register
  4 Wait for read flag to be clear
  5 Read from read register and discard result
2 Wait 1 second
  1 Initialise register with 1000
  2 Subtract 1
  3 Compare to 0
  4 Branch if positive to 2.1
3 Turn off light (see 1)
4 Wait 1 second (see 2)
5 Jump to 1
*/



/* START */

mov w10, 0

main_loop:
	poll_write:
		# Fetch F flag
	    ldr w0, status_address
	    lsr w0, w0, 31
	    and w0, w0, 1

	 	# Ensure not set
	    cmp w0, 0
	    bne poll_write

	load_onoff_request:
		ldr w0, request_address
		ldr w1, on_request

	 	# Load request into memory
		ldr w2, 0 # tag index used in load_loop

		load_loop:
	   		ldr w3, [w1, w2]
		 	str w0, w3
			add w0, w0, 4
			add w2, w2, 4

	  		# Loop if tag index less than 32 bytes
	 		cmp w2, 32
			bl load_loop

		# Load on/off flag into correct request tag
  		ldr w0, request_address
		add w0, w0, 24
	 	str w0, w10
	  	eor w10, w10, 1 # flip on/off register

	write:
		# w0 = request message
		ldr w0, request_address
	  	orr w0, led_channel

		# Store message to write register
	    ldr w1, write_address
		str w1, [w0]

	poll_read:
		# Fetch E flag
	    ldr w0, status_address
	    lsr w0, w0, 30
	    and w0, w0, 1

	 	# Ensure not set
	    cmp w0, 0
	    bne poll_read

	read:
		# Read register and discard result
	    ldr w0, read_address
		# todo

	wait:
		ldr w0, wait_loops

		wait_loop:
		    sub w0, w0, 1
			cmp w0, 0
		    bne wait_loop

	br main_loop

/* Constants */

led_channel:
    .int    0x8

base_mailbox_address:
    .int    0x3f00b880

read_address:
    .int    0x3f00b880

write_address:
    .int    0x3f00b8A0

status_address:
    .int    0x3f00b8b8

wait_loops:
    .int    1000000

request_address:
	.int 	todo (16 byte aligned so end in 0x0)

onoff_request:
	.int	32
 	.int	0
  	.int	0x00038041
	.int	8
	.int	0x0
	.int	130
	.int	0x0 # (24) will be changed to 0 or 1 later
	.int	0x0