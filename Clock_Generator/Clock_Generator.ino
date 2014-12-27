#include <LiquidCrystal.h>
#include <TimerOne.h>

LiquidCrystal lcd( 10, 9, 8, 7, 6, 5 );

// ---Time process parameter -----------
typedef struct sTime
{
    int Hour, Min, Sec;
    int H1, H2, M1, M2;
};

String StrData = "";

String timeTemp;
String HourStr = "00";
String MinStr = "00";
String SecStr = "00";

String MinStrTemp = "";

sTime itime = { 0, 0, 0,
                0, 0, 0, 0 };
sTime itimeSet = { 0, 0, 0,
                   0, 0, 0, 0};
                   
// ---Time process parameter end -----------

// ---Debounce parameter -----------
volatile boolean SetSW = false;
volatile int digitCnt = 0;

long debouncing_time1 = 400; //Debouncing Time in Milliseconds
volatile unsigned long last_micros1;
long debouncing_time2 = 400; //Debouncing Time in Milliseconds
volatile unsigned long last_micros2;
// ---Debounce parameter End -----------

boolean updateFlag = true;

void setup()
{
    Serial.begin( 115200 );
    lcd.begin( 16, 2 );
    
    pinMode( 2, INPUT );
    pinMode( 3, INPUT );
    
    pinMode( 4, OUTPUT );
    pinMode( 13, OUTPUT );
    
    Timer1.initialize( 500000 );
    Timer1.attachInterrupt( timerIsr );
    attachInterrupt( 0, debounceInterrupt1, FALLING );
    attachInterrupt( 1, debounceInterrupt2, FALLING );
    lcd.print( "   CNC  Clock" );
}

int x1 = 0;
int x2 = 0;
String str;

void loop()
{
    if( SetSW )
        Clock();
    else
        SetClock();
}

void SetClock()
{
    int timeValue = analogRead( A5 );
    timeValue = map( timeValue, 0, 1023, 0, 7 );
    SetNum( timeValue );
    lcd.setCursor( 0, 1 );
    lcd.blink();

    String timeSetStr = int2string(itimeSet.Hour) + ":" + 
                        int2string(itimeSet.Min) + ":" + 
                        int2string(itimeSet.Sec);
    
    lcd.print( timeSetStr );

    delay(100);
}

void SetNum( int value )
{
    
    switch( digitCnt )
    {
        case 0:
        {
            if( updateFlag )
            {
                itimeSet = itime;
                updateFlag = false;
            }
            int HourTemp = itimeSet.Hour % 10;
            itimeSet.Hour = ( value*10 ) + HourTemp;
            break;
        }
        case 1:
        {
            int HourTemp = itimeSet.Hour / 10;
            itimeSet.Hour = value + HourTemp*10;
            break;
        }
        case 2:
        {
            int MinTemp = itimeSet.Min % 10;
            itimeSet.Min = ( value*10 ) + MinTemp;
            break;
        }
        case 3:
        {
            int MinTemp = itimeSet.Min / 10;
            itimeSet.Min = value + MinTemp*10;
            break;
        }        
        case 4:
        {
            int SecTemp = itimeSet.Sec % 10;
            itimeSet.Sec = ( value*10 ) + SecTemp;
            break;
        }
        case 5:
        {
            int SecTemp = itimeSet.Sec / 10;
            itimeSet.Sec = value + SecTemp*10;
            break;
        }
        case 6:
        {
            itime = itimeSet;
            SetUpdate();
            digitCnt = 0;
            updateFlag = true;
            SetSW = true;
            break;
        }
    
    }
}

String int2string( int x )
{
    if( x < 9 )  return "0" + String( x );
    else  return String( x );
}

void SetUpdate()
{
    HourStr = int2string( itime.Hour );
    MinStr = int2string( itime.Min );
    SecStr = int2string( itime.Sec );
}

void timerIsr()
{	
    if( SetSW )  timeFunc();
}

//---------------- Debounce ----------------
void debounceInterrupt1()
{
  if((long)(micros() - last_micros1) >= debouncing_time1 * 1000) 
  {
    SW1Func();
    last_micros1 = micros();
  }
}

void debounceInterrupt2()
{
  if((long)(micros() - last_micros2) >= debouncing_time2 * 1000) 
  {
    SW2Func();
    last_micros2 = micros();
  }
}

void SW1Func()
{
    SetSW = !SetSW;
}

void SW2Func()
{
    if( ~SetSW )
    {
        if( digitCnt > 6 )  digitCnt = 0;
        digitCnt++;
    }
}

//---------------- Debounce End ----------------

//---------------- Time Process ----------------
void Clock()
{
    lcd.noBlink();
    digitalWrite( 13, LOW );
    lcd.setCursor( 0, 1 );
    timeTemp = HourStr + ":" + MinStr + ":" + SecStr; 

    lcd.print( timeTemp );
    
    if( MinStr != MinStrTemp )
    {
        SentData();
        MinStrTemp = MinStr;
    }
    delay(100);
}

void SentData()
{
    if( Serial.available()>0 )
    {
        StrData = Serial.readString();
        if( StrData == "OK" )
            Serial.print( timeTemp );
    }
}

void showTime()
{
    Serial.print( itime.Hour );
    Serial.print( " : " );
    Serial.print( itime.Min );
    Serial.print( " : " );
    Serial.print( itime.Sec );
    Serial.println();    
}

void timeFunc()
{
    if( itime.Hour >= 24 )
    {
        itime.Hour = 0;
        HourStr = "00";
    }

    if( itime.Min >= 60 )
    {
        itime.Min = 0;
        MinStr = "00";
        itime.Hour++;
        if( itime.Hour < 10 )
            HourStr = "0" + String( itime.Hour );
        else
            HourStr = String( itime.Hour );
    }

    if( itime.Sec >= 60 )
    {
        itime.Sec = 0;
        SecStr = "00";
        itime.Min++;
        if( itime.Min < 10 )
            MinStr = "0" + String( itime.Min );
        else
            MinStr = String( itime.Min );
    }
   
    if( itime.Sec < 10 )
        SecStr = "0" + String( itime.Sec );
    else
        SecStr = String( itime.Sec );
        
    itime.Sec++;    
}
//---------------- Time Process  End ----------------
