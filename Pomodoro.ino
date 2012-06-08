#include <LiquidCrystal.h>
#include <Time.h>  
#include <Timer.h>

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

#define POMODORO_TIME 270000 // 45 minutos of work...

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
Timer t;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Time to Work...");
  Serial.begin(9600); 
  
  t.after( POMODORO_TIME, pomodoroIsUp );
  t.every( 1000, syncTime );
}

void syncTime( ) {
  if(Serial.available() ) 
  {
    processSyncMessage();
  }
  if(timeStatus()!= timeNotSet) 
    Serial.println("waiting for sync message");
  else     
      digitalClockDisplay();  
}

void pomodoroIsUp( ) {
  lcd.setCursor(0,0);
  lcd.print("POMODORO!!!    ");
}

void loop() {    
  t.update();
}

void digitalClockDisplay() {
  lcd.setCursor(0,1);
  lcd.print(hour());
  printDigits(minute());
  printDigits(second());
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
    char c = Serial.read() ; 
    lcd.print(c);  
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    }  
  }
}

