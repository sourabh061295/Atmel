#include <avr/io.h>
#include <util/delay.h>
int main(void)
{
    uint8_t var;
    var |= 0x08;
    var |= 0x50;
    var |= (1<<4) | (1<<6);
    var &= ~(1<<2) & ~(1<<7);
    var ^= (1<<3);
    var |= (0x04) & (~(1<<5) & ~(1<<7));
    var= (var & 1<<3);
}