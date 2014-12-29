// Max Range 142, 244, 100

#include <structdefine.h>
#include <string.h>
#include <TimerOne.h>

String runMode = "Two";

int cntTime = 0;

#define StrLenMax  6
String StrData;
char InData[StrLenMax];
boolean flag = true;

void setup()
{
	Serial.begin(115200);
	// DDRB setting 1=output, 0=input
	DDRB = 0x0f;	//pin 0~7	
	DDRC = 0x0f;	//pin A0~A5
	DDRD = 0x3f;	//pin 8~13
	
        GoHome();
 
	Timer1.initialize( 5000 ); //slow 50k, med 10k, fast 5k
        Timer1.attachInterrupt( timerIsr );  
        Serial.print( "OK" );
}

sTime itime = { 0, 0, 0,
                0, 0 ,0 ,0 };

sCNCData CNC = { 0, 0, 0, 
                 0, 0, 0,
                 0, 0, 0,                 
                 1, 1, 1,
                 0, 0, 0,
                 0, 0, 0,
                 ""
          };

void loop()
{
    if( (Serial.available()>4) )
    {
        flag = false;
        StrData = Serial.readString();

        StrData.toCharArray( InData, StrLenMax );

        itime.Hour = atoi( subStr( InData, ":", 1 ) );
        itime.Min = atoi( subStr( InData, ":", 2 ) );  
        
        divideNum( itime.Hour, itime.Min );
        Clean();
        SwitchNum( itime.H1, 0, 80 );
        SwitchNum( itime.H2, 35, 80 );
        SwitchNum( itime.M1, 70, 80 );
        SwitchNum( itime.M2, 105, 80 );
 
        Serial.print( "OK" );
    }
}

void timerIsr()
{	
    runMotorX( CNC.xDir, CNC.xSet );
    runMotorY( CNC.yDir, CNC.ySet );
    runMotorZ( CNC.zDir, CNC.zSet );
}

void run( int x, int y, int z )
{
    Setflag( false );
    
    if( x > CNC.xPos )
        CNC.xDir = 1;
    if( x < CNC.xPos )
        CNC.xDir = -1;
    if( y > CNC.yPos )
        CNC.yDir = 1;
    if( y < CNC.yPos )
        CNC.yDir = -1;
    if( z > CNC.zPos )
        CNC.zDir = 1;
    if( z < CNC.zPos )
        CNC.zDir = -1;
    
    CNC.xSet = x;
    CNC.ySet = y;
    CNC.zSet = z;

    while(1)
    {
        delay(50);
        if( CNC.xOk && CNC.yOk && CNC.zOk )
        {
            cntTime++;
            CNC.temp = cntTime;
            break;
            if( cntTime > 10 )  break;
            if( (CNC.xSet==CNC.xPos) && (CNC.ySet==CNC.yPos) && (CNC.zSet==CNC.zPos) )
                break;  
        }
        cntTime++;
        if( cntTime > 10 )  break;
        CNC.temp = cntTime;
    }
    cntTime = 0;
}

void ShowData()
{
    Serial.print( "xDir = " );
    Serial.print( CNC.xDir );	
    Serial.print( " yDir = " );
    Serial.print( CNC.yDir );	
    Serial.print( " zDir = " );
    Serial.print( CNC.zDir );	
    Serial.print( " xOk = " );
    Serial.print( CNC.xOk );	
    Serial.print( " yOk = " );
    Serial.print( CNC.yOk );	
    Serial.print( " zOk = " );
    Serial.print( CNC.zOk );	
    Serial.print( " Temp = " );
    Serial.print( CNC.temp );	
    Serial.print( " Str = " );
    Serial.print( CNC.str );	
    Serial.print( " Xpos = " );
    Serial.print( CNC.xPos );
    Serial.print( " Ypos = " );
    Serial.print( CNC.yPos );	
    Serial.print( " Zpos = " );
    Serial.print( CNC.zPos );
	
    Serial.println();	
}

// ----------------- MotorModule -----------------
void Setflag( boolean i )
{
    CNC.xOk = i;
    CNC.yOk = i;
    CNC.zOk = i;
}

void runMotorX( int dir )
{
	boolean XSW = RangeLimit( "Xaxis", dir );
	PORTD = ( LimitSW( XSW, "Xaxis", runMode, dir  ) ) << 2;
}

void runMotorY( int dir )
{
	boolean YSW = RangeLimit( "Yaxis", dir );
	PORTB = LimitSW( YSW, "Yaxis", runMode, dir );
}

void runMotorZ( int dir )
{
	boolean ZSW = RangeLimit( "Zaxis", dir );
	PORTC = LimitSW( ZSW, "Zaxis", runMode, dir );
}

void runMotorX( int dir, int xcoord )
{   
    if( (dir>0) && (CNC.xPos >= xcoord) ) CNC.xOk = true;
    else if( (dir<0) && (CNC.xPos <= xcoord) ) CNC.xOk = true;
	else CNC.xOk = false;  
    
	boolean XSW = RangeLimit( "Xaxis", dir );
	
	if( (dir > 0) && (CNC.xPos < xcoord) )
    {
        PORTD = ( LimitSW( XSW, "Xaxis", runMode, dir  ) ) << 2;
	}
    else if( (dir < 0) && (CNC.xPos > xcoord) )
    {
		PORTD = ( LimitSW( XSW, "Xaxis", runMode, dir  ) ) << 2;
    }
    else
        xMotorStop();
}

void runMotorY( int dir, int ycoord )
{ 
    if( (dir>0) && (CNC.yPos >= ycoord) ) CNC.yOk = true;
    else if( (dir<0) && (CNC.yPos <= ycoord) ) CNC.yOk = true;
	else CNC.yOk = false;    
    
	boolean YSW = RangeLimit( "Yaxis", dir );
    
    if( (dir > 0) && (CNC.yPos < ycoord) )
    {
        PORTB = LimitSW( YSW, "Yaxis", runMode, dir );
	}
    else if( (dir < 0) && (CNC.yPos > ycoord) )
    {
		PORTB = LimitSW( YSW, "Yaxis", runMode, dir );
    }
    else
        yMotorStop();
}

void runMotorZ( int dir, int zcoord )
{
    if( (dir>0) && (CNC.zPos >= zcoord) ) CNC.zOk = true;
    else if( (dir<0) && (CNC.zPos <= zcoord) ) CNC.zOk = true;
	else CNC.zOk = false;        

	boolean ZSW = RangeLimit( "Zaxis", dir );
    
    if( (dir > 0) && (CNC.zPos < zcoord) )
    {
        PORTC = LimitSW( ZSW, "Zaxis", runMode, dir );
	}
    else if( (dir < 0) && (CNC.zPos > zcoord) )
    {
		PORTC = LimitSW( ZSW, "Zaxis", runMode, dir );
    }
    else
        zMotorStop();
}

void MotorTest( int dir )	//pos X, Y, Z
{
	runMotorX( dir );
	runMotorY( dir );
	runMotorZ( dir );
}	

boolean RangeLimit( String StrAxis, int dir )
{
	if( StrAxis == "Xaxis" )
	{
		int inHome = ( PIND & 0x80 ) >> XLimitHome;	//read pin
		int inEnd  = ( PIND & 0x40 ) >> XLimitEnd;
		
		if( (inHome == Xhome) && (inEnd != Xend) && (dir>0) )
			return true;
		else if( (inHome == Xhome) && (inEnd != Xend) && (dir<0) )
			return false;
		else if( (inHome != Xhome) && (inEnd == Xend) && (dir>0) )
			return false;
		else if( (inHome != Xhome) && (inEnd == Xend) && (dir<0) )
			return true;
		else
			return true;
	}

	if( StrAxis == "Yaxis" )
	{
		int inHome = ( PINB & 0x20 ) >> YLimitHome;	//read pin
		int inEnd  = ( PINB & 0x10 ) >> YLimitEnd;
		
		if( (inHome == Yhome) && (inEnd != Yend) && (dir>0) )
			return true;
		else if( (inHome == Yhome) && (inEnd != Yend) && (dir<0) )
			return false;
		else if( (inHome != Yhome) && (inEnd == Yend) && (dir>0) )
			return false;
		else if( (inHome != Yhome) && (inEnd == Yend) && (dir<0) )
			return true;
		else
			return true;
	}
	if( StrAxis == "Zaxis" )
	{
		int inHome = ( PINC & 0x10 ) >> ZLimitHome;	//read pin

		if( (inHome == Zhome) && (dir>0) )
			return true;
		else if( (inHome == Zhome) && (dir<0) )
			return false;
		else if( (CNC.zPos >= zStep) && (dir>0)  )
			return false;
		else if( (CNC.zPos >= zStep) && (dir<0)  )
			return true;
		else
			return true;
	}
}	

byte LimitSW( int SW, String StrAxis, String StrMode, int dir )
{
	if( SW > 0 )  return SwitchMode( StrAxis, StrMode, dir );
    else  return 0;		
}

byte SwitchMode( String StrAxis, String StrMode, int dir )
{
	if( StrMode == "One" )	return OnePhase( StrAxis, dir );
	if( StrMode == "Two" )	return TwoPhase( StrAxis, dir );
	if( StrMode == "OneTwo" )	return OneTwoPhase( StrAxis, dir );
}
int SetSwitchAxisCnt( String StrAxis )
{
	int CntTemp;
	if( StrAxis == "Xaxis" )  CntTemp = CNC.xCnt;	
	if( StrAxis == "Yaxis" )  CntTemp = CNC.yCnt;
	if( StrAxis == "Zaxis" )  CntTemp = CNC.zCnt;
	return CntTemp;
}

void GetSwitchAxisCnt( String StrAxis, int CntTemp )
{
	if( StrAxis == "Xaxis" )  CNC.xCnt = CntTemp;	
	if( StrAxis == "Yaxis" )  CNC.yCnt = CntTemp;
	if( StrAxis == "Zaxis" )  CNC.zCnt = CntTemp;
}

void UpdataPos( String StrAxis, int dir )
{
	if( dir >0 )
	{
		if( StrAxis == "Xaxis" )
			CNC.xPos++;
		if( StrAxis == "Yaxis" )  
			CNC.yPos++;
		if( StrAxis == "Zaxis" )  
			CNC.zPos++;
	}
	else
	{
		if( StrAxis == "Xaxis" )  
			CNC.xPos--;
		if( StrAxis == "Yaxis" )  
			CNC.yPos--;
		if( StrAxis == "Zaxis" )  
			CNC.zPos--;
	}
    
    if( CNC.xPos < 0 )
        CNC.xPos = 0;
    if( CNC.yPos < 0 )
        CNC.yPos = 0;
    if( CNC.zPos < 0 )
        CNC.zPos = 0;
        
}

byte OnePhase( String StrAxis, int dir )
{
	int CntTemp = SetSwitchAxisCnt( StrAxis );
	
	if( dir > 0 )
	{
		CntTemp++;
		UpdataPos( StrAxis, dir );
		if( CntTemp > 3 )  CntTemp = 0;	
	}
	else
	{
		CntTemp--;
		UpdataPos( StrAxis, dir );
		if( CntTemp < 0 )  CntTemp = 3;
	}
	GetSwitchAxisCnt( StrAxis, CntTemp );
	
	return OneCnt[CntTemp];
}

byte TwoPhase( String StrAxis, int dir )
{
	int CntTemp = SetSwitchAxisCnt( StrAxis );
	
	if( dir > 0 )
	{
		CntTemp++;
		UpdataPos( StrAxis, dir );
		if( CntTemp > 3 )  CntTemp = 0;
	}
	else
	{
		CntTemp--;
		UpdataPos( StrAxis, dir );
		if( CntTemp < 0 )  CntTemp = 3;
	}
	GetSwitchAxisCnt( StrAxis, CntTemp );
	
	return TwoCnt[CntTemp];
}

byte OneTwoPhase( String StrAxis, int dir )
{	
	int CntTemp = SetSwitchAxisCnt( StrAxis );
	
	if( dir > 0 )
	{
		CntTemp++;
		UpdataPos( StrAxis, dir );
		if( CntTemp > 7 )  CntTemp = 0;
	}
	else
	{
		CntTemp--;
		UpdataPos( StrAxis, dir );
		if( CntTemp < 0 )  CntTemp = 7;
	}
	GetSwitchAxisCnt( StrAxis, CntTemp );
	
	return OneTwoCnt[CntTemp];
}

void MotorStop()
{
    xMotorStop();
    yMotorStop();
    zMotorStop();
}

void xMotorStop()
{
    PORTD = 0;
}

void yMotorStop()
{
    PORTB = 0;
}

void zMotorStop()
{
    PORTC = 0;
}

void GoHome()
{
    run( 10, 10, 10 );
    while(1)
    {            
        boolean xinHome = ( PIND & 0x80 ) >> XLimitHome;	//read pin 7
        boolean yinHome = ( PINB & 0x20 ) >> YLimitHome;	//read pin 7
        boolean zinHome = ( PINC & 0x10 ) >> ZLimitHome;	//read pin 7

        if( (xinHome == Xhome) && (yinHome == Yhome) && (zinHome == Zhome) )
            break;

        runMotorX( -1 );
        runMotorY( -1 );
        runMotorZ( -1 );
        if( xinHome == Xhome )  xMotorStop();
        if( yinHome == Yhome )  yMotorStop();
        if( zinHome == Zhome )  zMotorStop();
     
        delay(10);
    }
}

// --------------- MotorModule End -----------------


// --------------- Digit -----------------
#define x1dig 13
#define x2dig 25
#define y1dig 50
#define y2dig 100
#define zclean 100
#define zdraw 50 
#define zup 0 

void Clean()
{
    CNC.str = "Clr";
    run( 20, 5, zup );
    run( 20, 5, zclean );
    delay(100);
    run( 120, 0, zclean );
    run( 120, 0, zup );
    run( 30, 30, zup );
    Setflag( true );
}

void Zero( int xoffset, int yoffset )
{
    CNC.str = "0";
    int x = xoffset;
    int y = yoffset;
    
    run( x, y, 0 );
    run( x, y, zdraw );
    run( x+x2dig, y, zdraw);
    run( x+x2dig, y+y2dig, zdraw);
    run( x, y+y2dig, zdraw);
    run( x, y, zdraw);
    run( x, y, 10);
    
    Setflag( true );
}

void One( int xoffset, int yoffset )
{
    CNC.str = "1";
    int x = xoffset;
    int y = yoffset;
    
    run( x+x1dig, y, zup );
    run( x+x1dig, y, zdraw );
    run( x+x1dig, y+y2dig, zdraw );
    run( x+x1dig, y+y2dig, zup );
    
    Setflag( true );
}

void Two( int xoffset, int yoffset )
{
    CNC.str = "2";
    int x = xoffset;
    int y = yoffset;
    
    run( x+x2dig, y, zup );
    run( x+x2dig, y, zdraw );
    run( x, y, zdraw );
    run( x, y+y1dig, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x, y+y2dig, zup );
    
    Setflag( true );
}

void Three( int xoffset, int yoffset )
{
    CNC.str = "3";
    int x = xoffset;
    int y = yoffset;
    
    run( x, y, zup );
    run( x, y, zdraw );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x, y+y2dig, zup );
    
    Setflag( true );
}

void Four( int xoffset, int yoffset )
{
    CNC.str = "4";
    int x = xoffset;
    int y = yoffset;
    
    run( x+x2dig, y, zup );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x+x2dig, y+y2dig, zup );
    
    run( x, y+y2dig, zup );
    run( x, y+y2dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x+x2dig, y+y1dig, zup );
    
    Setflag( true );
}

void Five( int xoffset, int yoffset )
{
    CNC.str = "5";
    int x = xoffset;
    int y = yoffset;

    run( x, y, zup );
    run( x, y, zdraw );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x+x2dig, y+y2dig, zup );
    
    Setflag( true );
}

void Six( int xoffset, int yoffset )
{
    CNC.str = "6";
    int x = xoffset;
    int y = yoffset;
    
    run( x+x2dig, y+y2dig, zup );
    run( x+x2dig, y+y2dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x, y, zdraw );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x, y+y1dig, zup );
    
    Setflag( true );
}

void Seven( int xoffset, int yoffset )
{
    CNC.str = "7";
    int x = xoffset;
    int y = yoffset;
    
    run( x+x2dig, y, zup );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x, y+y2dig, zup );
    
    Setflag( true );
}

void Eight( int xoffset, int yoffset )
{
    CNC.str = "8";
    int x = xoffset;
    int y = yoffset;
    
    run( x+x2dig, y+y1dig, zup );
    run( x+x2dig, y+y1dig, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x, y, zdraw );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x, y+y1dig, zup );
    
    Setflag( true );
}

void Nine( int xoffset, int yoffset )
{
    CNC.str = "9";
    int x = xoffset;
    int y = yoffset;
    
    run( x, y, zup );
    run( x, y, zdraw );
    run( x+x2dig, y, zdraw );
    run( x+x2dig, y+y2dig, zdraw );
    run( x, y+y2dig, zdraw );
    run( x, y+y1dig, zdraw );
    run( x+x2dig, y+y1dig, zdraw );
    run( x+x2dig, y+y1dig, zup );
    
    Setflag( true );
}

void SwitchNum( int num, int xoffset, int yoffset )
{
    switch( num )
    {
        case 0:
        {
            Zero( xoffset, yoffset );
            break;
        }
        case 1:
        {
            One( xoffset, yoffset );
            break;
        }
        case 2:
        {
            Two( xoffset, yoffset );
            break;
        }
        case 3:
        {
            Three( xoffset, yoffset );
            break;
        }
        case 4:
        {
            Four( xoffset, yoffset );
            break;
        }
        case 5:
        {
            Five( xoffset, yoffset );
            break;
        }
        case 6:
        {
            Six( xoffset, yoffset );
            break;
        }
        case 7:
        {
            Seven( xoffset, yoffset );
            break;
        }
        case 8:
        {
            Eight( xoffset, yoffset );
            break;
        }
        case 9:
        {
            Nine( xoffset, yoffset );
            break;
        }
    }
}

// --------------- Digit End -----------------

// --------------- Others --------------------
void divideNum( int in1, int in2 )
{
    int x1;
    int x2;
        
    if( in1 < 10 )
    {
        itime.H1 = 0;
        itime.H2 = in1;
    }
    else
    {
        x1 = in1 / 10;
        x2 = in1 % 10;
        itime.H1 = x1;
        itime.H2 = x2;
    }
    
    if( in2 < 10 )
    {
        itime.M1 = 0;
        itime.M2 = in2;
    }
    else
    {
        x1 = in2 / 10;
        x2 = in2 % 10;
        itime.M1 = x1;
        itime.M2 = x2;
    }        
}

char* subStr (char* str, char *delim, int index) {
  char *act, *sub, *ptr;
  static char copy[StrLenMax];
  int i;

  strcpy(copy, str);

  for (i = 1, act = copy; i <= index; i++, act = NULL) {
    sub = strtok_r(act, delim, &ptr);
    if (sub == NULL) break;
  }
  return sub;
}

// --------------- Others End --------------------
