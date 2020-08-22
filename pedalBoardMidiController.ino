#define BOUNCE_WITH_PROMPT_DETECTION
#include <SoftwareSerial.h>
#include <Bounce2.h>

#define midiIn 4
#define midiOut 5

#define backPin 14 
#define playPin 15
#define forwardPin 16
//#define undoPin 17
#define tapPin 18

#define fiveVoltPin 12
#define expIn A7

#define ledPin 13

#define micLoopStartPin 6 //Out to ditto mic looper
#define micLoopStopPin 7 //Out to ditto mic looper
#define micLoopTriggerPin 17

unsigned long lastTapFallTime;
unsigned long tapFallTime;
unsigned long tapElapsedTime;
unsigned long beatTime;
unsigned long pulseTime;
unsigned long firstBeat;
int pulseCount;

int expPedal;
byte lastExpState;
byte parts;
byte lastParts;

int currentBeat;
bool micLoopStartRequest;
bool micLoopStarted;
bool micLoopShortRequest;
bool micLoopShortStarted;
int micLoopShortBeatStart;

unsigned long nextPulse;
byte bankAndPatternByte;
byte bankBit;
byte patternByte;

bool VOLCA; //make this true (1) if you're using the KORG Volca Sample with Pajen unofficial firmware (tested with beta7) and false (0) if not
bool ER1; //make this true (1) if you're using the KORG Electribe ER-1 and false (0) if not

//part 10 is closed highhat on Volca sample. 10 chokes 9 if both samples are played simultaneously, 9 can choke 10 if played subsequently

#define debounceInterval 25

Bounce tap = Bounce();
Bounce play = Bounce();
Bounce forward = Bounce();
Bounce back = Bounce();
Bounce micLoopTrigIn = Bounce();

bool playing;
SoftwareSerial midiSerial(midiIn, midiOut);

void setup() {
  midiSerial.begin(31250);
  Serial.begin(9600);

  tap.attach(tapPin, INPUT_PULLUP);
  tap.interval(debounceInterval);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  pinMode(micLoopStartPin, OUTPUT);
  pinMode(micLoopStopPin, INPUT);
  digitalWrite(micLoopStartPin, HIGH);
  digitalWrite(micLoopStopPin, HIGH);

  pinMode(fiveVoltPin, OUTPUT);
  digitalWrite(fiveVoltPin, HIGH);

  pinMode(expIn, INPUT);//A7 = expPedalSensePin

  beatTime = 600000;
  pulseTime = beatTime/24;
  pulseCount = 0;
  nextPulse = 0;

  currentBeat=0;
  micLoopStartRequest = false;
  micLoopStarted = false;

  playing = false;
  play.attach(playPin, INPUT_PULLUP);
  play.interval(debounceInterval);

  bankAndPatternByte = 0;
  bankBit = 0;
  patternByte = 1;
  forward.attach(forwardPin, INPUT_PULLUP);
  forward.interval(debounceInterval);

  back.attach(backPin, INPUT_PULLUP);
  back.interval(debounceInterval);

  micLoopTrigIn.attach(micLoopTriggerPin, INPUT_PULLUP);
  micLoopTrigIn.interval(debounceInterval);

  VOLCA = true; //make this true if you're using the KORG Volca Sample with Pajen unofficial firmware (tested with beta7) and false if not
  ER1 = false; //make this true if you're using the KORG Electribe ER-1 and false if not
  
  if(ER1){
    parts = 8;
    lastParts = 8;}
  else if (VOLCA){
    parts = 10;
    lastParts = 10;
    }
  else{}
}

void loop() {
  tap.update();
  if ( tap.fell() ) {
      lastTapFallTime = tapFallTime;
      tapFallTime = micros();
      tapElapsedTime = tapFallTime - lastTapFallTime;
      if (tapElapsedTime < 3000000){
        if (beatTime > 0){
          beatTime = (tapElapsedTime+beatTime)/2;
          pulseTime = beatTime/24;
          firstBeat = micros() + beatTime;
        }
        else {
          beatTime = tapElapsedTime;
          }
        }
      else {
        beatTime = 0;
        }
    }
    
    if (micros() >= nextPulse){
    nextPulse = pulseTime + micros();
    
      if (playing){midiSerial.write(0xF8);}
      else{}
    
      if (pulseCount == 0){
        digitalWrite(ledPin, HIGH);
               
        if (micLoopStartRequest && currentBeat == 0){
          digitalWrite(micLoopStartPin, LOW); //grounding the pin triggers the Ditto Mic Looper
          micLoopStarted = true;
          micLoopStartRequest = false;
        }        
      }
      else if (pulseCount == 1){
        digitalWrite(ledPin, LOW);
        if (micLoopStarted){
          micLoopStarted = false;
          digitalWrite(micLoopStartPin, HIGH);
          }
        }
        else{}
      pulseCount++;
      if (pulseCount == 24){
        pulseCount = 0;
        currentBeat++;
        if (currentBeat == 4){
          currentBeat = 0;
          }else{}
        }else{}
    }

    play.update();
    if ( play.fell() ){
      if(playing){
        midiSerial.write(0xFC);//send stop
        }
      else{
        pulseCount = 0;
        currentBeat = 0;
        midiSerial.write(0xFA);//send start
        }
        playing = !playing;//toggle playing state
      }
      else{}

      forward.update();
      if ( forward.fell() && ER1 ){
        bankAndPatternByte++;
        bankBit = bankAndPatternByte>>7;
        patternByte = bankAndPatternByte & 127;
        sendPatternChange(bankBit, patternByte);
        parts = 8;//set drums to high energy
        } 
        else {}

      back.update();
      if ( back.fell() && ER1 ){
        bankAndPatternByte--;
        bankBit = bankAndPatternByte>>7;
        patternByte = bankAndPatternByte & 127;
        sendPatternChange(bankBit, patternByte);
        parts = 8; //set drums to high energy
        } else {}

       micLoopTrigIn.update();
       if ( micLoopTrigIn.fell() ){
        micLoopStartRequest = true;
        }

        expPedal = analogRead(A7);
        if(expPedal<120){//High energy
          if (ER1){parts = 8;}  
          else if (VOLCA){parts = 9;} 
          }
        else if (expPedal>900){
          if (ER1){parts = 2;}//Low energy
          else if (VOLCA){parts = 3;}
          }
        else {//Medium energy
          if (ER1){parts = 4;}
          else if (VOLCA){parts = 5;}
          }

        if(parts != lastParts){
          if (ER1){       
            lastParts = parts;
            switch(parts){
              case 2:
                partsUpdate(0x0E, 0x37);
                break;
              case 4:
                partsUpdate(0x06, 0x36);
                break;
              case 8:
                partsUpdate(0x00, 0x00);
                break;
              }
          }
          else if (VOLCA){
            if (lastParts > parts){
              volcaUpdate(lastParts, 0);//mute
              lastParts--;
              }
            else if (lastParts < parts){
              volcaUpdate(lastParts + 1, 1);//unmute
              lastParts++;
              }
            else{}
            }
        }
}

void write3(byte byte1, byte byte2, byte byte3){
  midiSerial.write(byte1);
  midiSerial.write(byte2);
  midiSerial.write(byte3);
}

void write2(byte byte1, byte byte2){
  midiSerial.write(byte1);
  midiSerial.write(byte2);
}

void partsUpdate(byte byte1, byte byte2){
  write3(0xB9, 0x63, 0x02);
  write3(0xB9, 0x62, 0x6D);
  write3(0xB9, 0x06, byte1);
  write3(0xB9, 0x63, 0x02);
  write3(0xB9, 0x62, 0x6E);
  write3(0xB9, 0x06, byte2);
}

void volcaUpdate(byte channel, byte muteByte){
  write3((0xB0 + channel), 58, muteByte);
  }

void sendPatternChange(byte bankBit, byte patternByte){
  write3(0xB9, 0, 0);
  write3(0xB9, 0x20, bankBit);
  write2(0xC9, patternByte);
  }
