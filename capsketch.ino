//A0 1/10th of Vcc - used to autorange - Always connected to voltage divided supply rail
//A1 measure reference supply voltage - Connected to capacitor supply side through relay
//A2 measure capacitor voltage - Connected to capacitor drain side through relay
//A3 
//Power to be supplied to arduino
//14 pF capacitance on analogread
//D0
//D1
//D2 int 0  - Reset to base state
//D3 int 1  - Perform measurement
//D4        - Relay enable - 30v range
//D5        - Relay enable - 15v range
//D6        - Relay enable - 5v range
//D7        - Relay enable - 1v range
//D8        - Relay enable - 100 ohm
//D9        - Relay enable - 1000 ohm
//D10       - Relay enable - 10000 ohm
//D11       - Relay enable - 100000 ohm
//D12       - Relay enable - 1 megaohm


/*Hardware abstraction*/

class outPin
{
  public:
    void set(bool onVal)
    {
      on = onVal;
      digitalWrite(hardwarePin, on);
    };
    bool get()
    {
      return on;
    };
    outPin(int pinNo)
    {
      hardwarePin = pinNo;
      digitalWrite(hardwarePin, LOW);
      on = false;
    }
  private:
    bool on;
    int hardwarePin; 
};

class inPin
{
  public:
    int get(bool refresh, const int &time)
    {
      if (refresh) //only update reading when we ask for it, we don't want to spam read the discharching cap as it may affect the result
      {
        lastVal = analogRead(hardwarePin);
        lastTime = millis();
      }
      return lastVal;
    }
    inPin(int pinNo)
    {
      hardwarePin = pinNo;
      lastVal = -1;
      lastTime = 0;
    }
  private:
    int hardwarePin;
    int lastVal;
    int lastTime;
};
/* End hardware abstraction */



void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
inPin autoRange(A0);
inPin rangedSupply(A1);
inPin rangedCapSide(A2);
outPin relay_30v(4);
outPin relay_15v(5);
outPin relay_5v(6);
outPin relay_1v(7);
outPin relay_100ohm(8);
outPin relay_1kohm(9);
outPin relay_10kohm(10);
outPin relay_100kohm(11);
outPin relay_1mohm(12);
attachInterrupt(digitalPinToInterrupt(2), resetButton, FALLING);
attachInterrupt(digitalPinToInterrupt(3), startButton, FALLING);
int startTime = millis();
}

void loop() {
//Just dump measurements to the screen repeatedly until we interupt for measurement
}


void resetButton()
{
  return;
}

void startButton()
{
  return;
}




