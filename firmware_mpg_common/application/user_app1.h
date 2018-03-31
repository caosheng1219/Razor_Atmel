/**********************************************************************************************************************
File: user_app1.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for user_app1.c

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
/*A16-A   A15-B   A14-CS  A13-C   A12 -INC   A11-UD*/
#define PIOA_PER_ADD_INIT  (u32)0x8403F807
#define PIOA_PDR_ADD_INIT  (u32)0x7BFC07F8
#define PIOA_OER_ADD_INIT  (u32)0xBF55FFF9
#define PIOA_ODR_ADD_INIT  (u32)0x40AA0006
#define PIOA_SODR_ADD_INIT (u32)0x8801C000
#define PIOA_CODR_ADD_INIT (u32)0x30003800

#define PIOB_PER_ADD_INIT  (u32)0x01BFFFB8
#define PIOB_PDR_ADD_INIT  (u32)0x00400040
#define PIOB_OER_ADD_INIT  (u32)0x01BFFFF0
#define PIOB_ODR_ADD_INIT  (u32)0x0040000F
#define PIOB_SODR_ADD_INIT (u32)0x01BFFE00
#define PIOB_CODR_ADD_INIT (u32)0x00000010

#define Phone_SODR         (u32)0x00000000
#define Phone_CODR         (u32)0x0001A000
#define MIC_SODR           (u32)0x00010000
#define MIC_CODR           (u32)0x0000A000
#define Silence_SODR       (u32)0x00008000
#define Silence_CODR       (u32)0x00002000
#define Test_v_SODR        (u32)0x0000A000
#define Test_v_CODR        (u32)0x00010000

#define Sound_u_SODR       (u32)0x00001800
#define Sound_u_CODR       (u32)0x00004000
#define Sound_d_SODR       (u32)0x00001000
#define Sound_d_CODR       (u32)0x00004800

#define INC_H_SODR         (u32)0x00001000
#define INC_H_CODR         (u32)0x00000000
#define INC_L_SODR         (u32)0x00000000
#define INC_L_CODR         (u32)0x00001000

#define Re_sign_D          (u32)0x00000000
#define Re_sign_C          (u32)0x00000010
#define Re_adc_D           (u32)0x00000010
#define Re_adc_C           (u32)0x00000000

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/



/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
void ChangeSign(u8 u8Mode_);
void Sound_up(void);
void Sound_down(void);
void Delay_us(u8 u8time_);
void Test(void);
/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_Idle(void);    

static void UserApp1SM_Error(void);         


#endif /* __USER_APP1_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

