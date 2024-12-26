#ifndef KH_AVR_TIMER2_CPP
#define KH_AVR_TIMER2_CPP
//================================================================//
// This is a working library in C++ for Atmega8A Timer2           //
// Working Finched in 11 July - 2021                              //
// Updating start in 30 Nov 2024                                  //
//================================================================//
//@ Pre scale value for the timer setup
#define PRE_SCALE 8.0
#define USE_INTERRUPT 1
#define GET_REAL_TIME 1
#ifndef F_CPU
  #define F_CPU 16000000UL
#endif

#if USE_INTERRUPT == 1
  #ifndef _AVR_INTERRUPT_H_
    #include "avr\interrupt.h"
  #endif
#endif


enum Timer2OperationsMode
{
  NormalMode_OP,
  PWMMode_OP,
  CTCMode_OP
};

class Atmega8Timer2
{
private:
  struct Timer2Interrupts
  {
    unsigned char En_Tim2OCI_Intr : 1;
    unsigned char En_Tim2OvrFlow_Intr : 1;
    Timer2Interrupts()
    {
      En_Tim2OCI_Intr = false;
      En_Tim2OvrFlow_Intr = false;
    }
  };

  Timer2Interrupts Timer2InterruptsSetup;
  // Timer 2 Operations mode

public:
  uint8_t compareWithNum;
  Atmega8Timer2(uint8_t value);
  Atmega8Timer2(bool ovi = false, bool oci = false);
  //~Atmega8Timer2();

  enum Timer2_ClkScaling
  {
    Pre_OFF,
    PreSc_1,
    PreSc_8,
    PreSc_64,
    PreSc_128,
    PreSc_256,
    PreSc_1024,
    ExtClk_FallingEdge,
    ExtClk_RisingEdge
  } timer2_scaler;
  enum Timer2CTC_OC2_Modes
  {
    CTC_OC2_SetMode,
    CTC_OC2_ClearMode,
    CTC_OC2_ToggleMode
  }; // Timer2 OC2 pin Modes in case of CTC operation mode
  enum Timer2PWM_OC2_Modes
  {
    PWM_OC2_InvertingMode,
    PWM_OC2_NonInvertingMode
  }; // Timer2 OC2 pin Modes in case of PWM operation mode
  enum Timer2PWM_Modes
  {
    PWM_Fast,
    PWM_PhCorrect
  }; // Timer 2 Pulse widith modulation modes

  Timer2OperationsMode timer_Op_Mode;

  void Setup_Timer2PWM(Timer2PWM_Modes pwm_mode, Timer2PWM_OC2_Modes oc2_modes);
  void Setup_Timer2CTC(Timer2CTC_OC2_Modes ctc_mode);
  void Setup_Timer2Interrupt(bool, bool);
  void Start_Timer2(Timer2_ClkScaling t2sel = Pre_OFF);

#if (GET_REAL_TIME == 1 && USE_INTERRUPT == 1)
  void Get_Current_Time();
#endif
  // void Stop_Timer2 ();
  // static Atmega8Timer2* ptrAtmegaCls;
};

#endif