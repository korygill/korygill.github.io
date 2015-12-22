/*
  Author: Kory Gill (do-not-reply-kory@msn.com)
     WWW: http://korygill.net/korygillracing-retro/kory.htm/
    Date: 8/26/97

  This programs computes air density.
  The formulas were taken from: http://usatoday.com/weather/wworks0.htm.

  NOTE: Please leave my comments as they are to give credit 
        to those who contributed to it.
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
#include <string.h>
#include <math.h>

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double FindAirDensity(double Temp, double Humidity, double Altitude, double Barometer);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double  Temp        = 59.0;
double  Humidity    = 0.0;
double  Altitude    = 0;
double  Barometer   = 29.92;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

int ParseCmdLine(int argc, char *argv[])
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
    printf("\nExample: %s -t59 -h0 -b29.92 -a0\n", s);
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
    
	double AirDensity;
	const double StdAirDensity = 1.22556;

    AirDensity = FindAirDensity(Temp, Humidity, Altitude, Barometer);
    
    // print results
    printf("         Temp: %7.2f F\n", Temp);
    printf("     Humidity: %7.2f %%\n", Humidity);
    printf("     Altitude: %7.2f ft\n", Altitude);
    printf("    Barometer: %7.2f in-Hg\n", Barometer);
    printf("   AirDensity: %7.2f grams/liter\n", AirDensity);
    printf("%% Std Density: %7.2f grams/liter\n", AirDensity/StdAirDensity*100);
    printf("\n");
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------

double FindAirDensity(double Temp, double Humidity, double Altitude, double Barometer)
{
	double Altitude_m;
	double Temp_c;
	double Baro_mb;
	double SaturationMixRatio_g_per_kg;
	double MixRatio_g_per_kg;
	double TempVirtual_c;
	double Baro_altstation_mb;
    double AirDensity_kg_per_m3;

    Altitude_m	= Altitude * 12.0 * 2.54 / 100.0;
	Temp_c	= (Temp-32.0) * 5.0 / 9.0;
	Baro_mb	= Barometer / (29.92 / 1013.0);
	SaturationMixRatio_g_per_kg = 3.884266 * pow(10,(7.5*Temp_c)/(237.7+Temp_c));
	MixRatio_g_per_kg = SaturationMixRatio_g_per_kg * Humidity / 100.0;
	TempVirtual_c = ((1.0 + 1.609 * MixRatio_g_per_kg) / (1.0 + MixRatio_g_per_kg)) * Temp_c;
	Baro_altstation_mb = Baro_mb * exp(-Altitude_m / 7000.0);
	AirDensity_kg_per_m3 = (Baro_altstation_mb * 100.0) / ((TempVirtual_c + 273.0) * 287.0);

    return AirDensity_kg_per_m3;
}

