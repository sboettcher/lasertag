


// IR protocol definitions.
#define IR_BIT_LENGTH_US 10000
#define IR_NUM_BITS (8 + 2)
#define IR_MASK_PARITY_STOP 0x03

// TODO(Jan): evtl wegen UART auf T1 wechseln.
// Initialize timer 0 to count to 10000 (10ms) in up-mode (MC_1) and enable compare interrupt.
#define IR_START_TIMER TA0CCR0 = (IR_BIT_LENGTH_US - 1); TA0CCTL0 = CCIE; TA0CTL = TASSEL_2 | ID_3| MC_1;

// Stop timer and reset the count.
#define IR_STOP_TIMER TA0CTL = TACLR;

// Enable / disable the interrupt on P1.6 which detects the start bit.
#define IR_ENABLE_INTERRUPT P1IE |= BIT6; P1IES |= BIT6; P1IFG = 0x00;
#define IR_DISABLE_INTERRUPT P1IE &= ~BIT6; P1IES &= ~BIT6; P1IFG = 0x00;
