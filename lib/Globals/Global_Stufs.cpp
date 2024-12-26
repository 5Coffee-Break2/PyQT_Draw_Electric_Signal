#include "Global_Stufs.hpp"

void Set_Leds()
{
    LED_1 |= 1 << LED_1_Pin;
    LED_2 |= 1 << LED_2_Pin;   
    LED_3 |= 1 << LED_3_Pin;   
}

void my_Delay_Fn(long dt)
{
  for (long i = 0; i <= dt; i++)
  {
    asm("");
  }
}

void Wink_Led1(long ld)
{
  LED_1_Outp ^= (1 << LED_1_Pin);
  my_Delay_Fn(ld);
  LED_1_Outp ^= (1 << LED_1_Pin);
}

void Wink_Led2(long ld )
{
  (LED_2_Outp) ^= 1 << (LED_2_Pin);
  my_Delay_Fn(ld);
  (LED_2_Outp) ^= 1 << (LED_2_Pin);
}

void Wink_Led3(long ld)
{
  LED_3_Outp ^= 1 << LED_3_Pin;
  my_Delay_Fn(ld);
  LED_3_Outp ^= 1 << LED_3_Pin;
} 

void Wink_Both(long ld)
{
   LED_1_Outp ^= 1 << LED_1_Pin;
   LED_2_Outp ^= 1 << LED_2_Pin;
  my_Delay_Fn(ld);
  LED_1_Outp ^= 1 << LED_1_Pin;
  LED_2_Outp ^= 1 << LED_2_Pin;
}