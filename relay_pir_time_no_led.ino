/* 
 * //////////////////////////////////////////////////
 * //making sense of the Parallax PIR sensor's output
 * //////////////////////////////////////////////////
 *
 * Switches a LED according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 *
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time, 
 * which might give the impression no motion is present. 
 * This code will shut off the led and relay when the PIR sensor does not 
 * detect any movement with a a designated period of time. 
 * It will turn on the relay and LED when it detects movement. 
 * 
 */

/////////////////////////////
// VARIABLES
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

boolean lockLow = true;
boolean takeLowTime;  

// Pin Assignments 
int pirPin = 7;    // the digital pin connected to the PIR sensor's output
int relay = 8;  // This will be the relay

long unsigned int kill = 30000; // relay will turn off load if pir hasn't been activated in 30 seconds
long unsigned int ontime = 0;



/////////////////////////////
//SETUP
void setup(){
  
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(relay, OUTPUT); 
  digitalWrite(pirPin, LOW);
  

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  
    // This will delay our sensor for 30000 milliseconds, giving it time to calibrate
    // Then the sensor will be active.
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
    }
    
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){
  
     if(digitalRead(pirPin) == HIGH && digitalRead(relay) == HIGH){
        Serial.println("Rest ON time");
        ontime = millis();
     }
  
     // if pir is triggered AND the relay is off, go ahead and turn it on.
     if(digitalRead(pirPin) == HIGH && digitalRead(relay) == LOW){
       
       Serial.println("Relay ON");
       digitalWrite(relay, HIGH);
       ontime = millis();
       
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(20);
         }   
         
         takeLowTime = true; // means that there was a transition from HIGH to LOW 
       }

      // Here we use the led to visualize when the PIR sensor is low. If the LED is off
      // That means the PIR does not currently detect any movement.
      if(digitalRead(pirPin) == LOW){    
   

        if(takeLowTime){
         lowIn = millis();          //save the time of the transition from high to LOW
         takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       //if((!lockLow && millis() - lowIn > kill) && ontime > 30000){  
       if( ontime > kill ){  
         
           // makes sure this block of code is only executed again after 
           // a new motion sequence has been detected. When can just use millis() in the 
           // place of current time that way the time resets itself with out needing to keep track.
           lockLow = true;         
           digitalWrite(relay, LOW); // turns relay off
           Serial.print("motion ended at ");      // output
           Serial.print((millis() - kill)/1000);  // converts millsecs to secs
           Serial.println(" sec");
           Serial.println("Its been 30 seconds, Turning Relay OFF.");
           delay(50);
        }
      }
  }
