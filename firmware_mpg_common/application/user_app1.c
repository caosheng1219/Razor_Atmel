/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

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
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp1_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
  u8 au8WelcomeMessage[] = "Hide and Go Seek";
  u8 au8Instructions[] = "B0 start game";
  AntAssignChannelInfoType sAntSetupData;
  
  /* Clear screen and place start messages */
#ifdef EIE1
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */

#endif /* EIE1 */
  

  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel assignment is queued so start timer */
#ifdef EIE1
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    LedOn(RED);
#endif /* EIE1 */
    
#ifdef MPG2
    LedOn(RED0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
#ifdef EIE1
    LedBlink(RED, LED_4HZ);
#endif /* EIE1 */
    
#ifdef MPG2
    LedBlink(RED0, LED_4HZ);
#endif /* MPG2 */

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


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Assignrole;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */

static void UserApp1SM_Assignrole(void)
{
  u8 au8AssignMessage[] = "Assign your role";
  u8 au8Instructions[] = "B0 Hider  B1 Seeker";
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, B0 Start); 
    
    UserApp1_StateMachine=UserApp1SM_Hider;
  }
  
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, B0 Start); 
    
    UserApp1_StateMachine=UserApp1SM_Seeker;
  }
  
  
  
  
}
  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Seeker(void)
{
  static u16 u16Counter=10000;
  static bool bCountdown=FALSE;

  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    bCountdown=TRUE;
    LCDCommand(LCD_CLEAR_CMD);
  }
  
  if(bCountdown)
  {
    u16Counter--;
    if(u16Counter==9000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "9");
    }
    
    if(u16Counter==8000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "8");
    }
    
    if(u16Counter==7000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "7");
    }
    
    if(u16Counter==6000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "6");
    }
    
    if(u16Counter==5000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "5");
    }
    
    if(u16Counter==4000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "4");
    }
    
    if(u16Counter==3000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "3");
    }

    if(u16Counter==2000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "2");
    }
    
    if(u16Counter==1000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker            "); 
      LCDMessage(LINE2_START_ADDR, "1");
    }
    
    if(u16Counter==0)
    {     
      LCDMessage(LINE1_START_ADDR, "Ready or not  "); 
      LCDMessage(LINE2_START_ADDR, "Here I come!");
      AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
      bCountdown=FALSE;

      UserApp1_StateMachine = UserApp1SM_ChannelOpen;
    }

  }
}
  
  
    
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to open */
static void UserApp1SM_WaitChannelOpen(void)
{
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_ChannelOpen;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
    
} /* end UserApp1SM_WaitChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Channel is open, so monitor data */
static void UserApp1SM_ChannelOpen(void)
{
  static s8 s8RssiChannel0;
  static u8 au8Temp[]={'-',1,1,'d','B','m','\0'};
  static u8 u8Temp;
  static bool bFound=FALSE;
  /* Check for BUTTON0 to close channel */
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue close channel and change LED to blinking green to indicate channel is closing */
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);


    UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
  }


  
  /* Always check for ANT messages */
  if( AntReadAppMessageBuffer() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;
    }

    s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;
    u8Temp = abs(s8RssiChannel0);
    au8Temp[1] = u8Temp/10 + 48;
    au8Temp[2] = u8Temp%10 + 48;
    LCDMessage(LINE1_END_ADDR-6, au8Temp);
    
    if(bFound=FALSE)
    {
      if(s8RssiChannel0>-120&&s8RssiChannel0<-110)
      {
        LedOn(ORANGE);
        LedOff(RED);
        LedOff(PURPLE);
        LedOff(CYAN);
        LedOff(RED);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-110&&s8RssiChannel0<-100)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOff(PURPLE);
        LedOff(CYAN);
        LedOff(RED);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-100&&s8RssiChannel0<-90)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOn(PURPLE);
        LedOff(CYAN);
        LedOff(RED);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-90&&s8RssiChannel0<-80)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOn(PURPLE);
        LedOn(CYAN);
        LedOff(RED);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-80&&s8RssiChannel0<-70)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOn(PURPLE);
        LedOn(CYAN);
        LedOn(RED);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-70&&s8RssiChannel0<-60)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOn(PURPLE);
        LedOn(CYAN);
        LedOn(RED);
        LedOn(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-60&&s8RssiChannel0<-50)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOn(PURPLE);
        LedOn(CYAN);
        LedOn(RED);
        LedOn(BLUE);
        LedOn(GREEN);
        LedOff(WHITE);
      }
      if(s8RssiChannel0>-50&&s8RssiChannel0<-40)
      {
        LedOn(ORANGE);
        LedOn(RED);
        LedOn(PURPLE);
        LedOn(CYAN);
        LedOn(RED);
        LedOn(BLUE);
        LedOn(GREEN);
        LedOn(WHITE);
        
      }
      if(s8RssiChannel0==40)
      {
        LedOff(ORANGE);
        LedOff(RED);
        LedOff(PURPLE);
        LedOff(CYAN);
        LedOff(RED);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
        bFound=TRUE;
    
      }
    }
    if(bFound)
    {
      LedBlink(ORANGE,LED_2HZ);
      LedBlink(RED,LED_2HZ);
      LedBlink(PURPLE,LED_2HZ);
      LedBlink(CYAN,LED_2HZ);
      LedBlink(RED,LED_2HZ);
      LedBlink(BLUE,LED_2HZ);
      LedBlink(GREEN,LED_2HZ);
      LedBlink(WHITE,LED_2HZ);
      
    }
    
    
    
    
    
    
    
    
    
    
    
  }
  
      
} /* end UserApp1SM_ChannelOpen() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel to close */
static void UserApp1SM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOn(YELLOW);
#endif /* MPG1 */

#ifdef MPG2
    LedOn(GREEN0);
    LedOn(RED0);
#endif /* MPG2 */
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
#ifdef MPG1
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);
#endif /* MPG1 */

#ifdef MPG2
    LedBlink(RED0, LED_4HZ);
    LedOff(GREEN0);
#endif /* MPG2 */
    
    UserApp1_StateMachine = UserApp1SM_Error;
  }
    
} /* end UserApp1SM_WaitChannelClose() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */

static void AntGetdBmAscii(s8 s8Data)
{
	static u8 u8aTemp[]={'-',1,1,'d','B','m','\0'};
	
	u8 temp=abs(s8Data);
	u8aTemp[1]=temp/10+48;
	u8aTemp[2]=temp%10+48;
	LCDMessage(LINE1_START_ADDR+14,u8aTemp);
}



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
