#include <math.h>
/*Hardware abstraction*/
class outPin
{
  public:
    void set(bool onVal)
    {
      on = onVal;
      digitalWrite(hardwarePin, on);
      delay (10); //Wait 10 ms for physical relay to switch, specs say this takes 3-5 ms.
    };
    float getOhms()
    {
      return ohms;
    }

    bool get()
    {
      return on;
    };
    outPin(int pinNo) //divider constructor
    {
      hardwarePin = pinNo;
      digitalWrite(hardwarePin, LOW);
      on = false;
    }
    outPin(int pinNo, float value)  //resistor constructor
    {
      hardwarePin = pinNo;
      digitalWrite(hardwarePin, LOW);
      on = false;
      ohms = value;
    }
  private:
    bool on;
    int hardwarePin; 
    float ohms;
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


//A0 1/10th of Vcc - used to autorange - Always connected to voltage divided supply rail
inPin autoRange(A0);
//A1 measure reference supply voltage - Connected to capacitor supply side through relay
inPin rangedSupply(A1);
//A2 measure capacitor voltage - Connected to capacitor drain side through relay
inPin rangedCapSide(A2);
//A3 
//Power to be supplied to arduino
//14 pF capacitance on analogread
//D0 NOPE - SERIAL
//D1 NOPE - SERIAL
//D2 int 0  - Measurement Button - declared in setup()
//D3 int 1  - 
//D4        - Relay enable - 30v range
outPin relay_30v(4);
//D5        - Relay enable - 15v range
outPin relay_15v(5);
//D6        - Relay enable - 5v range
outPin relay_5v(6);
//D7        - Relay enable - supply to cap side
outPin relay_capSupply(13);

outPin * resistorArray[5];
//D8        - Relay enable - 10m ohm
outPin relay_10mohm(8,10000000);

//D9        - Relay enable - 1m ohm
outPin relay_1mohm(9,1000000);
//D10       - Relay enable - 100k ohm
outPin relay_100kohm(10,100000);
//D11       - Relay enable - 10k ohm
outPin relay_10kohm(11,10000);
//D12       - Relay enable - 1k ohm
outPin relay_1kohm(12,1000);

//D13       - 
int startTime = millis();
outPin *curDivider = NULL;
void setup() {
Serial.begin(115200);
attachInterrupt(digitalPinToInterrupt(2), startButton, FALLING);
}

void loop() {
  //Dump current voltage reading using internal representation (0-1023 == 0-30v)
delay(200);
Serial.println(autoRange.get(1));
Serial.println("\n");
resistorArray[0] = & relay_10mohm;
resistorArray[1] = & relay_1mohm;
resistorArray[2] = & relay_100kohm;
resistorArray[3] = & relay_10kohm;
resistorArray[4] = & relay_1kohm;
}
void startButton()
{
    int rangeValue = autoRange.get(true); //Learn what the power supply value is (30V = 1024)
    int range = 0;
    //Set the curDivider pointer to the relay object we want to use for the current test
    if (rangeValue > 500) // More than 15V with a small amount of wiggle
    {      curDivider = &relay_30v;}
    else  {curDivider = &relay_15v;}
    
    //Less than 5v not implemented, need dual power supplies to deal with this properly, replace input to linear regulator with a second supply
  //At this point rangedSupply and rangedCapSide should be properly voltage limited 
  
  //Get an approximate value of Tau, we basically just spam read the voltage until a threshold voltage has been reached
    curDivider->set(HIGH);
    
    //Voltage divider set, cap charged.
    
    bool trigger = false;
    int resistorIndex = 4;
    int startValue;
    for (; (resistorIndex == 0) || trigger; resistorIndex--) //Loop until we find a slow enough discharge
    {
      chargeCap();
      startTime = millis();
      startValue = rangedCapSide.get(1);
      resistorArray[resistorIndex]->set(HIGH);
      do
      {
        delay (20);
      } while (rangedCapSide.get(1) > startValue * .9);
      resistorArray[resistorIndex]->set(LOW);
      if ((millis() - startTime) >= 1000) //If this took longer than 1s then this is the resistor we want to use
        {trigger = true;}
    }

    //Full 10 second test
    chargeCap(); //Cap charged
    startValue = rangedCapSide.get(1);
    resistorArray[resistorIndex]->set(HIGH); //Discharge begins
    delay (10000); //Discharge for 10 seconds
    resistorArray[resistorIndex]->set(LOW); //Disconnect resistor
    int endValue = rangedCapSide.get(1);

    float capacitance = (-1)/(log(endValue/startValue));
    
    Serial.println ("****************************\n");
    Serial.println ("Capacitance is\n");
    Serial.println (capacitance);
    Serial.println ("\n****************************");

    

}

void chargeCap()
{
  //Detect when the cap is fully charged to supply voltage. This will check the voltage every 100 ms and continue
  //Cap should charge extremely fast unless current limited by power supply
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



