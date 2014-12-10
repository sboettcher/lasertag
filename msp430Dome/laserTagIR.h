

// IR protocol definitions.
#define IR_BIT_LENGTH_US 1000
#define IR_NUM_BITS (8 + 2)

// TODO(Jan): evtl wegen UART auf T1 wechseln.
// Initialize timer 0 to count to 10000 (10ms) in up-mode (MC_1) and enable compare interrupt.
// Select SMCLK (TASSEL_2) and devide by 8 (ID_3) -> 1MHz counts 1 us.
#define IR_START_TIMER TA1CCR0 = (IR_BIT_LENGTH_US - 1); TA1CCTL0 = CCIE; TA1CTL = TASSEL_2 | ID_3| MC_1;

// Stop timer and reset the count.
#define IR_STOP_TIMER TA1CTL = TACLR;

// Enable / disable the interrupt on P1.6 which detects the start bit.
#define IR_ENABLE_INTERRUPT P1IE |= BIT6; P1IES |= BIT6; P1IFG = 0x00;
#define IR_DISABLE_INTERRUPT P1IE &= ~BIT6; P1IES &= ~BIT6; P1IFG = 0x00;
