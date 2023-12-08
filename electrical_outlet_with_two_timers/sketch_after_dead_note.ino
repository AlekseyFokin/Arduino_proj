#include <GyverTM1637.h>
#include <EEPROM.h>
GyverTM1637 disp(10,8);

//Работа с постоянной памятью, метки о том что скетч загружался хоть раз на плату и начальные значения таймеров
#define INIT_ADDR 300
#define INIT_KEY 50
#define TIME_TO_WORK_ADDR 110
#define TIME_TO_WORK_INIT 2
#define TIME_TO_REST_ADDR 210
#define TIME_TO_REST_INIT 4

// настройки кнопок
int button1In=3;
bool button1WasUp = true; //работа +1 минута

int button2In=4;
bool button2WasUp = true; //работа -1 минута

int button3In=5;
bool button3WasUp = true; //отдых +1 минута

int button4In=6;          //работа -1 минута
bool button4WasUp = true;

int timeToWork=1; //переменные куда загружаются данные из памяти
int timeToRest=22;

int outRelePin=2;    // пин реле
bool rele;//метка реле включено - true
unsigned long start; //время переключения реле
long totalTimeToWork;
long totalTimeToRest;

void setup() {
  //проверка первого запуска то есть проверяет есть ли в памяти уже записи
  if (EEPROM.read(INIT_ADDR)!=INIT_KEY) {
    EEPROM.write(INIT_ADDR, INIT_KEY);  //проставил отметку при первом запуске и сохранил начальные данные
    EEPROM.put(TIME_TO_WORK_ADDR,TIME_TO_WORK_INIT);
    EEPROM.put(TIME_TO_REST_ADDR,TIME_TO_REST_INIT);
  }
EEPROM.get(TIME_TO_WORK_ADDR,timeToWork); //загрузка из памяти таймаутов работы и отдыха
EEPROM.get(TIME_TO_REST_ADDR,timeToRest);

   disp.clear(); //настройка дисплея
   disp.brightness(7);
  
  pinMode(button1In, INPUT_PULLUP); //включение кнопок
  pinMode(button2In, INPUT_PULLUP);
  pinMode(button3In, INPUT_PULLUP);
  pinMode(button4In, INPUT_PULLUP);

  pinMode(outRelePin, OUTPUT); //включение реле
  rele=true;
  digitalWrite(outRelePin, HIGH);
  start=millis(); //старт периода
  //Serial.begin(9600);
 totalTimeToWork=((long)timeToWork)*1000*60;//приведение типов таймаутов
 totalTimeToRest=((long)timeToRest)*1000*60;
}


void loop() {

//unsigned long currentMillis=millis();// текущее время
 
//Serial.print("current=");Serial.print(currentMillis);
//Serial.print(" start= ");Serial.print(start);
//Serial.print(":  current-start=");Serial.print(currentMillis-start);
//Serial.print(" <> "); 
//if (rele) {Serial.print(" TimeToWork= ");Serial.print(timeToWork);Serial.print("=>");Serial.println(totalTimeToWork);} 
//else{Serial.print(" TimeToRest= ");Serial.println(timeToRest);}

prinToScreen(timeToWork,timeToRest);//вывод на экран
if(rele==true) {//если реле включено 
                if ((millis()-start)>totalTimeToWork){//и время работы вышло - выключаю реле и запускаю отсчет периода отдыха
                                                          rele=false;
                                                          digitalWrite(outRelePin, LOW);
                                                          start=millis();
                                                        //  Serial.println(" Выключаю ");
                                                        }
}

if(rele==false) { // если реле выключено
                if ((millis()-start)>totalTimeToRest){ // и время отдыха вышло  - включаю реле и запускаю отсчет периода работы
                                                          rele=true;
                                                          digitalWrite(outRelePin, HIGH);
                                                          start=millis();
                                                          //Serial.println(" Включаю ");
                                                        }
}

if (readButton(button1In,button1WasUp)&&(timeToWork<99)) {timeToWork=timeToWork+1;  // увеличение периода работы нажатием на кнопку с сохранением в пзу
                                                          EEPROM.put(TIME_TO_WORK_ADDR,timeToWork);
                                                          totalTimeToWork=((long)timeToWork)*1000*60;//приведение типов таймаутов
                                                        //start=millis();
                                                         // return;
                                                         } 
if (readButton(button2In,button2WasUp)&&(timeToWork>1)) {timeToWork=timeToWork-1;// уменьшение периода работы нажатием на кнопку с сохранением в пзу
                                                         EEPROM.put(TIME_TO_WORK_ADDR,timeToWork);
                                                         totalTimeToWork=((long)timeToWork)*1000*60;//приведение типов таймаутов
                                                         //start=millis();
                                                         //return;
                                                         }
if (readButton(button3In,button3WasUp)&&(timeToRest<99)) {timeToRest=timeToRest+1; // увеличение периода отдыха нажатием на кнопку с сохранением в пзу
                                                          EEPROM.put(TIME_TO_REST_ADDR,timeToRest);
                                                          totalTimeToRest=((long)timeToRest)*1000*60;
                                                        //start=millis();
                                                        //  return;
                                                        }
if (readButton(button4In,button4WasUp)&&(timeToRest>1)) {timeToRest=timeToRest-1;// уменьшение периода отдыха нажатием на кнопку с сохранением в пзу
                                                         EEPROM.put(TIME_TO_REST_ADDR,timeToRest);
                                                         totalTimeToRest=((long)timeToRest)*1000*60;
                                                         //start=millis();
                                                         //return;
                                                         }
}
  
bool readButton(int port,bool& buttonWasUp)
{
// читаем инвертированное значение для удобства
  bool buttonIsUp = digitalRead(port);
  if (buttonWasUp && !buttonIsUp) {
    delay(10);
    buttonIsUp = digitalRead(port);
    if (!buttonIsUp) {
                      buttonWasUp=buttonIsUp;
                      return true;
                     }
  else {
         buttonWasUp=buttonIsUp;
         return false;
        }
  }
  buttonWasUp=buttonIsUp;
  return false;
  
}

void prinToScreen(int timeToWork, int timeToRest){// индикация на мониторе

int one=0;
int two=0;
int three=0;
int four=0;

  if (timeToWork>9) {one=timeToWork/10; two=timeToWork % 10;}
     else {two=timeToWork;}

  if (timeToRest>9) {three=timeToRest/10; four=timeToRest % 10;}
     else {four=timeToRest;}  
 disp.display(one,two,three,four);     

}
