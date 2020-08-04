#define BOUNCE_WITH_PROMPT_DETECTION
#include <SoftwareSerial.h>
#include <Bounce2.h>

#define midiIn 4
#define midiOut 5

#define backPin 14 
#define playPin 15
#define forwardPin 16
#define undoPin 17
#define tapPin 18

#define fiveVoltPin 12
//expression pedal sense pin A7

#define ledPin 13

#define micLoopStartPin 6
#define micLoopStopPin 7
#define micLoopTrigPin1 9
#define micLoopTrigPin2 8

unsigned long lastTapFallTime;
unsigned long tapFallTime;
unsigned long tapElapsedTime;
unsigned long beatTime;
unsigned long pulseTime;
unsigned long firstBeat;
int pulseCount;

int expPedal;
int lastExpPedal;
byte expState;
byte lastExpState;
byte parts; //contains data for 8 parts on electribe

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

#define debounceInterval 25

Bounce tap = Bounce();
Bounce play = Bounce();
Bounce forward = Bounce();
Bounce back = Bounce();
Bounce undo = Bounce();
Bounce micLoopTrigIn1 = Bounce();
Bounce micLoopTrigIn2 = Bounce();

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

  pinMode(A7, INPUT);//A7 = expPedalSensePin

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

  undo.attach(undoPin, INPUT_PULLUP);
  undo.interval(debounceInterval);

  micLoopTrigIn1.attach(micLoopTrigPin1, INPUT_PULLUP);
  micLoopTrigIn1.interval(debounceInterval);

  micLoopTrigIn2.attach(micLoopTrigPin2, INPUT_PULLUP);
  micLoopTrigIn2.interval(debounceInterval);

  expState = 'H';
  parts = B11111111;
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
        
        expPedal = analogRead(A7);
                           
        if(expPedal<120){
          expState = 'H';//High energy     
          }
        else if (expPedal>900){
          expState = 'L';//Low energy
          }
        else {
          expState = 'M';//Medium energy
          }

        if (expState != lastExpState){
          Serial.write(expState);
          lastExpState = expState;
          }
        
        if (micLoopStartRequest && currentBeat == 0){
          digitalWrite(micLoopStartPin, LOW); //grounding the pin triggers on the Ditto Mic Looper
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
          }
        }
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
        playing = !playing;
      }
      else{}

      forward.update();
      if ( forward.fell() ){
        bankAndPatternByte++;
        bankBit = bankAndPatternByte>>7;
        patternByte = bankAndPatternByte & 127;
        write3(0xB9, 0, 0);
        write3(0xB9, 0x20, bankBit);
        write2(0xC9, patternByte);
        }
      else {}

      back.update();
      if ( back.fell() ){
        bankAndPatternByte--;
        bankBit = bankAndPatternByte>>7;
        patternByte = bankAndPatternByte & 127;
        write3(0xB9, 0, 0);
        write3(0xB9, 0x20, bankBit);
        write2(0xC9, patternByte);
        }
       undo.update();
       if ( undo.fell() ){
        write3(0xBF, 79, 127);
        }
       else {}

       micLoopTrigIn1.update();
       if ( micLoopTrigIn1.fell() ){
        micLoopStartRequest = true;
        }
        
       micLoopTrigIn2.update();
       if ( micLoopTrigIn2.fell() ){
        //micLoopShortRequest = true;
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
