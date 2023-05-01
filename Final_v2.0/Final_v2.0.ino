/*********************      **********************
  10 to GPS Module TX        A4 to I2C Module SDA
  09 to GPS Module RX        A5 to i2C Module SCL
 *********************      **********************/

/*--------------------------------------------------------------------------------------------------------------------------------------
  ------------- COLOMBO FORT-RAGAMA JUNCTION TRAIN ROUTE AUTOMATION DISPLAY SYSTEM -----------------------------------------------------
  --------------------------------------------------------------------------------------------------------------------------------------*/

#include <SD.h>
#include <Wire.h>
#include <TMRpcm.h>
#include <TimeLib.h>
#include <TinyGPS.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

TinyGPS gps;
TinyGPSPlus gpsp;
LiquidCrystal_I2C lcd(0x27, 16, 4);

#define Number_of_Stations  4         // Change the value for other Routes
#define wait 5000                     // Change the duration of Display time
#define S_RX    D1                    // define software serial RX pin
#define S_TX    D0                     // define software serial TX pin
#define SD_ChipSelectPin 8            // define CS pin

TMRpcm sound;                         //crete an object for speaker library

SoftwareSerial mySerial(S_RX, S_TX);  // configure SoftSerial library

#define time_offset   19800  // define a clock offset of 3600 seconds (1 hour) ==> UTC + 1

// variable definitions
char Time[]  = "TIME: 00:00:00";
char Date[]  = "DATE: 00-00-2000";

long lat, lon;
byte last_second, Second, Minute, Hour, Day, Month;
int Year, Return, count = 0, control = 0, ReturnList[Number_of_Stations];
int sort[Number_of_Stations] = {0};
bool voice1, voice2 = false;

char* StationList[Number_of_Stations] = {"Colombo Fort", "Maradana", "Dematagoda", "Ragama"};

double Lat[2 * Number_of_Stations] = {6933500, 6933580, 6928900, 6929600, 6937180, 6937790, 7028700, 7030300};
double Lon[2 * Number_of_Stations] = {79849500, 79850050, 79864750, 79866360, 79878690, 79879370, 79921250, 79921390};

// Example 1 -->> Colombo Fort Station's Possible Latitude cordinates (10^5) are in {Lat[0] - Lat[1]} Range
// Example 2 -->> Colombo Fort Station's Possible Longtitude cordinates (10^5) are in {Lon[0] - Lon[1]} Range

char* Now_voice[Number_of_Stations] = {"Colombo.wav", "Maradana.wav", "Dematagoda.wav", "Ragama.wav"};
char* Nxt_voice[Number_of_Stations] = {"ColomboN.wav", "MaradanaN.wav", "DematagodaN.wav", "RagamaN.wav"};

void gpsdump(TinyGPS &gps);
void dTime();
void print_wday();
void statCheck();

void setup(void)
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  lcd.begin();
  lcd.backlight();

  Serial.println("<<<<<---------------COLOMBO FORT-RAGAMA JUNCTION TRAIN ROUTE AUTOMATION DISPLAY SYSTEM--------------->>>>>");
  Serial.println("----------------------------------------------------------------------------------------------------------");
  Serial.println();

  sound.speakerPin = 9;                  //define speaker pin.
  sound.quality(1);                      // 0 or 1  -- 1 equals 2x oversampling
  sound.setVolume(5);                    //0 to 7. Set volume level
}

void loop()
{
  bool newdata = false;

  while (1)
  {
    if (mySerial.available())
    {
      char c = mySerial.read();
      //Serial.print(c);  // uncomment to see raw GPS data

      if (gpsp.encode(c))
      {
        dTime();
      }
      if (gps.encode(c))
      {
        newdata = true;
        break;
      }
    }
  }

  if (newdata)
  {
    gpsdump(gps);
    statCheck();

    for (int i = 1; i <= Number_of_Stations; i++)
    {
      if (count != Number_of_Stations)
      {
        if (Return == i)
        {
          lcd.clear();

          lcd.setCursor(0, 0);
          lcd.print("------NOW-------");
          lcd.setCursor(-4, 1);
          lcd.print(StationList[sort[count - 1]]);
          lcd.setCursor(4, 2);
          lcd.print("Station");
          lcd.setCursor(-4, 3);
          lcd.print("----------------");

          if (voice1 != true)
          {
            sound.stopPlayback();
            sound.play(Now_voice[sort[count - 1]]);

            voice1 = true;
          }

          delay(wait);
          lcd.clear();

          lcd.setCursor(0, 0);
          lcd.print("------NEXT------");
          lcd.setCursor(-4, 1);
          lcd.print(StationList[sort[count]]);
          lcd.setCursor(4, 2);
          lcd.print("Station");
          lcd.setCursor(-4, 3);
          lcd.print("----------------");

          if (voice2 != true)
          {
            sound.stopPlayback();
            sound.play(Nxt_voice[sort[count]]);

            voice2 = true;
          }

          delay(wait);
          lcd.clear();

        }
      }
      else
      {
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("---DESTINATION--");
        lcd.setCursor(-4, 1);
        lcd.print(StationList[sort[count - 1]]);
        lcd.setCursor(4, 2);
        lcd.print("Station");
        lcd.setCursor(-4, 3);
        lcd.print("----------------");

        if (voice1 != true)
        {
          sound.stopPlayback();
          sound.play(Now_voice[sort[count - 1]]);

          voice1 = true;
        }
      }
    }

    if (Return == 0)
    {
      // print time & date
      print_wday(weekday());   // print day of the week
      lcd.setCursor(0, 1);     // move cursor to column 0 row 2
      lcd.print(Time);         // print time (HH:MM:SS)
      lcd.setCursor(-4, 2);     // move cursor to column 0 row 3
      lcd.print(Date);         // print date (DD-MM-YYYY)

      if (control != 0)
      {
        lcd.setCursor(-4, 3);
        lcd.print("NEXT: ");
        lcd.setCursor(2, 3);
        lcd.print(StationList[sort[count]]);
      }
      else
      {
        lcd.setCursor(-4, 3);
        lcd.print("________________");
      }
    }
    Return = 0;
  }
}
