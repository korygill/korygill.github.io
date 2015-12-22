/*
  Author: Kory Gill (do-not-reply-kory@msn.com)
     WWW: http://korygill.net/korygillracing-retro/kory.htm/
    Date: 8/97

  This programs computes dew point and air density.
  It uses dew point calculations found on the WWW and air density based on
  a Doug Walby article in RRW&MT (Road Racing World and Motorcyle Technology).

  Note: I calculate the dew point directly from the temp and humidity.  These
        do not correspond to the values used in the article, but he may have
        been making those up anyway.


  NOTE: Please leave my comments as they are to give credit 
        to those who contributed to it.
*/

/*
  http://www.crest.org/efficiency/strawbale-list-archive/9602/msg00226.html

  >From a book, here are some approixamate dew point temperatures
  versus moisture in the air (in units of grains of water vapor per lb of air)
  
    TEMP, AMT MOISTURE
    ------------
    0F,  0  (accurate)
    20F, 20 (approiximate)  ...Zone 1: T=Vm
    Vm=T
    30F, 28 (approiximate)  ...Zone 2: T=[(30-20)/(28-8)]* Vm-20    =(Vm/2)+16
    Vm=(T-16)*2
    40F, 35 (approiximate)  ...Zone 3: T=[(40-30)/(35-28)]*Vm-10    =(10/7)*Vm-10
    Vm=(T+10)/(10/7)
    50F, 50 (accurate)      ...Zone 4: T=[(50-40)/(50-35)]*Vm-(50/3)=(2/3)*Vm+(50/3)
    Vm=(T-(50/3))/(2/3)
    80F,150 (accurate)      ...Zone 5: T=[(80-50)/(150-50)*Vm+45    =(3/10)*Vm+35
    Vm=(T-35)/(3/10)
    100F,300 (accurate)     ...Zone 6: T=[(100-80)/(300-150)]*Vm    =(2/15)*Vm+60
    Vm=(T-60)/(2/15)
    
      Kory: (interpolate the result)
      Use nearest temp in col 1 equal/above current temp
      Solve for Vm (max amount of moisture in the air)
      Using Releative Humidity (Rh), multiply to get V (amount of moisture in the air)
      Use same formula above to solve for T with the new V (Vm in equation)
*/

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
typedef int BOOL;
#define TRUE    1
#define FALSE   0

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double FindV(double T);
double FindT(double V);
double FindAirDensity(double Temp, double TempDew, double Humidity, double Altitude, double Barometer);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double  Temp        = 70.0;
double  Humidity    = 40.0;
double  Altitude    = 0;
double  Barometer   = 29.92;
BOOL    fTable      = FALSE;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

int __cdecl ParseCmdLine(int argc, char *argv[])
{
    int arg = 0;
    
    while (--argc)
    {
        arg++;
        switch(argv[arg][0])
        {
        case '-':
        case '/':
            switch(argv[arg][1])
            {
            case 't':
            case 'T':
                Temp    = atof(&argv[arg][2]);
                break;
            case 'h':
            case 'H':
                Humidity = atof(&argv[arg][2]);
                break;
            case 'a':
            case 'A':
                // make a multiple of 100
                Altitude = (double)(((int)(atof(&argv[arg][2]))/100)*100);
                if (Altitude > 4900.0) Altitude = 4900.0;
                if (Altitude < 0.0) Altitude = 0.0;
                break;
            case 'b':
            case 'B':
                Barometer = atof(&argv[arg][2]);
                break;
            case 'p':
            case 'P':
                fTable = TRUE;
                break;
            case '?':
                return FALSE;
                break;
            default:
                printf("Unknown param: %s\n", argv[arg]);
                return FALSE;
                break;
            }
            break;
            default:
                printf("Unknown param: %s\n", argv[arg]);
                return FALSE;
                break;
        }
    }
    
    return TRUE;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

void Usage(char * s)
{
    printf("Usage: %s <options>\n", s);
    printf("          -t<current temp F>\n");
    printf("          -h<relative humidity>\n");
    printf("          -b<barometer in-Hg)\n");
    printf("          -a<altitude> (in 100 feet increments 4900 max)\n");
    printf("          -p -> print dew point table\n");
    printf("\nExample: %s -t70 -h40 -b29.92 -a300\n", s);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

void main(int argc, char** argv)
{
    struct tm *newtime;
    time_t aclock;
    
    printf("\n===============================\n");
    printf("Air - written by Kory Gill\n");
    printf("===============================\n");
    
    time( &aclock );
    newtime = localtime( &aclock );
    printf( "Time: %s", asctime( newtime ) );

    printf("===============================\n");
    
    if (argc > 1)
    {
        if (FALSE == ParseCmdLine(argc, argv))
        {
            Usage(argv[0]);
            return;
        }
    }
    else
    {
        Usage(argv[0]);
        return;
    }
    
    double VMax;        // max moisture in air
    double V;           // moisture in air
    double TempDew;     // dew point temperature
    double AirDensity;  // calculated air density
    
    if (fTable)
    {
        double  Temp;
        double  Humidity;
        
        printf("Temp/Hum\n         ");
        for (Temp = 50; Temp <= 100; Temp += 10)
        {
            printf("%7.2f  ", Temp);
        }
        printf("\n");
        for (Humidity = 0; Humidity <= 100; Humidity += 10)
        {
            printf("%7.2f  ", Humidity);
            for (Temp = 50; Temp <= 100; Temp += 10)
            {
                VMax = FindV(Temp);
                V = VMax * Humidity / 100.0;
                TempDew = FindT(V);
                
                // print results
                printf("%7.2f  ", TempDew);
            }
            printf("\n");
        }
        printf("\n");
        printf("\n");
    }
    
    VMax = FindV(Temp);
    V = VMax * Humidity / 100.0;
    TempDew = FindT(V);
    AirDensity = FindAirDensity(Temp, TempDew, Humidity, Altitude, Barometer);
    
    // print results
    printf("       Temp: %7.2f F\n", Temp);
    printf("   Humidity: %7.2f %%\n", Humidity);
    //printf("       VMax: %7.2f\n", VMax);
    //printf("          V: %7.2f\n", V);
    printf("    TempDew: %7.2f F\n", TempDew);
    printf("   Altitude: %7.2f ft\n", Altitude);
    printf("  Barometer: %7.2f in-Hg\n", Barometer);
    printf(" AirDensity: %7.2f grams/liter\n", AirDensity);
    printf("\n");
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double FindV(double T)
{
    if (T > 80.0)
    {
        return (T-60.0)/(2.0/15.0);
    }
    else if (T > 50.0)
    {
        return (T-35.0)/(3.0/10.0);
    }
    else if (T > 40.0)
    {
        return (T-(50.0/3.0))/(2.0/3.0);
    }
    else if (T > 30.0)
    {
        return (T+10.0)/(10.0/7.0);
    }
    else if (T > 20.0)
    {
        return (T-16.0)*2.0;
    }
    else if (T > 0.0)
    {
        return T;
    }
    else
    {
        return T;
    }
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double FindT(double V)
{
    if (V > 150.0)
    {
        return (2.0/15.0)*V+60.0;
    }
    else if (V > 50.0)
    {
        return (3.0/10.0)*V+35.0;
    }
    else if (V > 35.0)
    {
        return (2.0/3.0)*V+(50.0/3.0);
    }
    else if (V > 28.0)
    {
        return (10.0/7.0)*V-10.0;
    }
    else if (V > 20.0)
    {
        return (V/2.0)+16.0;
    }
    else if (V > 0.0)
    {
        return V;
    }
    else
    {
        return V;
    }
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double FindAirDensity(double Temp, double TempDew, double Humidity, double Altitude, double Barometer)
{
    // water vapor pressure as function of dew point temp
    // index is degrees C (zero based)
    double WaterVapor[40] = {
        4.6,  4.9,  5.3,  5.7,  6.1,  6.5,  7.0,  7.5,  8.0,  8.6,
            9.2,  9.9, 10.5, 11.2, 12.0, 12.8, 13.6, 14.5, 15.5, 16.5,
            17.6, 18.7, 19.8, 21.1, 22.4, 23.8, 25.2, 26.8, 28.4, 30.1,
            31.9, 33.7, 35.7, 37.8, 40.0, 42.2, 44.6, 47.1, 49.8, 52.5
    };
    
    // standard atmospheric pressure as a function of altitude
    // index is elev in 100 foot increments (zero based)
    double StandardAirPressure[50] = {
        29.92, 29.81, 29.70, 29.60, 29.49, 29.38, 29.28, 29.17, 
            29.07, 28.96, 28.85, 28.75, 28.64, 28.54, 28.44, 28.33, 
            28.23, 28.13, 28.02, 27.92, 27.82, 27.72, 27.62, 27.52, 
            27.42, 27.31, 27.21, 27.11, 27.01, 26.91, 26.82, 26.72, 
            26.62, 26.52, 26.42, 26.32, 26.23, 26.13, 26.03, 25.94, 
            25.84, 25.74, 25.65, 25.56, 25.46, 25.37, 25.27, 25.18, 
            25.08, 24.99
    };
    
    // temp in Celcius
    double TempC  = (Temp  -32)*5.0/9.0;
    int TempDewC = (int)((TempDew-32)*5.0/9.0);
    
    // make sure it fits in the array
    if (TempDewC < 0) TempDewC = 0;
    else if (TempDewC > 39) TempDewC = 39;
    
    // barometer in as standard pressure in mm Hg
    int BarometerIndex = (int)Altitude/100;
    Barometer = StandardAirPressure[BarometerIndex] * (Barometer/29.92);
    
    // convert to mm Hg
    double BarometerM = Barometer * 25.4;
    
    // find density = 1.229 * (273/T) * ((P-W)/760) grams/liter
    // T is kelvin, 
    // P is absolute pressure in mm Hg, and 
    // W is correction factor for humidity (water vapor)
    double DryAirDensity = 1.2929 * (273.0/(TempC+273.0)) * ((BarometerM-WaterVapor[TempDewC])/760.0);
    
    return DryAirDensity;
}

