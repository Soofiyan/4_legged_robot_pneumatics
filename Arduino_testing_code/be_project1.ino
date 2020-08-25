#include <avr/interrupt.h>
#include <avr/io.h>

//variable for ps2
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

//varibale for motor
#define left_leg 7
#define right_leg  6
#define right_ext 5
#define left_ext 4
#define USART_BAUDRATE 9600

#define left_leg_in   digitalWrite(left_leg,LOW);
#define left_leg_out  digitalWrite(left_leg,HIGH);
#define right_leg_in  digitalWrite(right_leg,LOW);
#define right_leg_out digitalWrite(right_leg,HIGH);
#define right_ext_in  digitalWrite(right_ext,HIGH);
#define right_ext_out digitalWrite(right_ext,LOW);
#define left_ext_in   digitalWrite(left_ext,LOW);
#define left_ext_out  digitalWrite(left_ext,HIGH);

void motor1();
void motor2();
void manual();
void receive();
void rest_pos_hopping();
void ps2();
long limit_var(long in_var, long l_limit, long h_limit);
long map_value(long in_value, long in_min, long in_max, long out_min, long out_max);

//variable for ps2
uint8_t RX[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
volatile int RX_range = 200, RX_raw = 255, RX_ad = 255, RX_count = 0;
uint8_t TX[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
volatile int flag[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t TX_raw = 200, TX_ad = 201, TX_flag = 0;

int xj1 = 0, yj1 = 0, xj2 = 0, yj2 = 0;
int butt[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int w1 = 0, w2 = 0, w3 = 0;
int js_error = 20;
int pwm_range = 255, s_mode = 0;

void setup()
{
  pinMode(right_leg, OUTPUT);
  pinMode(left_leg, OUTPUT);
  pinMode(right_ext, OUTPUT);
  pinMode(left_ext, OUTPUT);
  UBRR0 = 103; // for configuring baud rate of 9600bps
  UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
  // Use 8-bit character sizes
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  // Turn on the transmission, reception, and Receive interrupt
  sei();// enable global interrupt
}

void loop()
{
  receive();
  switch (s_mode)
  {
    case 0:
      rest_pos();
      break;
    case 1:
      right_ext_out;                    //proncking front side
      left_ext_out;
      left_leg_out;
      right_leg_out;
      delay(300);
      right_ext_out;
      left_ext_out;
      left_leg_in;
      right_leg_in;
      delay(120);
      break;
    case 2:
      right_ext_in;                   //proncking back side
      left_ext_in;
      left_leg_out;
      right_leg_out;
      delay(300);
      right_ext_in;
      left_ext_in;
      left_leg_in;
      right_leg_in;
      delay(120);
      break;
    case 3:
      right_leg_out;                //walking front side
      left_leg_in;
      right_ext_in;
      left_ext_out;
      delay(75);
      right_leg_out;
      left_leg_in;
      right_ext_out;
      left_ext_in;
      delay(75);
      right_leg_out;
      left_leg_out;
      right_ext_out;
      left_ext_in;
      delay(1500);
      right_leg_in;
      left_leg_out;
      right_ext_out;
      left_ext_in;
      delay(75);
      right_leg_in;
      left_leg_out;
      right_ext_in;
      left_ext_out;
      delay(75);
      right_leg_out;
      left_leg_out;
      right_ext_in;
      left_ext_out;    
      delay(1500);
      break;
    case 4:
      right_leg_out;                //walking back side
      left_leg_in;
      right_ext_out;
      left_ext_in;
      delay(75);
      right_leg_out;
      left_leg_in;
      right_ext_in;
      left_ext_out;
      delay(75);
      right_leg_out;
      left_leg_out;
      right_ext_in;
      left_ext_out;
      delay(1500);
      right_leg_in;
      left_leg_out;
      right_ext_in;
      left_ext_out;
      delay(75);
      right_leg_in;
      left_leg_out;
      right_ext_out;
      left_ext_in;
      delay(75);
      right_leg_out;
      left_leg_out;
      right_ext_out;
      left_ext_in;    
      delay(1500);
      break;
    case 5:
      rest_pos_hopping();
      break;
  }
}


void rest_pos()
{
  right_leg_out;
  left_leg_out;
  right_ext_in;
  left_ext_out;
}

void rest_pos_hopping()
{
  right_leg_in;
  left_leg_in;
  right_ext_in;
  left_ext_out;
  delay(110);
  right_leg_out;
  left_leg_out;
  right_ext_in;
  left_ext_out;
  delay(300);
  
}

void receive()
{
  yj1 = map_value(RX[0], 0, RX_range, (-pwm_range), pwm_range);
  xj1 = map_value(RX[1], 0, RX_range, pwm_range, (-pwm_range));
  yj2 = map_value(RX[2], 0, RX_range, (-pwm_range), pwm_range);
  xj2 = map_value(RX[3], 0, RX_range, pwm_range, (-pwm_range));


  if (butt[PS_START] == 1)
  {
    butt[PS_START] = 0;
  }
  if (butt[PS_SELECT] == 1)
  {
    butt[PS_SELECT] = 0;
  }
  if (butt[PS_UP] == 1)
  {
//    s_mode = 1;
    butt[PS_UP] = 0;
  }
  if (butt[PS_DOWN] == 1)
  {
//    s_mode = 2;
    butt[PS_DOWN] = 0;
  }
  if (butt[PS_LEFT] == 1)
  {
//    s_mode = 3;
    butt[PS_LEFT] = 0;
  }
  if (butt[PS_RIGHT] == 1)
  {
//    s_mode = 4;
    butt[PS_RIGHT] = 0;
  }
  if (butt[PS_SQUARE] == 1)
  {
    s_mode = 4;
    butt[PS_SQUARE] = 0;
  }
  if (butt[PS_CIRCLE] == 1)
  {
    s_mode = 2;
    butt[PS_CIRCLE] = 0;
  }
  if (butt[PS_TRIANGLE] == 1)
  {
    s_mode = 1;
    butt[PS_TRIANGLE] = 0;
  }
  if (butt[PS_CROSS] == 1)
  {
    s_mode = 3;
    butt[PS_CROSS] = 0;
  }
  if (butt[PS_L1] == 1)
  {
    
    butt[PS_L1] = 0;
  }
  if (butt[PS_R1] == 1)
  {
    
    butt[PS_R1] = 0;
  }
  if (butt[PS_L2] == 1)
  {
    s_mode = 5;
    butt[PS_L2] = 0;
  }
  if (butt[PS_R2] == 1)
  {
    s_mode = 0;
    butt[PS_R2] = 0;
  }
  if (butt[PS_L3] == 1)
  {
    butt[PS_L3] = 0;
  }
  if (butt[PS_R3] == 1)
  {
    butt[PS_R3] = 0;
  }
}

void ps2()
{
  if ((RX_raw > 200) && (RX_raw < 255))     //201 to 216 for addresses of analog values, 231 to 246 for buttons;
  {
    RX_ad = RX_raw;
    if ((RX_raw > 230) && (RX_raw < 247))
    {
      uint8_t r_temp0 = (RX_raw - 231);
      butt[r_temp0] = 1;
    }
  }
  else if ((RX_raw >= 0) && (RX_raw < 201))
  {
    uint8_t r_temp1 = (RX_ad - 201);
    if (r_temp1 < 16)
    {
      RX[r_temp1] = RX_raw;
    }
  }
}


long map_value(long in_value, long in_min, long in_max, long out_min, long out_max)
{
  return (in_value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long limit_var(long in_var, long l_limit, long h_limit)
{
  if (in_var > h_limit)
  {
    in_var = h_limit;
  }
  else if (in_var < l_limit)
  {
    in_var = l_limit;
  }
  return in_var;
}
ISR (USART_RX_vect)
{
 RX_raw = UDR0;
 ps2();
}
