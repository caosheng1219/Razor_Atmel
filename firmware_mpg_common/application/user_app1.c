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

  static bool bState1=FALSE;
  static bool bState2=FALSE;
  static bool bBuzzerOn=FALSE;
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
  static u8 au8Input[1];
  static u16 u16Counter1=0;    //a counter control buzzer
  static u16 u16Counter2=0;    //a counter to help store something inputted

  
  DebugScanf(au8Input);

  if(au8Input[0]!='\0')      // store data 
  {
    au8Store[u16Counter2]=au8Input[0];
    u16Counter2++;
    au8Input[0]='\0';
  }
  /*change state*/
  if(au8Store[u16Counter2-2]=='1'&&au8Store[u16Counter2-1]=='\r')  
  {
    bState1=TRUE;
    bState2=FALSE;
    for(u8 u8i=0;u8i<100;u8i++)
    {
      au8Store[u8i]='\0';
    }
  }
  
  if(au8Store[u16Counter2-2]=='2'&&au8Store[u16Counter2-1]=='\r')
  {
    bState2=TRUE;
    bState1=FALSE;
    for(u8 u8i=0;u8i<100;u8i++)
    {
      au8Store[u8i]='\0';
    }
  }
  
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);  
    bState1=TRUE;
    bState2=FALSE;    
  }
 
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);     
    bState2=TRUE;
    bState1=FALSE;
  }
  /*change state complete*/
  
  /*enter state*/
  if(bState1)
  {
    UserApp1_StateMachine=UserApp1SM_StateMachine1;
  }
  

  if(bState2)
  {
    UserApp1_StateMachine=UserApp1SM_StateMachine2;
  }
  
  if(bBuzzerOn)
  { 

    u16Counter1++;
    if(u16Counter1==1)
    {
      PWMAudioOn(BUZZER1);
    }
    if(u16Counter1==100)
    {
      PWMAudioOff(BUZZER1);
    }
    if(u16Counter1==1000)
    {
      u16Counter1=0;
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


static void UserApp1SM_StateMachine1(void)
{
    LCDMessage(LINE1_START_ADDR,"STATE 1             ");
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOff(RED);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(GREEN);
    LedPWM(LCD_RED,LED_PWM_100);
    LedOff(LCD_GREEN);
    LedPWM(LCD_BLUE,LED_PWM_100);
    DebugPrintf("Entering state 1");
    DebugPrintf("\n\r");
    bState1=FALSE;
    PWMAudioOff(BUZZER1);
    bBuzzerOn=FALSE;  
    
    UserApp1_StateMachine=UserApp1SM_Idle;
}

static void UserApp1SM_StateMachine2(void)
{  
    LCDMessage(LINE1_START_ADDR,"STATE 2             ");
    LedBlink(GREEN,LED_1HZ);
    LedBlink(YELLOW,LED_2HZ);
    LedBlink(ORANGE,LED_4HZ);
    LedBlink(RED,LED_8HZ);
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedPWM(LCD_RED,LED_PWM_100);
    LedPWM(LCD_GREEN,LED_PWM_30);
    LedOff(LCD_BLUE);
    DebugPrintf("Entering state 2");
    DebugPrintf("\n\r");
    bState2=FALSE;
    bBuzzerOn=TRUE;
    
    UserApp1_StateMachine=UserApp1SM_Idle;
}






/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
