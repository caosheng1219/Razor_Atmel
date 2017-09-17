/**********************************************************************************************************************
File: user_app1.c                                                                
Cao sheng

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
  PWMAudioSetFrequency(BUZZER1, 200);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u8 au8Store[100];
  static u8 au8Flag[1];
  static u8 au8State1[]="Entering state 1";
  static u8 au8State2[]="Entering state 2";
  static u8 au8LCDMessage1[]="STATE 1             ";
  static u8 au8LCDMessage2[]="STATE 2             ";
  static u32 u32Counter1=0;
  static u8 u8StateFlag=0;

  static bool bState1=FALSE;
  static bool bState2=FALSE;
  static bool bBuzzerOn=FALSE;
  static u8 u8abc=0;
  
  DebugScanf(au8Flag);
  if(au8Flag[0]!='\0')
  {
    au8Store[0]=au8Flag[0];
    
    if(au8Store[0]==1)
    {
      u8StateFlag=1;
    } 
  
    if(au8Store[0]==2)
    {
      u8StateFlag=2;
    }
 
    if(au8Store[0]=='\r')
    {
      if(u8StateFlag==1)  
      {
        u8abc=1;
      }
  
      if(u8StateFlag==2)
      {
        u8abc=2;
      }
    }
  }
  
  

  if(WasButtonPressed(BUTTON1)||u8abc==1)
  {
    ButtonAcknowledge(BUTTON1);  
    LCDMessage(LINE1_START_ADDR,au8LCDMessage1);
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOff(RED);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(GREEN);
    bState1=TRUE;
    bState2=FALSE;
 
    LedPWM(LCD_RED,LED_PWM_100);
    LedOff(LCD_GREEN);
    LedPWM(LCD_BLUE,LED_PWM_100);
  }
  if(bState1)
  {
    DebugPrintf(au8State1);
    bState1=FALSE;
    PWMAudioOff(BUZZER1);
    bBuzzerOn=FALSE;
  }
  
  if(WasButtonPressed(BUTTON2)||u8abc==2)
  {
   
    ButtonAcknowledge(BUTTON2); 
    LCDMessage(LINE1_START_ADDR,au8LCDMessage2);
    LedBlink(GREEN,LED_1HZ);
    LedBlink(YELLOW,LED_2HZ);
    LedBlink(ORANGE,LED_4HZ);
    LedBlink(RED,LED_8HZ);
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    bState2=TRUE;
    bState1=FALSE;
    LedPWM(LCD_RED,LED_PWM_100);
    LedPWM(LCD_GREEN,LED_PWM_20);
    LedOff(LCD_BLUE);

  }
  if(bState2)
  {
    DebugPrintf(au8State2);
    bState2=FALSE;
    bBuzzerOn=TRUE;
  }
  
  if(bBuzzerOn)
  { 

    u32Counter1++;
    if(u32Counter1<100)
    {
      PWMAudioOn(BUZZER1);
    }
    if(u32Counter1==100)
    {
      PWMAudioOff(BUZZER1);
    }
    if(u32Counter1==1000)
    {
      u32Counter1=0;
    }
  }
} /* end UserApp1SM_Idle() */
    
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
