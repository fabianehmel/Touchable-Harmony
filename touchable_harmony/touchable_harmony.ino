// #########################
// #####  GLOBAL VARS  #####
// #########################

// ControlPins for Multiplexer 1
int mux1PinA = 0;
int mux1PinB = 1;
int mux1PinC = 2;
int mux1PinD = 3;

// ControlPins for Multiplexer 2
int mux2PinA = 4;
int mux2PinB = 5;
int mux2PinC = 6;
int mux2PinD = 7;

// Input- and Output-Pins for Data
int notePin = A4;
int ledPin = A5;
int masterPinA = A3;
int masterPinB = A2;

// Vars for handle data of row A
int noteA = 0;
int valA = 0;
int valAOrig = 0;
int baseA = 0;
int baseAOrig = 0;

// Vars for handle data of row B
int noteB = 0;
int valB = 0;
int valBOrig = 0;
int baseB = 0;
int baseBOrig = 0;

// Vars for remember the current and last position
int i = 1;
int last_i = 0;

// Configuration-Array for Multiplexer 1
int mux1Array[16][4] = {
  {0, 0, 0, 0},
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {0, 0, 1, 0},
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {1, 1, 1, 0},
  {0, 0, 0, 1},
  {1, 0, 0, 1},
  {0, 1, 0, 1},
  {1, 1, 0, 1},
  {0, 0, 1, 1},
  {1, 0, 1, 1},
  {0, 1, 1, 1},
  {1, 1, 1, 1}
};

// Configuration-Array for Multiplexer 2
int mux2Array[8][4] = {
  {0, 0, 0, 0},
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {0, 0, 1, 0},
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {1, 1, 1, 0}
};

// Eight MIDI-Accords as Array
int akkordArray[8][8] = {
  {12, 14, 16, 17, 19, 21, 23, 24},
  {24, 26, 28, 29, 31, 33, 35, 36},
  {36, 38, 40, 41, 43, 45, 47, 48},
  {48, 50, 52, 53, 55, 57, 59, 60},
  {60, 62, 64, 65, 67, 69, 71, 72},
  {72, 74, 76, 77, 79, 81, 83, 84},
  {84, 86, 88, 89, 91, 93, 95, 96},
  {96, 98, 100, 101, 103, 105, 107, 108}
};

// Vars for the MIDI-Clock
byte counter; 
byte CLOCK = 248; 
byte START = 250; 
byte CONTINUE = 251; 
byte STOP = 252; 





// #########################
// ########  SETUP  ########
// #########################

void setup() {
  
  // Set Pin-Setup for Multiplexer 1
  pinMode(mux1PinA, OUTPUT);
  pinMode(mux1PinB, OUTPUT);
  pinMode(mux1PinC, OUTPUT);
  pinMode(mux1PinD, OUTPUT);

  // Set Pin-Setup for Multiplexer 2
  pinMode(mux2PinA, OUTPUT);
  pinMode(mux2PinB, OUTPUT);
  pinMode(mux2PinC, OUTPUT);
  pinMode(mux2PinD, OUTPUT);  

  // Set Pin-Setup for I/O-Pins  
  pinMode(notePin, INPUT);
  pinMode(masterPinA, INPUT);
  pinMode(masterPinB, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  
  // Set inital value for mux1Array
  digitalWrite(mux1PinA, mux1Array[i][0]);
  digitalWrite(mux1PinB, mux1Array[i][1]);
  digitalWrite(mux1PinC, mux1Array[i][2]);
  digitalWrite(mux1PinD, mux1Array[i][3]);
  
  // init mux1Array
  usbMIDI.setHandleRealTimeSystem(RealTimeHandler);
  
}




// #########################
// ########  LOOP  #########
// #########################

void loop() {
  // Read the MIDI
  usbMIDI.read();
}




// #########################
// ###  RealTimeHandler  ###
// #########################

void RealTimeHandler(byte realtimebyte) { 

  if (realtimebyte == CLOCK) { 
    
    counter++;
      
    // get Base Values
    baseAOrig = analogRead(masterPinA);
    baseA = map(baseAOrig, 1023, 0, 0, 7);
    baseBOrig = analogRead(masterPinB);
    baseB = map(baseBOrig, 1023, 0, 0, 7);    
    
    if (counter == 24) { 
      
      counter = 0;
      
      // Switch LED
      digitalWrite(mux2PinA, mux2Array[i][0]);
      digitalWrite(mux2PinB, mux2Array[i][1]);
      digitalWrite(mux2PinC, mux2Array[i][2]);
      digitalWrite(mux2PinD, mux2Array[i][3]);
      digitalWrite(ledPin, HIGH);
      
      // Switch Off old Notes
      if (valA>0) { usbMIDI.sendNoteOff(noteA, 100, 1); }
      if (valB>0) { usbMIDI.sendNoteOff(noteB, 100, 2); }

      // Read A
      valAOrig = analogRead(notePin);
      valA = map(valAOrig, 1023, 0, 0, 8);
      noteA = akkordArray[baseA][valA];

      // Switch 
      digitalWrite(mux1PinA, mux1Array[i+8][0]);
      digitalWrite(mux1PinB, mux1Array[i+8][1]);
      digitalWrite(mux1PinC, mux1Array[i+8][2]);
      digitalWrite(mux1PinD, mux1Array[i+8][3]);

      // Read B
      valBOrig = analogRead(notePin);
      valB = map(valBOrig, 1023, 0, 0, 8);
      noteB = akkordArray[baseB][valB];

      // 
      last_i = i;
      i++;
      if (i>7) { i = 0; }
           
      //
      Serial.print("#A");
      Serial.print(last_i+1);
      if (valA>0) {
        Serial.print("   Note: "); Serial.print(noteA);
        Serial.print("   Original Value: "); Serial.println(valAOrig);
        usbMIDI.sendNoteOn(noteA, 100, 1);
      } else {
        // Controller is down, no note
        Serial.println("   OFF");
      }
      
      //
      Serial.print("#B");
      Serial.print(last_i+1);
      if (valB>0) {
        Serial.print("   Note: "); Serial.print(noteB);
        Serial.print("   Original Value: "); Serial.println(valBOrig);
        usbMIDI.sendNoteOn(noteB, 100, 2);
      } else {
        // Controller is down, no note
        Serial.println("   OFF");
      }
      
      // Print Vars of Master-Controller
      Serial.print("Base A: "); Serial.println(baseAOrig);
      Serial.print("Base B: "); Serial.println(baseBOrig);
      
      // Print an empty line
      Serial.println("");
      
      // Print a divider when reached last controller
      if (last_i==7) { Serial.println(""); Serial.println(""); Serial.println("--------------------"); Serial.println(""); Serial.println(""); }
      
      // Write new configuration to Multiplexer 1
      digitalWrite(mux1PinA, mux1Array[i][0]);
      digitalWrite(mux1PinB, mux1Array[i][1]);
      digitalWrite(mux1PinC, mux1Array[i][2]);
      digitalWrite(mux1PinD, mux1Array[i][3]);

    }     
  } 

  // Reset counter
  if (realtimebyte == START || realtimebyte == CONTINUE) {  counter = 0; } 

}
