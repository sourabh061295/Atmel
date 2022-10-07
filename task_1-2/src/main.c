#include <stdint.h>
#include <avr/io.h>

void shortDelay(int delaySec)
{
    uint16_t i, j;
    uint16_t delayHex = delaySec > 134 ? 0xFFFF : 0x01E9 * delaySec;
    for (j = 0; j < delayHex; j++)
    {
        for (i = 0; i < 0xFFFF; i++)
        {
            asm volatile("nop");
        }
    }
}

int main(void)
{
    DDRG |= 0x02;
    while (1)
    {
        PORTG ^= 0x02;
        shortDelay(5);
    }
    return 0;
}