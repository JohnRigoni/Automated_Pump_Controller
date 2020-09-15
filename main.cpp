
// Little disclaimer, this code was nicely organized in different
// .ino files that I put together and used in the Arduino IDE. No
// longer will my technological potential be burdened by remnants
// of text editor past. I transfered to platform.io, an extension
// in VScode, which gave me propper syntax correction, helping me
// tie off a few loose ends.


#include <Arduino.h>
#include <avr/sleep.h>
#include <DS3232RTC.h>
#include <SPI.h>
#include <SD.h>
#include <avr/wdt.h>

void inter();
int interval_check(int chour);
void allign_interval(int current_interval);
void alarm_set(int number, int digit);
void Relay_Enable();
void Relay_Disable();
void Setting_Alarm_Hour(int sleep_hour) ;
void Setting_Alarm_Minute(int sleep_minute);
void Setting_Alarm_Second(int sleep_second);
void Going_To_Sleep();
void logg(float bat);
float battery();
void reseting();
void on_and_off(int current_interval, float bat_lev );
void sd();
bool validate_data(File dataFile);
void array_sort();


void(* resetFunc) (void) = 0;

int time_start[4] = {};
int time_end[4] = {};
int time_on[4] = {};
int time_off[4] = {};
int sleep_until;
time_t t;
time_t first_wake;
long glo_count = 0;
boolean state = LOW; //Boolean that alternates High and Low each interrupt


void setup() {
  MCUSR = MCUSR & B11110111;
  wdt_enable(WDTO_8S);


  pinMode(2, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(6, OUTPUT);

  digitalWrite(8, HIGH);
  digitalWrite(7, LOW);
  delay(15);
  digitalWrite(8, LOW);



  digitalWrite(3, HIGH);
  digitalWrite(A1, HIGH);
  delay(500);
  digitalWrite(3, LOW);
  digitalWrite(A1, LOW);
  delay(500);

  //    // START initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.squareWave(SQWAVE_NONE);
  RTC.alarmInterrupt(ALARM_1, true);  //Enable interrupt output for Alarm 1
  //  // END  initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags


  digitalWrite(3, HIGH);
  digitalWrite(A1, HIGH);
  delay(500);
  digitalWrite(3, LOW);
  digitalWrite(A1, LOW);
  delay(500);

  attachInterrupt(digitalPinToInterrupt(2), inter, FALLING);


  sd();
  array_sort();

  for ( int j = 0; j < 4; j++) {

  }

  t = RTC.get();
  first_wake = t;
}

int current_interval = 0;
int previous_interval = 0;


void loop() {

  float bat_lev = battery();

  previous_interval = current_interval;
  current_interval = interval_check(hour(t));


  if (current_interval == 9) {
    int time_till = hour(t) + 1;
    while (interval_check(time_till) == 9) {
      time_till++;
      if (time_till == 24) {
        time_till -= 24;
      }
    }

    alarm_set(time_till, 1);
    glo_count = 1;
  }

  else if (glo_count == 0) {  
    allign_interval(current_interval);
  }
  else {   
    on_and_off(current_interval, bat_lev);
  }
  glo_count++;
}


void  inter() {

  sleep_disable();  //Disable sleep mode
  cli();            //Removes the interrupt from pin 2 to ensure only 1 interrupt is called
}


// Will return the current interval.  Will return 9 if no current interval
int interval_check(int chour){  

  if ((time_start[0] == 0) & (time_end[0] == 0)){
    return 0;
  }

    for (int i = 0; i < 4; i++){
      if (time_start[i] < time_end[i]){
        if ((chour >= time_start[i]) & (chour < time_end[i])){
          

          return i;
        }
      }

      if (time_start[i] > time_end[i]){
        if ((chour >= time_start[i]) | (chour < time_end[i])){
          return i;
        }
      } 
    }
  return 9;
}


void allign_interval(int current_interval){

  bool s = false;
  
  if (second(t) == 0){
    s = true;
  }

  int hour_hold = time_start[current_interval];
  int hour_adjust=0;
  int min_hold = 0;
  int cycle=0;
    
    while (hour(first_wake) > hour_hold){
      min_hold += time_off[current_interval];
      cycle++;
      
      while (min_hold >= 60){
        hour_hold++;
        min_hold -= 60;
      }
      
    }


    if((hour_hold > 0) & ((min_hold == 0 ) & s)){
      return;
    }

    if( min_hold != 0){
        if((minute(first_wake) >= min_hold) & (((minute(first_wake) % min_hold)  == 0) & (s))){
          return;     
        }
    }

      bool ger = true;


    if (minute(first_wake) >= min_hold ){    
        
        while ((minute(first_wake) >= min_hold) & (ger == true)){
          min_hold += time_off[current_interval];
               
          while(min_hold >= 60){
            hour_adjust++;
            min_hold -= 60;

          }
          if (min_hold == 0){
                min_hold =  time_off[current_interval];
                ger = false;   
          }
        }
 
        if(hour_adjust > 0){

          hour_adjust += hour(first_wake);
          if (hour_adjust == 24){
            hour_adjust -= 24;
          }
          
          alarm_set(hour_adjust, 1);
          
        }
     
        if (min_hold > 0){

          alarm_set(min_hold, 2);
        }
        return;

     }

    else if(minute(first_wake) < min_hold){ 

      alarm_set(min_hold, 2);
      return;      
    }
    
}


int number_;
int e_min_;
int glo_num;

// This function will set the alarm to precisly 
// the next wakeup interval
    //for int digit, 1=hour, 2=minute, 3=second
void alarm_set(int number, int digit){

  glo_num = number;
  int e_min=0;
  int e_hour=0;

  if (digit == 3){
    
    while (number >= 60){
      number -= 60;
      e_min++;
    }

    number_ = number;
    e_min_ = e_min;

    if (e_min > 0){

      e_min += minute(t);
      if(e_min >= 60){
        e_min -= 60;
      }
      
      Setting_Alarm_Minute(e_min);
      Going_To_Sleep();
    }

    if (number > 0){
        if (number >= 60){
          number -= 60;
        }
        Setting_Alarm_Second(number);
        Going_To_Sleep();
    }    
  }


  else if (digit == 2){

    while (number  >= 60){
      number  -= 60;
      e_hour++;
    }
    
    if (e_hour > 0){
      e_hour += hour(t);
      if(e_hour >= 24){
        e_hour -= 24;
      }
      Setting_Alarm_Hour(e_hour);
      Going_To_Sleep();
    }
    
    if (number > 0){     

     Setting_Alarm_Minute(number);
     Going_To_Sleep();    
    }
    
  }
  
  else{

    if (number >= 24){
      number -= 24;
    }
    
    Setting_Alarm_Hour(number);
    Going_To_Sleep();
    
  }
}


void Relay_Enable(){

   digitalWrite(7, HIGH);     //Will turn relay_pin1 to high or low depending on state
   digitalWrite(8, LOW);  
   delay(25);                           //Ensures enough time for pins to change before next steps
   digitalWrite(7, LOW); 
    state = !state;   
  
}

void Relay_Disable(){

   digitalWrite(8, HIGH);     //Will turn relay_pin1 to high or low depending on state
   digitalWrite(7, LOW);  
   delay(25);                           //Ensures enough time for pins to change before next steps
   digitalWrite(8, LOW);  
    state = !state;  
  
}


void Setting_Alarm_Hour(int sleep_hour) {

  RTC.setAlarm(ALM1_MATCH_HOURS , 0, 0, sleep_hour, 0);
  RTC.alarm(ALARM_1);
}

void Setting_Alarm_Minute(int sleep_minute) {

  RTC.setAlarm(ALM1_MATCH_MINUTES , 0, sleep_minute, 0, 0);
  RTC.alarm(ALARM_1);
}


void Setting_Alarm_Second(int sleep_second) {

  RTC.setAlarm(ALM1_MATCH_SECONDS , sleep_second, 0, 0, 0);
  RTC.alarm(ALARM_1);
}


void Going_To_Sleep(){

  delay(100);
  
   sleep_enable();                       //Enabling sleep mode
   sei();                                //Enable interrupts
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //Setting the sleep mode, in our case full sleep
   wdt_reset();
   wdt_disable();
   sleep_cpu();                          //Activating sleep mode

   
   wdt_reset();  
   MCUSR = MCUSR & B11110111;
   wdt_enable(WDTO_4S);
   delay(10);


   t = RTC.get();

}


void logg(float bat){

  float bat_lev = (float)(roundf(bat * 10)) / 10;

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {

    dataFile.print(month(t));
    dataFile.print(" ");
    dataFile.print(day(t));
    dataFile.print(" ");
    dataFile.print(year(t));
    dataFile.print(" ");
    dataFile.flush();
    dataFile.print(hour(t));
    dataFile.print(" ");
    dataFile.print(minute(t));
    dataFile.print(" ");
    dataFile.print(second(t));
    dataFile.print(" ");
    dataFile.print(state);
    dataFile.print(" ");
    dataFile.println(bat_lev);
    
  }
  dataFile.close();
}


float battery(){

  return 7.5;

  int level = analogRead(A2);

  if (level <= 645){

      File dataFile = SD.open("datalog.txt", FILE_WRITE);

      if (dataFile) {

          
          dataFile.print(month(t));
          dataFile.print(" ");
          dataFile.print(day(t));
          dataFile.print(" ");
          dataFile.print(year(t));
          dataFile.print(" ");
          dataFile.flush();
          dataFile.print(hour(t));
          dataFile.print(" ");
          dataFile.print(minute(t));
          dataFile.print(" ");
          dataFile.print(second(t));
          dataFile.print(" ");
          dataFile.flush();
          dataFile.print("Low battery, shutting down. Minimum is 6.2 volts "); 
          dataFile.flush();
          dataFile.print("Currently at ");
          dataFile.print((float)(roundf((((level/204.8)*2) * 10)+1)) / 10); 
          dataFile.print(" volts");
          dataFile.print("b @ ");
          dataFile.println(level); 
                 
      }
      dataFile.close();

      delay(100);

  digitalWrite(8, HIGH);

      delay(100);
    
    digitalWrite(6, HIGH);
    delay(1000);
    
  }

  return ((level/204.8)*2);
}



void reseting() {

  int next_reset = hour(first_wake) + 1;


  if (next_reset >= 24) {
    next_reset -= 24;
  }

  if (hour(t) == next_reset) {

    File debug = SD.open("debug.txt", FILE_WRITE);
    if (debug) {

      debug.println();
      debug.println(" ");
      debug.println("I just reset");
      debug.println();
    }

    debug.close();
    resetFunc();
  }

}


time_t t_prev;

void on_and_off(int current_interval, float bat_lev ){

  int num;

      Relay_Enable();      
      logg(bat_lev);
      num = second(t) + time_on[current_interval];
      t_prev = t;
      alarm_set(num, 3);
      
      Relay_Disable();
      logg(bat_lev);
      reseting();

      if(current_interval == interval_check(hour(t))){
          
          num = minute(t_prev) + time_off[current_interval];
          alarm_set(num, 2);

    }
      
}


int count=0;
volatile int first_dig;
int num_hold;
int j = 0;
char nully[10] = {'0','1','2','3','4','5','6','7','8','9'};
char empty[1] = {'\0'};

int boi[16];

void sd() {

  // see if the card is present and can be initialized:
  while (!SD.begin(10)) {

    // don't do anything more:
    delay(750);
  }
  

  int i = 0;
  
  int n;
  for (n = 0; n <16;n++){
    
    boi[n] = 99;
  }
  
  // byte garbage;
  bool valid;
  bool truth = false;
  File dataFile = SD.open("set.txt");


  if (dataFile) {
    
    while (dataFile.available()) {          
        valid = validate_data(dataFile);       

        int current_num;
        if ((valid) & (count != 2) & (count != 3) & (count != 4)){
          current_num = num_hold - '0';
          truth = true;
        } 

            
        int second_dig;
        if ((valid) & (count == 2 )){
          second_dig = num_hold - '0';
          current_num = (current_num * 10) + second_dig;         
        }      


         int third_dig;
         if ((valid) & (count == 3) ){
          third_dig = num_hold - '0';
          current_num = (current_num * 10) + third_dig;         
        }   


         int fourth_dig;
         if ((valid) & (count == 4 )){
          fourth_dig = num_hold - '0';
          current_num = (current_num * 10) + fourth_dig;         
        }   
         if ((valid) & (count == 3) ){
          third_dig = num_hold - '0';
          current_num = (current_num * 10) + third_dig;         
        }   


         if ((valid) & (count == 4 )){
          fourth_dig = num_hold - '0';
          current_num = (current_num * 10) + fourth_dig;         
        }   


         
        if (!valid){
          if (truth == true){
            boi[i] = current_num;
            i++;          
          }
          truth = false;
          // garbage = dataFile.read();
          dataFile.read();
          count = 0;
        }
        
    } 
  dataFile.close(); 
  }

}


bool validate_data(File dataFile){

  
  for(j = 0; j<11; j++){ 
            
      if ((nully[j] == dataFile.peek())  &&  (empty[0] == dataFile.peek())){
                  num_hold = dataFile.read();
                  count++;
                  return true;
                }
    
  }
  return false;
}


void array_sort(){

  int j;
  int b=0;
  for ( j=0; j<4; j++){

    if(boi[b] != 99){  
    time_start[j] = boi[b]; 
    b++;
    time_end[j] = boi[b];
    b++;
    time_on[j] = boi[b];
    b++;
    time_off[j] = boi[b];
    b++;
    
  }
  }
}

