/*
 *  Copyright 2014 Joel Stanley <joel@jms.id.au>
 *
 *  HAB flight computer using ChibiOS
 *
 */

#include <ch.h>
#include <hal.h>

#if defined(SEMIHOSTING)
#include <stdio.h>
#else
#define printf(...) do {} while(0)
#endif

static WORKING_AREA(led_blinker_stack, 128);
static msg_t __attribute__((noreturn)) led_blinker(void *arg)
{
    (void)arg;

    chRegSetThreadName("led_blinker");

    while (TRUE) {
        palClearPad(GPIOC, GPIOC_LED4);
        chThdSleepMilliseconds(500);

        palSetPad(GPIOC, GPIOC_LED4);
        chThdSleepMilliseconds(500);
    }
}

void __attribute__((noreturn)) main(void)
{
    halInit();
    chSysInit();

    chThdCreateStatic(led_blinker_stack, sizeof(led_blinker_stack),
            NORMALPRIO, led_blinker, NULL);

    printf("Entering mainloop\n");

    while (1) {
        /* Press the blue USER button to light LD3. */
        if (palReadPad(GPIOA, GPIOA_BUTTON))
            palSetPad(GPIOC, GPIOC_LED3);

        chThdSleepMilliseconds(250);
        palClearPad(GPIOC, GPIOC_LED3);
    }
}
