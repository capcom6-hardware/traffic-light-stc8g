#define DIGITAL_ON(port, bit) (P##port |= (1 << bit))
#define DIGITAL_OFF(port, bit) (P##port &= ~(1 << bit))
#define DIGITAL_TOGGLE(port, bit) (P##port ^= (1 << bit))
