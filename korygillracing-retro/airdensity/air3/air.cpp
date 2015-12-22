/*

  Author: Kory Gill (do-not-reply-kory@msn.com)

     WWW: http://korygill.net/korygillracing-retro/kory.htm/

    Date: 8/14/98



  This programs computes air density.



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



double FindAirDensity(double Temp, double Humidity, double Barometer);



//---------------------------------------------------------------------------

//

//---------------------------------------------------------------------------



double  Temp        = 59.0;

double  Humidity    = 0.0;

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

    printf("\nExample: %s -t59 -h0 -b29.92\n", s);

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

	double StdAirDensity;

	//1.22556

    StdAirDensity = FindAirDensity(59.0, 0.0, 29.92);



    AirDensity = FindAirDensity(Temp, Humidity, Barometer);

    

    // print results

    printf("         Temp: %7.2f F\n", Temp);

    printf("     Humidity: %7.2f %%\n", Humidity);

    printf("    Barometer: %7.2f in-Hg\n", Barometer);

    printf("   AirDensity: %7.2f kg/m3\n", AirDensity);

    printf("%% Std Density: %7.2f kg/m3\n", AirDensity/StdAirDensity*100);

    printf("\n");

}



//---------------------------------------------------------------------------

//

//---------------------------------------------------------------------------



double FindAirDensity(double Temp, double Humidity, double Barometer)

{

	double Temp_c;

	double Temp_k;

	double Baro_mb;

	double Baro_pa;

	double SaturationPressure_mb;

	double VaporPressure_mb;

	double TempVirtual_k;

	double AirDensity_kg_per_m3;

	double GasConstant = 287.0;



	Temp_c	= (Temp-32.0) * 5.0 / 9.0;

	Temp_k	= Temp_c + 273.0;

	

	Baro_mb = Barometer / (29.92 / 1013.0);

	Baro_pa = Baro_mb * 100.0;



	SaturationPressure_mb = 6.11 * pow(10,(7.5*Temp_c)/(237.7+Temp_c));

	VaporPressure_mb = Humidity * SaturationPressure_mb / 100.0;

	TempVirtual_k = Temp_k / (1.0 - (VaporPressure_mb/Baro_mb)*(1.-0.622));



	// D = P/(T*R)

	AirDensity_kg_per_m3 = Baro_pa / (TempVirtual_k*GasConstant);

	

    return AirDensity_kg_per_m3;

}



