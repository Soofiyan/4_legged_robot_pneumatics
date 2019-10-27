/*
 * 2K19_AR.c
 *
 * Created: 4/3/2019 4:37:45 PM
 * Author : Lenovo
 */
#define F_CPU 14745600UL
#include "avr_compiler.h"

#define leg DDRJ
#define joint DDRA
#define readSwitch DDRE
#define led DDRK
#define button DDRB

#define legPort PORTJ
#define jointPort PORTA
#define readSwitchPort PORTE
#define ledPort PORTK
#define buttonPort PORTB

// #define backLegLow legPort |= (1<<0)
// #define backLegHigh legPort &= ~(1<<0)
// #define frontLegLow legPort |= (1<<1)
// #define frontLegHigh legPort &= ~(1<<1)
//
// #define frontJointLow jointPort |= (1<<0)
// #define frontJointHigh jointPort &= ~(1<<0)
// #define backJointLow jointPort |= (1<<1)
// #define backJointHigh jointPort &= ~(1<<1)

#define backLegLow legPort &= ~(1<<0)
#define backLegHigh legPort |= (1<<0)
#define frontLegLow legPort &= ~(1<<1)
#define frontLegHigh legPort |= (1<<1)

#define frontJointLow jointPort &= ~(1<<0)
#define frontJointHigh jointPort |= (1<<0)
#define backJointLow jointPort &= ~(1<<1)
#define backJointHigh jointPort |= (1<<1)

#define frontUp ((readSwitchPort & 0x20))
#define backUp ((readSwitchPort & 0x04))
#define frontDown ((readSwitchPort & 0x40))
#define backDown ((readSwitchPort & 0x08))


#define BAUD 9600
#define BAUDRATE  ((F_CPU/(BAUD*16UL)-1))

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

int xj1=0,yj1=0,xj2=0,yj2=0,pot1=0,pot2=0,pot3=0,pot4=0;                            //analog values(serially received from remote);
int butt[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};                                    //digital values(serially received from remote);

uint8_t RX[16]={100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
int RX_range=200,RX_raw=255,RX_ad=255,RX_count=0;
uint8_t TX[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
int flag[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t TX_raw=200,TX_ad=201,TX_flag=0;
uint8_t start = 0;
int hop1=0,fow1=0;
int current=0,previous=0;
int count=0, pronk = 0, bg1=0, bg2=0, nw=0 ,ju1=0 ,j2=0 ,j3=0, sdu=0, opj1=0, sdd=0, sandCount=0,m=0;

void init();
void foward();
void receive();
void serialstart_3();
void startPos();
void hop();
void interrup();
void pronking();
void borseGait1();
void borseGait2();
void nw_gait();
void jump1();
void jump2();
void jump3();
void sandDuneup();
void sandStart();
void opjump1();
void sandDunedown();
void tussock1_1();
void tussock1_2();
void tussock2_1();
void tussock2_2();
void mountain();

int main(void)
{
    sei();
    init();
    //interrup();
    serialstart_3();
    startPos();
    /* Replace with your application code */
    //PORTA = 0x00;
    while (1)
    {
        if(hop1)
        {
            hop();
        }
        if(fow1)
        {
            foward();
        }
        if(pronk)
        {
            pronking();
        }
        if(bg1)
        {
            borseGait1();
        }
        if (bg2)
        {
            borseGait2();
        }
        if (nw)
        {
            nw_gait();
        }
        if (ju1)
        //if(PINB==(1<<0))
        {
            jump1();
        }
        //if (j2)
//         if(PINB==(1<<1))
//         {
//             jump2();
//         }
        //if (j3)
//         if(PINB==(1<<2))
//         {
//             jump3();
//         }
        if(sdu)
        //if(PINB==(1<<3))
        {
            sandDuneup();
        }
        if(opj1)
        //if(PINB==(1<<4))
        {
            opjump1();
        }
//         if(ss)
//         //if(PINB==(1<<5))
//         {
//             sandStart();
//         }
        if(sdd)
        //if(PINB==(1<<6))
        {
            sandDunedown();
        }
        if (m)
        {
            mountain();
        }
//         if (PINB==(1<<7))
//         {
//             bg1^=1;
//         }
//         if (count==10)
//         {
//             hop1=0;
//             fow1=0;
//             startPos();
//             count=0;
//         }
        //hop();
        //foward();
        receive();
        //ledPort=PINB;
        //ledPort = PINE;
        //j0 j1
        
    }
}

void interrup()
{
    TCCR1B=1<<WGM12 | 1<<CS10 | 1<<CS12;
    TIMSK1= 1<<OCIE1A;
    OCR1A=720;
}

void pronking()
{
    frontJointHigh;
    backJointLow;
    _delay_ms(80);
    hop();
    frontJointLow;
    backJointHigh;
    _delay_ms(150);
}
void init()
{
    leg=0xFF;
    joint=0xFF;
    readSwitch=0x00;
    led=0xFF;
    button=0x00;
}

void startPos()
{
    frontJointHigh;
    backJointLow;
    frontLegHigh;
    backLegHigh;
}

void foward()
{
    backLegHigh;
    _delay_ms(200);
    //while(backDown);
    backLegLow;
    frontJointHigh;
    backJointHigh;
    //while(backUp);
    _delay_ms(200);
    frontLegHigh;
    _delay_ms(200);
    //while(frontDown);
    frontLegLow;
    frontJointLow;
    backJointLow;
    _delay_ms(200);
    //while(frontUp);
}

void hop()
{
//     backLegHigh;
//     frontLegHigh;
//     while(frontDown || backDown);
//     backLegLow;
//     frontLegLow;
//     while(frontUp || backUp);
PORTJ = 0x00;
_delay_ms(200);
PORTJ = 0xFF;
_delay_ms(200);
}

void borseGait1()
{
    frontLegLow;
    _delay_ms(25);
    backLegLow;
    _delay_ms(105);
    frontLegHigh;
    _delay_ms(25);
    backLegHigh;
    _delay_ms(105);
}
void borseGait2()
{
    backLegLow;
    _delay_ms(25);
    frontLegLow;
    _delay_ms(105);
    backLegHigh;
    _delay_ms(25);
    frontLegHigh;
    _delay_ms(105);
}
void nw_gait()
{
    backJointHigh;
    frontJointLow;
    _delay_ms(200);
    backLegLow;
    frontLegLow;
    _delay_ms(150);
    backLegHigh;
    frontLegHigh;
    _delay_ms(75);
    backJointLow;
    frontJointHigh;
    _delay_ms(200);
}

void jump1()
{
    frontLegLow;
    _delay_ms(25);
    backLegLow;
    _delay_ms(175);
    frontLegHigh;
    _delay_ms(25);
    backLegHigh;
    _delay_ms(150);
    backLegLow;
    _delay_ms(80);
    frontJointLow;
    ju1=0;

    //_delay_ms(1000);
    //startPos();
    //_delay_ms(1000);
//     frontLegLow;
//     _delay_ms(50);
//     backLegLow;
//     _delay_ms(150);
//     frontLegHigh;
//     _delay_ms(50);
//     backLegHigh;
//     _delay_ms(150);
//     backLegLow;
//     _delay_ms(60);
//     frontJointLow;
//     ju1=0;
}

void sandDuneup()
{
    frontJointHigh;
    _delay_ms(200);
    frontLegLow;
    backLegHigh;
    _delay_ms(50);
    frontLegHigh;
    backLegLow;
    _delay_ms(50);
}

void sandStart()
{
    backLegHigh;
    frontLegLow;
    backJointLow;
    frontJointHigh;
}

void opjump1()
{
    frontLegLow;
    _delay_ms(50);
    backLegLow;
    _delay_ms(150);
    frontLegHigh;
    _delay_ms(50);
    backLegHigh;
    _delay_ms(150);
    frontLegLow;
    _delay_ms(60);
    backJointHigh;
    backLegLow;
    _delay_ms(40);
    backLegHigh;
    opj1=0;
    //_delay_ms(200);
    
}

void sandDunedown()
{
    //frontLegHigh;
    backLegLow;
    //_delay_ms(30);
    //frontLegLow;
    _delay_ms(150);
    backLegHigh;
    _delay_ms(150);
}

void jump2()
{
    backLegLow;
    _delay_ms(180);
    backLegHigh;
    frontLegLow;
    _delay_ms(100);
    frontLegHigh;
    backLegLow;
//     _delay_ms(500);
//     startPos();
//     _delay_ms(1000);
}

void jump3()
{
    frontLegLow;
    backLegLow;
    _delay_ms(60);
    frontLegHigh;
    backLegHigh;
    _delay_ms(120);
    frontLegLow;
    backLegLow;
    backJointLow;
//     _delay_ms(500);
//     startPos();
//     _delay_ms(1000);
}

void tussock1_1()
{
    backLegLow;
    frontLegLow;
    _delay_ms(200);
    backLegHigh;
    frontLegHigh;
    _delay_ms(120);
    backLegLow;
    _delay_ms(100);
    frontJointLow;
    _delay_ms(150);
    backLegHigh;
}

void tussock1_2()
{
    backLegLow;
    frontLegLow;
    _delay_ms(200);
    backLegHigh;
    frontLegHigh;
    _delay_ms(120);
    frontLegLow;
    _delay_ms(100);
    backJointHigh;
    _delay_ms(150);
    frontLegHigh;
}

void tussock2_1()
{
            backLegLow;
            _delay_ms(50);
            frontLegLow;
            _delay_ms(150);
            backLegHigh;
            _delay_ms(50);
            frontLegHigh;
            _delay_ms(150);
            backLegLow;
            _delay_ms(60);
            frontJointLow;
            frontLegLow;
            _delay_ms(40);
            backLegHigh;
            frontLegHigh;
}

void tussock2_2()
{
        frontLegLow;
        _delay_ms(50);
        backLegLow;
        _delay_ms(150);
        frontLegHigh;
        _delay_ms(50);
        backLegHigh;
        _delay_ms(150);
        frontLegLow;
        _delay_ms(60);
        backJointHigh;
        backLegLow;
        _delay_ms(40);
        backLegHigh;
        frontLegHigh;
}

void mountain()
{
    frontLegLow;
    backLegLow;
    _delay_ms(80);
    frontLegHigh;
    //backLegLow;
    _delay_ms(80);
}

void sahil_gait()
{
    backLegLow;
    backJointHigh;
    frontJointHigh;
    _delay_ms(200);
    backLegHigh;
    backJointHigh;
    frontJointHigh;
    _delay_ms(200);
    frontLegLow;
    backJointHigh;
    frontJointHigh;
    _delay_ms(200);
    frontLegHigh;
}
void serialstart_3()
{
    UBRR2H=BAUDRATE>>8;
    UBRR2L=BAUDRATE;
    UCSR2B=0b10011000;//enable RXEN TXEN
    UCSR2C=0b00000110;// UCSZ1 UCSZ0
}

ISR(USART2_RX_vect)
{
    RX_count=1;
    RX_raw=UDR2;
    current=RX_raw;
    if ((RX_raw>200) && (RX_raw<255))                    //201 to 216 for addresses of analog values, 231 to 246 for buttons;
    {
        RX_ad=RX_raw;
        //        if ((RX_raw>210) && (RX_raw<227))
        //        {
        //            uint8_t r_temp0=(RX_raw-211);
        //            butt_rel[r_temp0]=1;
        //        }
        if ((RX_raw>230) && (RX_raw<247))
        {
            uint8_t r_temp0=(RX_raw-231);
            butt[r_temp0]=1;
        }
    }
    else if ((RX_raw>=0) && (RX_raw<201))
    {
        uint8_t r_temp1=(RX_ad-201);
        if (r_temp1<16)
        {
            RX[r_temp1]=RX_raw;
        }
    }
    previous=current;
}

void receive()
{
//     yj1=map_value(RX[0],0,RX_range,(-pwm_range),pwm_range);
//     xj1=map_value(RX[1],0,RX_range,pwm_range,(-pwm_range));
//     yj2=map_value(RX[2],0,RX_range,(-pwm_range),pwm_range);
//     xj2=map_value(RX[3],0,RX_range,pwm_range,(-pwm_range));
    
    if (butt[PS_START]==1)
    {
        startPos();
        butt[PS_START]=0;
    }
    if (butt[PS_SELECT]==1)
    {
        //hop1 ^=1;
        sandStart();
        butt[PS_SELECT]=0;
    }
    if (butt[PS_UP]==1)
    {
        //fow1 ^=1;
        //hop1 ^= 1;
        //backLegHigh;
        bg1^=1;
        butt[PS_UP]=0;
    }
    if (butt[PS_DOWN]==1)
    {
        //pronk^=1;
        bg2^=1;
        butt[PS_DOWN]=0;
    }
    if (butt[PS_LEFT]==1)
    {
        backJointLow;
        frontJointHigh;
        butt[PS_LEFT]=0;
    }
    if (butt[PS_RIGHT]==1)
    {
        backJointHigh;
        frontJointLow;
        
        butt[PS_RIGHT]=0;
    }
    if (butt[PS_SQUARE]==1)
    {
        sdd^=1;
        butt[PS_SQUARE]=0;
    }
    if (butt[PS_CIRCLE]==1)
    {
        sdu^=1;
        butt[PS_CIRCLE]=0;
    }
    if (butt[PS_TRIANGLE]==1)
    {
        ju1^=1;
        butt[PS_TRIANGLE]=0;
    }
    if (butt[PS_CROSS]==1)
    {
        opj1^=1;
        butt[PS_CROSS]=0;
    }
    if (butt[PS_L1]==1)
    {
        tussock1_1();
        //bg1 ^=1;
        butt[PS_L1]=0;
    }
    if (butt[PS_R1]==1)
    {
        tussock2_1();
        //bg2^=1;
        butt[PS_R1]=0;
    }
    if (butt[PS_L2]==1)
    {
        tussock1_2();
        //nw ^=1;
        butt[PS_L2]=0;
    }
    if (butt[PS_R2]==1)
    {
        tussock2_2();
        //j3^=1;
        butt[PS_R2]=0;
    }
    if (butt[PS_L3]==1)
    {
        m^=1;
        //ju1^=1;
        butt[PS_L3]=0;
    }
    if (butt[PS_R3]==1)
    {
        //j2^=0;
        butt[PS_R3]=0;
    }
}

// ISR(TIMER0_COMPA_vect)
// {
//     if(current==previous)
//     {
//         count++;
//     }
//     else
//     {
//         count=0;
//     }
// }
