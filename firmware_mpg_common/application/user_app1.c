/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  AT91C_BASE_PIOA->PIO_PER    = PIOA_PER_ADD_INIT;
  AT91C_BASE_PIOA->PIO_PDR    = PIOA_PDR_ADD_INIT;
  AT91C_BASE_PIOA->PIO_OER    = PIOA_OER_ADD_INIT;
  AT91C_BASE_PIOA->PIO_ODR    = PIOA_ODR_ADD_INIT;
  AT91C_BASE_PIOA->PIO_SODR   = PIOA_SODR_ADD_INIT;
  AT91C_BASE_PIOA->PIO_CODR   = PIOA_CODR_ADD_INIT;
  
  AT91C_BASE_PIOB->PIO_PER    = PIOB_PER_ADD_INIT;
  AT91C_BASE_PIOB->PIO_PDR    = PIOB_PDR_ADD_INIT;
  AT91C_BASE_PIOB->PIO_OER    = PIOB_OER_ADD_INIT;
  AT91C_BASE_PIOB->PIO_ODR    = PIOB_ODR_ADD_INIT;
  AT91C_BASE_PIOB->PIO_SODR   = PIOB_SODR_ADD_INIT;
  AT91C_BASE_PIOB->PIO_CODR   = PIOB_CODR_ADD_INIT;
  
  for(u8 i=0;i<10;i++)
  {
  Sound_down();
  }
  
  LedOn(PURPLE);
  LedOff(GREEN);
  LedOff(BLUE);
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,"SIN:MUTE VOL:0");
  LCDMessage(LINE2_START_ADDR,"ADC12B:0x***"); 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------
Function: ChangeSign

Description
When Button3 was pressed,change the sign source

Requires:
  - u8 u8Mode_

Promises:
  - Change the sign source and set the I/O
*/
void ChangeSign(u8 u8Mode_)
{
  AT91C_BASE_PIOB->PIO_SODR   = Re_sign_D;
  AT91C_BASE_PIOB->PIO_CODR   = Re_sign_C;
  switch(u8Mode_)
  {
  case 0:
    AT91C_BASE_PIOA->PIO_SODR   = Silence_SODR;
    AT91C_BASE_PIOA->PIO_CODR   = Silence_CODR;
    LedOn(PURPLE);
    LedOff(GREEN);
    LedOff(BLUE);
    LedOff(WHITE);
    LCDMessage(LINE1_START_ADDR,"SIN:MUTE");
    break;
    
  case 1:
    AT91C_BASE_PIOA->PIO_SODR   = MIC_SODR;
    AT91C_BASE_PIOA->PIO_CODR   = MIC_CODR;
    LedOn(BLUE);
    LedOff(PURPLE);
    LedOff(GREEN);  
    LedOff(WHITE);
    LCDMessage(LINE1_START_ADDR,"SIN:MIC");
    break; 
    
  case 2:
    AT91C_BASE_PIOA->PIO_SODR   = Phone_SODR;
    AT91C_BASE_PIOA->PIO_CODR   = Phone_CODR;
    LedOn(GREEN);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(WHITE);
    LCDMessage(LINE1_START_ADDR,"SIN:PHO");
    break;

    
  default:break;
  }
}/* end ChangeSign()*/

/*----------------------------------------------------------------------------------------------------------------------
Function: Sound_up

Description
When Buttn0 was pressed,turn up the sound

Requires:
  - 

Promises:
  - Give the INC a drop along signal 
*/
void Sound_up(void)
{
  AT91C_BASE_PIOA->PIO_SODR = Sound_u_SODR;
  AT91C_BASE_PIOA->PIO_CODR = Sound_u_CODR;
  
  for(u8 i=0;i<10;i++)
  {
    AT91C_BASE_PIOA->PIO_SODR = INC_L_SODR;
    AT91C_BASE_PIOA->PIO_CODR = INC_L_CODR;
    Delay_us(10);
    AT91C_BASE_PIOA->PIO_SODR = INC_H_SODR;
    AT91C_BASE_PIOA->PIO_CODR = INC_H_CODR;    
  }
  
}/* end Sound_up()*/

/*----------------------------------------------------------------------------------------------------------------------
Function: 

Description


Requires:
  - 

Promises:
  - 
*/
void Sound_down(void)
{
  AT91C_BASE_PIOA->PIO_SODR = Sound_d_SODR;
  AT91C_BASE_PIOA->PIO_CODR = Sound_d_CODR;
  
  for(u8 i=0;i<10;i++)
  {
    AT91C_BASE_PIOA->PIO_SODR = INC_L_SODR;
    AT91C_BASE_PIOA->PIO_CODR = INC_L_CODR;
    Delay_us(10);
    AT91C_BASE_PIOA->PIO_SODR = INC_H_SODR;
    AT91C_BASE_PIOA->PIO_CODR = INC_H_CODR;    
  }
  
}/* end Sound_down */

/*----------------------------------------------------------------------------------------------------------------------
Function: 

Description


Requires:
  - 

Promises:
  - 
*/
void Test(void)
{
  AT91C_BASE_PIOA->PIO_SODR = Test_v_SODR;
  AT91C_BASE_PIOA->PIO_CODR = Test_v_CODR;
  AT91C_BASE_PIOB->PIO_SODR = Re_adc_D;
  AT91C_BASE_PIOB->PIO_CODR = Re_adc_C;
  LedOff(BLUE);
  LedOff(PURPLE);
  LedOff(GREEN);  
  LedOn(WHITE);
  LCDMessage(LINE1_START_ADDR,"SIN:TEST");
}/* end Test*/

/*----------------------------------------------------------------------------------------------------------------------
Function: 

Description


Requires:
  - 

Promises:
  - 
*/
void Delay_us(u8 u8time_)
{
  u8 i=0;
  u8 j=0;
  for(i=0;i<u8time_;i++)
  {
    for(j=0;j<48;j++)
    {
    }
  }
}/* end Delay_us()*/

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u8 u8SignMode = 0;
  static u16 u16TEST_V=0; 
  static u8 au8Vol[1]={'0'};
  static u8 au8ADC[7]={'0','0','0',' ',' ',' ',' '};
  static bool bTest=FALSE;
   
  LedOff(RED);
  
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    LedOn(RED);
    u8SignMode++;
    
    if(u8SignMode>2)
    {
      u8SignMode=0;
    }
    
    ChangeSign(u8SignMode);
  }
  
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    LedOn(RED);
    Test();
    bTest=TRUE;
    LCDMessage(LINE2_START_ADDR+9,&au8ADC[0]);  
  }  
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LedOn(RED);
    au8Vol[0]++;
    
    if(au8Vol[0]<='9'&&au8Vol[0]>='0')
    {
      LCDMessage(LINE1_START_ADDR+13,&au8Vol[0]);
      LCDClearChars(LINE1_START_ADDR+14,6);
    }
    else
    {
      au8Vol[0]='9';
    }
    
    Sound_up();
  }
  
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    LedOn(RED);
    au8Vol[0]--;

    if(au8Vol[0]<='9'&au8Vol[0]>='0')
    {
      LCDMessage(LINE1_START_ADDR+13,&au8Vol[0]);
      LCDClearChars(LINE1_START_ADDR+14,6);
    }
    else
    {
      au8Vol[0]='0';
    }    
    
    Sound_down();
  }
  
  if(bTest)
  {
    if(Adc12StartConversion(ADC12_BLADE_AN0))
    {
      u16TEST_V = AT91C_BASE_ADC12B->ADC12B_CDR[ADC12_BLADE_AN0];
      au8ADC[0]=HexToASCIICharUpper(u16TEST_V/16/16);
      au8ADC[1]=HexToASCIICharUpper(u16TEST_V/16%16);
      au8ADC[2]=HexToASCIICharUpper(u16TEST_V%16);
      bTest=TRUE;
    }
  }

} /* end UserApp1SM_Idle() */
    


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

