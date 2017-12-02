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

static u8 UserApp1_au8MasterName[9]   = "0\0\0\0\0\0\0\0";

static AntAssignChannelInfoType sChannelInfo;
static bool bInitialize=FALSE;
static u8 u8ChangeFlag=0;
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

  static u8 au8Networkkey[8]=AU8_ANT_PLUS_NETWORK_KEY;
  /* Clear screen and place start messages */
#ifdef EIE1
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, "B1 Normal B2 LoseWei");
  LCDMessage(LINE2_START_ADDR, "B3 Find Device");

  PWMAudioSetFrequency(BUZZER1, 2000);

  
  LedOff(ORANGE);
  LedOff(RED);
  LedOff(PURPLE);
  LedOff(CYAN);
  LedOff(RED);
  LedOff(BLUE);
  LedOff(GREEN);
  LedOff(WHITE);
  /* Start with LED0 in RED state = channel is not configured */

#endif /* EIE1 */
  

  
 /* Configure ANT for this application */
  sChannelInfo.AntChannel          = ANT_CHANNEL_0;
  sChannelInfo.AntChannelType      = CHANNEL_TYPE_SLAVE;
  sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sChannelInfo.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sChannelInfo.AntTxPower          = ANT_TX_POWER_USERAPP;

  sChannelInfo.AntNetwork          = ANT_NETWORK_DEFAULT;
  

  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sChannelInfo.AntNetworkKey[i] = au8Networkkey[i];
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sChannelInfo) )
  {
    /* Channel assignment is queued so start timer */

    UserApp1_u32Timeout = G_u32SystemTime1ms;
    LedOn(RED);
    
    
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */

    LedBlink(RED, LED_4HZ);

    
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
  if(AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */
/*!-------------------------------------------------------------------------------------------------------------------
@fn static void UserApp1SM_Idle(void)

@brief Wait for user to press BUTTON0 to connect to an HRM 
*/
static void UserApp1SM_Idle(void)
{
  PWMAudioOff(BUZZER1);
  bInitialize=FALSE;
  u8ChangeFlag=0;
  if(WasButtonPressed(BUTTON1))
  {
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(RED);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
    ButtonAcknowledge(BUTTON1);
    LCDCommand(LCD_CLEAR_CMD);
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_ChannelOpening1;
  }
  
  if(WasButtonPressed(BUTTON2))
  {
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
    ButtonAcknowledge(BUTTON2);
    LCDCommand(LCD_CLEAR_CMD);
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_ChannelOpening2;
  }
  
  if(WasButtonPressed(BUTTON3))
  {
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(RED);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
    ButtonAcknowledge(BUTTON3);
    LCDCommand(LCD_CLEAR_CMD);
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_ChannelOpening3;
  }
  
} /* end UserApp1SM_Idle() */
    




static void UserApp1SM_ChannelOpening1(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_Mode1;
  }
}
static void UserApp1SM_ChannelOpening2(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_Mode2;
  }
}
static void UserApp1SM_ChannelOpening3(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_Mode3;
  }
}






static void UserApp1SM_Mode1(void)
{
  static u8 au8CurrentMessage[4]={0,0,0,0};
  static u8 u8Heartrate=50;
  static u8 au8Show[20]={'H','e','a','r','t','R','a','t','e',':','0','0','0'};
  static bool bBlink=FALSE;

  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Idle;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "B1 Normal B2 LoseWei");
    LCDMessage(LINE2_START_ADDR, "B3 Find Device");
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
  }
  
  if( AntReadAppMessageBuffer() )
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      for(u8 u8i=0;u8i<4;u8i++)
      {
        au8CurrentMessage[u8i]=G_au8AntApiCurrentMessageBytes[u8i+4];
      }
      u8Heartrate=au8CurrentMessage[3];
      au8Show[10]=u8Heartrate/100+48;
      au8Show[11]=(u8Heartrate%100)/10+48;
      au8Show[12]=(u8Heartrate%100)%10+48;
      LCDMessage(LINE1_START_ADDR,au8Show);
      
    }
  }
  if(u8Heartrate<50)
  {
    PWMAudioOn(BUZZER1);
  }
  else
  {
    PWMAudioOff(BUZZER1);
  }
  
  if(u8Heartrate>50&&u8Heartrate<60)
  {
    LedOn(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>60&&u8Heartrate<70)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>70&&u8Heartrate<80)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>80&&u8Heartrate<90)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>90&&u8Heartrate<100)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOn(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>100&&u8Heartrate<110)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOn(GREEN);
    LedOn(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>110&&u8Heartrate<120)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOn(GREEN);
    LedOn(YELLOW);
    LedOn(ORANGE);
    LedOff(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>120&&u8Heartrate<130)
  {
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOn(GREEN);
    LedOn(YELLOW);
    LedOn(ORANGE);
    LedOn(RED);
    bBlink=FALSE;
  }
  if(u8Heartrate>130&&!bBlink)
  {
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
    LedBlink(ORANGE,LED_2HZ);
    LedBlink(RED,LED_2HZ);
    LedBlink(PURPLE,LED_2HZ);
    LedBlink(CYAN,LED_2HZ);
    LedBlink(YELLOW,LED_2HZ);
    LedBlink(BLUE,LED_2HZ);
    LedBlink(GREEN,LED_2HZ);
    LedBlink(WHITE,LED_2HZ);
    bBlink=TRUE;
  }

  
  
}

static void UserApp1SM_Mode2(void)
{

  static u8 u8Age=0;

  static u8 au8HRMemory[100];
  static u16 u16Counterdown=10000;
  static u8 u8Counter=0;
  static u8 u8StaticHeartRate=0;
  static u32 u32store=0;
  static u8 u8MaxLoseWeightHR=0;
  static u8 u8MinLoseWeightHR=0;  
  static u8 u8Heartrate=0;
  static u8 au8Show[20]={'H','e','a','r','t','R','a','t','e',':','0','0','0'};
  static u8 au8Show2[20]={'A','p','p','r','o','p','r','i','a','t','e','0','0','0','-','0','0','0'};
  static bool bNotShown=TRUE;
  static u8 au8CurrentMessage[4]={0,0,0,0};
  static u8 au8AgeMessage[20]={'B','U','T','T','O','N','1',' ','G','O',' ','O','N',' ','0',' ',' ',' ',' ','0'};
  static u8 u8AgeHi[1];
  static u8 u8AgeLo[1];
  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Idle;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "B1 Normal B2 LoseWei");
    LCDMessage(LINE2_START_ADDR, "B3 Find Device");
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
  }
  
  if(!bInitialize)
  {    
    au8AgeMessage[14]=48;
    au8AgeMessage[19]=48;
    u8AgeLo[0]=0;
    u8AgeHi[0]=0;
    LCDMessage(LINE1_START_ADDR, "SET YOUR AGE");
    LCDMessage(LINE2_START_ADDR, au8AgeMessage);
    bInitialize=TRUE;
    bNotShown=TRUE;

  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    u8ChangeFlag=1;
    LCDCommand(LCD_CLEAR_CMD);
    u8Age=u8AgeHi[0]*10+u8AgeLo[0];
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    u8AgeHi[0]=u8AgeHi[0]+1;
    if(u8AgeHi[0]>9)
    {
      u8AgeHi[0]=u8AgeHi[0]-10;
    }
    au8AgeMessage[14]=u8AgeHi[0]+48;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "SET YOUR AGE");
    LCDMessage(LINE2_START_ADDR, au8AgeMessage);
  }
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    u8AgeLo[0]=u8AgeLo[0]+1;
    if(u8AgeLo[0]>9)
    {
      u8AgeLo[0]=u8AgeLo[0]-10;
    }
    au8AgeMessage[19]=u8AgeLo[0]+48;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "SET YOUR AGE");
    LCDMessage(LINE2_START_ADDR, au8AgeMessage);
  }
  
  if(u8ChangeFlag==1)
  {
    u16Counterdown--;
    if( AntReadAppMessageBuffer() )
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        for(u8 u8i=0;u8i<4;u8i++)
        {
          au8CurrentMessage[u8i]=G_au8AntApiCurrentMessageBytes[u8i+4];
        }
        u8Heartrate=au8CurrentMessage[3];
        au8Show[10]=u8Heartrate/100+48;
        au8Show[11]=(u8Heartrate%100)/10+48;
        au8Show[12]=(u8Heartrate%100)%10+48;
        LCDMessage(LINE1_START_ADDR,au8Show);
        
        au8HRMemory[u8Counter]=G_au8AntApiCurrentMessageBytes[7];
        u8Counter++;
      }
    }
    if(u16Counterdown==0)
    {
      u8ChangeFlag=2;
      LCDCommand(LCD_CLEAR_CMD);
    }
  }
  
  if(u8ChangeFlag==2)
  {
    u16Counterdown=10000;
    for(u8 u8j=0;u8j<u8Counter;u8j++)
    {
      u32store+=au8HRMemory[u8j];
    }
    u8StaticHeartRate=u32store/u8Counter;
    u8ChangeFlag=3;
  }
  
  if(u8ChangeFlag==3)
  {
    u8Counter=0;
    u8MaxLoseWeightHR=((160-u8Age-u8StaticHeartRate) * 0.85)+u8StaticHeartRate;
    u8MinLoseWeightHR=((160-u8Age-u8StaticHeartRate) * 0.65)+u8StaticHeartRate;
    au8Show2[11]=u8MinLoseWeightHR/100+48;
    au8Show2[12]=(u8MinLoseWeightHR%100)/10+48;
    au8Show2[13]=(u8MinLoseWeightHR%100)%10+48;
    au8Show2[15]=u8MaxLoseWeightHR/100+48;
    au8Show2[16]=(u8MaxLoseWeightHR%100)/10+48;
    au8Show2[17]=(u8MaxLoseWeightHR%100)%10+48;
    
    if(bNotShown)
    {
      LCDMessage(LINE2_START_ADDR,au8Show2);
      bNotShown=FALSE;
    }
    
    if( AntReadAppMessageBuffer() )
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        for(u8 u8i=0;u8i<4;u8i++)
        {
          au8CurrentMessage[u8i]=G_au8AntApiCurrentMessageBytes[u8i+4];
        }
        u8Heartrate=au8CurrentMessage[3];
        au8Show[10]=u8Heartrate/100+48;
        au8Show[11]=(u8Heartrate%100)/10+48;
        au8Show[12]=(u8Heartrate%100)%10+48;
        LCDMessage(LINE1_START_ADDR,au8Show);
        
        au8HRMemory[u8Counter]=G_au8AntApiCurrentMessageBytes[7];
        u8Counter++;
      }
    }   
 
  }
}

static void UserApp1SM_Mode3(void)
{
  static s8 s8RssiChannel0=-99;
  static u8 au8Temp[]={'R','S','S','I','=','-',1,1,'d','B','m','\0'};
  static u8 u8Temp;


  
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Idle;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "B1 Normal B2 LoseWei");
    LCDMessage(LINE2_START_ADDR, "B3 Find Device");
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
  }
  
  if( AntReadAppMessageBuffer() )
    {
      /* New data message: check what it is */
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        if(G_sAntApiCurrentMessageExtData.u8Channel == 0x00)
        {
          s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;

        }
      }
       


      u8Temp = abs(s8RssiChannel0);
      au8Temp[6] = u8Temp/10 + 48;
      au8Temp[7] = u8Temp%10 + 48;
      LCDMessage(LINE1_END_ADDR-20, au8Temp);
    

      if(s8RssiChannel0>-120&&s8RssiChannel0<-110)
      {
        LedOff(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-110&&s8RssiChannel0<-100)
      {
        LedOn(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-100&&s8RssiChannel0<-90)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-90&&s8RssiChannel0<-80)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-80&&s8RssiChannel0<-70)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-70&&s8RssiChannel0<-65)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-65&&s8RssiChannel0<-60)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOn(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(s8RssiChannel0>-60&&s8RssiChannel0<-55)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOn(YELLOW);
        LedOn(ORANGE);
        LedOff(RED);
        
      }
      if(s8RssiChannel0>=-50)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOn(YELLOW);
        LedOn(ORANGE);
        LedOn(RED);
        s8RssiChannel0=-99;
      }
    }

}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */





/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
