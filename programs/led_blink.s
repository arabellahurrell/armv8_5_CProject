/*
 * Code to make RaspberryPi LED flash
 */

// Branch to the start of the program
b start

/*
 * Constants
 */

// On/off request
request:
    .int    32
    .int    0
    .int    0x00038041
    .int    8
    .int    0
    .int    130
    .int    0 // 0 = off, 1 = on
    .int    0

// Address of above 'request' on/off request
// Hardcoded in since only way to access address
// 0x80000 (kernel file start address) + 0x1 (offset from top of file)
constant_request_address:
    .int    0x80001

// Mailbox register addresses
read_address:
    .int    0x3f00b880
read_status_address:
    .int    0x3f00b898
write_address:
    .int    0x3f00b8a0
write_status_address:
    .int    0x3f00b8b8

// Mailbox LED channel
led_channel:
    .int    8

// Halt instruction used for testing
halt:
    and x0, x0, x0

// A large number used to wait between on/off loops
wait_loops:
    .int    1000000

// Address the request is copied to each loop
// This kernel is loaded into address 0x80000
// This is an arbitrary 4-byte aligned address after this address
copy_request_address:
    .int    0x81000

/*
 * Program
 */

start:
    movz w10, #0 // Constant 0
    movz w11, #1 // Constant 1
    mov w12, w11 // Flag to track on/off state of LED

main_loop:
    // Wait for write-full flag to be clear
	poll_write:
		ldr w0, write_status_address
		ldr w1, [w0]
	    add w1, w10, w1, lsr #31
	    and w1, w1, w11

	    cmp w1, #0
	    b.ne poll_write

    // Load on/off message into request address
	copy_request:
	    ldr w0, constant_request_address
	    ldr w1, copy_request_address

        movz w2, w10 // message word index
        movz w3, #4  // 4-byte words

        load_loop:
            ldr w4, [w0, w2]
            str w4, [w1, w2]
            add w2, w2, w3

            cmp w2, #28 // final word index
            b.ne load_loop

        // Set the on/off bit
		str w12, [w1, #24] // index of on/off bit
	  	eor w12, w12, w11  // toggle on/off flag

    // Write our request to the write register
	write_message:
	    ldr w0, copy_request_address
		ldr w1, led_channel
	  	orr w0, w0, w1

		ldr w1, write_address
		str w0, [w1]

    // Wait for read-empty flag to be clear
	poll_read:
	    ldr w0, read_status_address
	    ldr w1, [w0]
		add w1, w10, w1, lsl #30
	    and w1, w1, w11

	    cmp w1, #0
	    b.ne poll_read

    // Read the response from the read register
    // We can discard the response
	read_response:
	    ldr w0, read_address
	    ldr w1, [w0]

    // Wait a period of time before performing loop again
	wait:
		ldr w0, wait_loops

		wait_loop:
		    sub w0, w0, w11
			cmp w0, #0
		    b.ne wait_loop

	b main_loop