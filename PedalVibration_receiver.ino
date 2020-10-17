
int OUT1_A = 2;
int OUT1_B = 3;
int ENABLE_PIN1 = 5;

int OUT2_A = 6;
int OUT2_B = 7;
int ENABLE_PIN2 = 9;

int POTI_PIN = 14;

int lastGasVibSpeed = 0;
int lastBrakeVibSpeed = 0;

int potiValue = 0;
float percentage = 0.0;

void setBrakeSpeed(int speed);
void setGasSpeed(int speed);

const char STATE_IDLE = 0;
const char STATE_READ_GAS_VALUE = 1;
const char STATE_READ_BRAKE_VALUE = 2;

char state = STATE_IDLE;

const int WHEEL_SLIP_ADDRESS = 0x00;
const int START_BIT = 0x80;

void setup()
{
  Serial.begin(9600);

  pinMode(OUT1_A, OUTPUT);
  pinMode(OUT1_B, OUTPUT);
  pinMode(ENABLE_PIN1, OUTPUT);

  pinMode(OUT2_A, OUTPUT);
  pinMode(OUT2_B, OUTPUT);
  pinMode(ENABLE_PIN2, OUTPUT);
  
  pinMode(POTI_PIN, INPUT);

  digitalWrite(OUT1_A, LOW);
  digitalWrite(OUT1_B, LOW);
  
  digitalWrite(OUT2_A, LOW);
  digitalWrite(OUT2_B, LOW);

  state = STATE_IDLE;
}

void readPotiValue()
{
  potiValue = analogRead(POTI_PIN);
  if (potiValue < 0)
  {
    potiValue = 0;
  }
  else if (potiValue > 1000)
  {
    potiValue = 1000;
  }
  
  percentage = ((float)potiValue / 1000.0);
}

void loop()
{
  if (Serial.available() > 0)
  {
    byte incomingByte[1];
    int dataCount = Serial.readBytes(incomingByte, 1);
    if (dataCount == 0)
    {
      return;
    }

    int receivedValue = incomingByte[0];

    if (state == STATE_IDLE)
    {
      if (receivedValue < START_BIT)
      {
        return;
      }

      if ((receivedValue - START_BIT) == WHEEL_SLIP_ADDRESS)
      {
        state = STATE_READ_GAS_VALUE;
      }
    }
    else if (state == STATE_READ_GAS_VALUE)
    {
      int gasSlip = receivedValue;
      int gasVibSpeed = 0;
      
      if (gasSlip > 8)
      {
        gasVibSpeed = 255;
      }
      else if (gasSlip > 0)
      {
        gasVibSpeed = (145 + (gasSlip * 15));
      }

      if (lastGasVibSpeed != gasVibSpeed)
      {
        readPotiValue();
        setGasSpeed(gasVibSpeed * percentage);
        lastGasVibSpeed = gasVibSpeed;
      }

      state = STATE_READ_BRAKE_VALUE;
    }
    else if (state == STATE_READ_BRAKE_VALUE)
    {
      int brakeSlip = receivedValue;
      int brakeVibSpeed = 0;
      
      if (brakeSlip > 8)
      {
        brakeVibSpeed = 255;
      }
      else if (brakeSlip > 0)
      {
        brakeVibSpeed = (145 + (brakeSlip * 15));
      }

      if (lastBrakeVibSpeed != brakeVibSpeed)
      {
        readPotiValue();
        setBrakeSpeed(brakeVibSpeed * percentage);
        lastBrakeVibSpeed = brakeVibSpeed;
      }

      state = STATE_IDLE;
    }
  }
}

void setBrakeSpeed(int speed)
{
  analogWrite(ENABLE_PIN1, speed);
  
  if (speed > 0)
  {
    digitalWrite(OUT1_A, HIGH);
  }
  else
  {
    digitalWrite(OUT1_A, LOW);
  }
}

void setGasSpeed(int speed)
{
  analogWrite(ENABLE_PIN2, speed);
  
  if (speed > 0)
  {
    digitalWrite(OUT2_A, HIGH);
  }
  else
  {
    digitalWrite(OUT2_A, LOW);
  }
}
