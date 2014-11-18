


// IR code reading states.
#define STATE_IDLE 0
#define STATE_READ_DATA 1
#define STATE_CHECK_PARITY 2
#define STATE_WAIT_FOR_STOP 3

// IR protocol definitions.
#define IR_BIT_LENGTH_US 10000
#define IR_NUM_BITS 8

// TODO(Jan): evtl wegen UART auf T1 wechseln.
// Initialize timer 0 to count to 10000 (10ms) in up-mode (MC_1) and enable compare interrupt.
#define IR_START_TIMER {TA0CCR0 = (IR_BIT_LENGTH_US - 1); TA0CCTL0 = CCIE; TA0CTL = TASSEL_2 | ID_3| MC_1; }

// Stop timer and reset the count.
#define IR_STOP_TIMER TA0CTL = TACLR;
