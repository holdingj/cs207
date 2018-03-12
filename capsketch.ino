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
//D13       - Relay enable - supply to cap


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
    int get(bool refresh)
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
outPin relay_capSupply(13);

int startTime = millis();

void setup() {
Serial.begin(115200);
attachInterrupt(digitalPinToInterrupt(2), resetButton, FALLING);
attachInterrupt(digitalPinToInterrupt(3), startButton, FALLING);
}

void loop() {
//Just dump measurements to the screen repeatedly until we interupt for measurement
}


void resetButton()
{
  relay_30v.set(LOW);
  relay_15v.set(LOW);
  relay_5v.set(LOW);
  relay_1v.set(LOW);
  relay_100ohm.set(LOW);
  relay_1kohm.set(LOW);
  relay_10kohm.set(LOW);
  relay_100kohm.set(LOW);
  relay_1mohm.set(LOW);
  startTime = millis();

  
  return;
}

void startButton()
{
  int rangeValue = autoRange.get(true); //Learn what the power supply value is (30V = 1024)
  int range = 0;

  if (rangeValue > 512) // More than 15V
  {
    relay_30v.set(HIGH);
    range = 1;
  }
  else if (rangeValue > 200) // More than 5V
  {
    relay_15v.set(HIGH);
    range = 2;
  }
  else if (rangeValue > 33) // More than 1V
  {
    relay_5v.set(HIGH);
    range = 3;
  }
  else if (rangeValue > 0) // More than grounded
  {
    relay_1v.set(HIGH);
    range = 4;
  }

//At this point rangedSupply and rangedCapSide should be properly voltage limited


  chargeCap(); 
  startTime = millis();
/*    relay_100ohm.set(LOW);
  relay_1kohm.set(LOW);
  relay_10kohm.set(LOW);
  relay_100kohm.set(LOW);
  relay_1mohm.set(LOW); */
  
  
  

}

void chargeCap()
{
  //Detect when the cap is fully charged to supply voltage. This will check the voltage every 100 ms and continue
  relay_capSupply.set(HIGH); //Connect cap to supply
  
  int delta = 1023;
  int measValue = rangedCapSide.get(true);
  while (delta > 0)
  {
    delay(100);
    delta = rangedCapSide.get(true) - measValue;
    measValue = rangedCapSide.get(false);
  }
  relay_capSupply.set(LOW);
}




