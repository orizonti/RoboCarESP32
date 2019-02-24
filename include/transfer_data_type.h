
#include "stdint.h"
typedef struct 
{
   uint16_t HEADER1;
   uint16_t HEADER2;
   uint16_t SIZE_UNIT;
} HEADER_STRUCT;


typedef struct 
{
   HEADER_STRUCT HEADER;

   uint16_t Speed1;
   uint16_t Speed2;
   uint16_t Speed3;
   uint16_t Speed4;
} DC_MotorControlStruct;

typedef struct 
{
   HEADER_STRUCT HEADER;

   uint16_t AccelX;
   uint16_t AccelY;
   uint16_t AccelZ;

   uint16_t AngularSpeedX;
   uint16_t AngularSpeedY;
   uint16_t AngularSpeedZ;
} AccelerometerStruct;

typedef struct 
{
   HEADER_STRUCT HEADER;

   uint16_t AngleMotor1;
   uint16_t AngleMotor2;
   uint16_t AngleMotor3;

   uint16_t SpeedMotor1;
   uint16_t SpeedMotor2;
   uint16_t SpeedMotor3;
} StepMotorControlStruct;

typedef struct 
{
   HEADER_STRUCT HEADER;

  uint16_t RangeRigth;
  uint16_t RangeLeft;
} RangeControlStruct;

typedef struct 
{
   HEADER_STRUCT HEADER;

  uint16_t CurrentConsumption;
  uint16_t BatteryVoltage;
} BatterControlStruct;

