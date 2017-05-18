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
  LCDCommand(LCD_CLEAR_CMD);
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
  static u8 au8AllMessage[]="Welcome to caosheng and fanyi's LCD homework!                                        ";
  static u8 au8MessageLine1[20];   //the array to store the message display on line1
  static u8 au8MessageLine2[20];   //the array to store the message display on line2
  static u32 u32Counter=400;  //to count every 400ms rolls one time
  static bool bOn=FALSE;  //the flag to control pause
  static u8 u8Flag1=0;  //to mark where the au8MessageLine1 and au8MessageLine2 begin
  static u8 u8Flag2=20;
  
  /* get value for au8MessageLine1&au8MessageLine2 */
  for(u8 i=0;i<20;i++)
  {
    au8MessageLine1[i]=au8AllMessage[i+u8Flag1];  
  }
  for(u8 j=0;j<20;j++)
  {
    au8MessageLine2[j]=au8AllMessage[j+u8Flag2];
  }
  
  /* use BUTTON0 to start and use BUTTON1 to pause*/
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    bOn=TRUE;
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    bOn=FALSE;
    PWMAudioOff(BUZZER1);
  }
  
  /*when pressed BUTTON0*/
  if(bOn)
  {
    u32Counter--;
    
    /* these code is for phenomenon while the program is running*/
    if(u32Counter==400)
    {
      LedOn(WHITE);
      PWMAudioSetFrequency(BUZZER1, 100);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==350)
    {
      LedOn(PURPLE);
      PWMAudioSetFrequency(BUZZER1, 200);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==300)
    {
      LedOn(BLUE);
      PWMAudioSetFrequency(BUZZER1, 300);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==250)
    {
      LedOn(CYAN);
      PWMAudioSetFrequency(BUZZER1, 400);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==200)
    {
      LedOn(GREEN);
      PWMAudioSetFrequency(BUZZER1, 500);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==150)
    {
      LedOn(YELLOW);
      PWMAudioSetFrequency(BUZZER1, 600);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==100)
    {
      LedOn(ORANGE);
      PWMAudioSetFrequency(BUZZER1, 700);
      PWMAudioOn(BUZZER1);
    }
    
    if(u32Counter==50)
    {
      LedOn(RED);
      PWMAudioSetFrequency(BUZZER1, 800);
      PWMAudioOn(BUZZER1);
    }
    if(u32Counter==0)
    {
      PWMAudioOff(BUZZER1);
      LedOff(WHITE);
      LedOff(PURPLE);
      LedOff(BLUE);
      LedOff(CYAN);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(ORANGE);
      LedOff(RED);
      u32Counter=400;
      u8Flag1++;  //change the au8MessageLine1 and au8MessageLine2
      u8Flag2++;
      if(u8Flag2>65) // when au8MessageLine2 beyong au8AllMessage, refresh the flag
      {
        u8Flag1=0;
        u8Flag2=20;
      }
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8MessageLine1);
      LCDMessage(LINE2_START_ADDR, au8MessageLine2);

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
