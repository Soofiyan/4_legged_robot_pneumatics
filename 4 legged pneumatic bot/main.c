#define F_CPU 14745600UL
#define BAUD 9600
#define BAUDRATE ((F_CPU/(BAUD*16UL)-1))

#define PS_L1 0
#define PS_R1 1
#define PS_L2 2
#define PS_R2 3
#define PS_L3 4
#define PS_R3 5
#define PS_TRIANGLE 6
#define PS_SQUARE 7
#define PS_CROSS 8
#define PS_CIRCLE 9
#define PS_UP 10
#define PS_LEFT 11
#define PS_DOWN 12
#define PS_RIGHT 13
#define PS_START 14
#define PS_SELECT 15


#include "avr_compiler.h"


uint8_t butt[16],RX_raw = 0,RX_ad1 = 0,RX[4],pwm_range = 255,RX_range = 200;
int xj1=0,yj1=0,xj2=0,yj2=0,x_vect=0,y_vect =0;
bool up,down,left,right,start;
uint8_t val_sens = 0;
bool next_pair = 0;


void pneumatic_init()
{
    DDRA = 0xFF;        //for hip pneumatics 0 --> left_front,right_back; 1 --> right_front,left_back
    DDRB = 0xFF;        //for foot pneumatics 0 --> left_front,right_back; 1 --> right_front,left_back
    DDRD = 0x00;        //for ir/reed switch 0 --> left_front, 1 --> right_front,
                        //                    2 --> left_back, 3 --> right_back
}

void bluetooth_init()
{
    DDRJ |= (1<<PJ0)|(1<<PJ1);
    PORTJ |= (1<<PJ1)|(1<<PJ0);//bluetooth
}

uint8_t sensor_read()
{
    uint8_t value = PIND & 0x0F;
    return value;
}

void left_front_right_back()
{
    _delay_ms(350);
    PORTA |= (1<<0);
    _delay_ms(800);
//    val_sens = sensor_read();
//    while(val_sens & 0x02);
    PORTB &= ~(1<<0);
    _delay_ms(350);
    PORTB |= (1<<0);
    PORTA |= (1<<1);
    next_pair = 1;
}

void right_front_left_back()
{
    _delay_ms(350);
    PORTA &= ~(1<<1);
    _delay_ms(800);
//    val_sens = sensor_read();
//    while(val_sens & 0x01);
    PORTB &= ~(1<<1);
    _delay_ms(350);
    PORTB |= (1<<1);
    PORTA &= ~(1<<0);
    left_front_right_back();
}

void standing()
{
    PORTA |= (1<<0)&(1<<1);
    PORTB |= (1<<0)|(1<<1);
    _delay_ms(5000);
}

void walking()
{
    if(next_pair == 1)
    {
        next_pair = 0;
        right_front_left_back();
    }
}

void usart_init(void)
{
    UBRR3H=BAUDRATE>>8;
    UBRR3L=BAUDRATE;
    UCSR3B=0b10011000;//enable RXEN TXEN
    UCSR3C=0b00000110;// UCSZ1 UCSZ0
}

long map_value(long in_value, long in_min, long in_max, long out_min, long out_max)
{
    return (in_value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ps2_values(void)
{
    if (butt[10]==1)//up
    {
        up ^=1;
        butt[10] = 0;
    }
    if (butt[11]==1)//left
    {
        left ^= 1;
        butt[11] = 0;
    }
    if (butt[12]==1)//down
    {
        down ^= 1;
        butt[12] = 0;
    }
    if (butt[13]==1)//right
    {
        right ^= 1;
        butt[13] = 0;
    }
    if (butt[14]==1)//start
    {
        start ^= 1;
        butt[14] = 0;
    }
}

int main()
{
    pneumatic_init();
    bluetooth_init();
    usart_init();
    standing();
    while(1)
    {
        if(start == 1)
        {
            walking();
        }
        else
        {
            standing();
        }
    }
}
