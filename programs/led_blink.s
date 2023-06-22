movz w10, #0
movz w11, #1
mov w12, w11

// use x not w

main_loop:
	poll_write:
		ldr w0, status_address
	    add w0, w10, w0, lsr #31
	    and w0, w0, w11

	    cmp w0, #0
	    bne poll_write

	load_request:
	    ldr w0, request_address

        movz w1, #32
        movz w2, #0
		str w1, [w0, w2]

        movz w1, #0
        movz w2, #4
		str w1, [w0, w2]

        movz w1, #0x00038041
        movz w2, #8
		str w1, [w0, w2]

        movz w1, #8
        movz w2, #12
		str w1, [w0, w2]

        movz w1, #0
        movz w2, #16
		str w1, [w0, w2]

        movz w1, #130
        movz w2, #20
		str w1, [w0, w2]

        movz w2, #24
		str w12, [w0, w2]
	  	eor w12, w12, w11

        movz w1, #0
        movz w2, #28
		str w1, [w0, w2]

	write:
	    ldr w0, request_address
		ldr w1, led_channel
	  	orr w0, w0, w1

		ldr w1, write_address
		str w0, [w1]

	poll_read:
	    ldr w0, status_address
		add w0, w0, #0, lsl #30
	    and w0, w0, w11

	    cmp w0, #0
	    bne poll_read

	read:
	    ldr w0, read_address

	wait:
		ldr w0, wait_loops

		wait_loop:
		    sub w0, w0, #1
			cmp w0, #0
		    bne wait_loop

	b main_loop

led_pin:
    .int    130

led_channel:
    .int    8

read_address:
    .int    0x3f00b880

write_address:
    .int    0x3f00b8A0

status_address:
    .int    0x3f00b8b8

wait_loops:
    .int    1000000

request_address:
    .int    1000