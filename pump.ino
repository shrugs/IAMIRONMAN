#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define SETPOINT (0.12)
#define STD_ERR (46/1024)

// LHL and RHL calculated as linear delta offset compared to setpoint
#define LHL (SETPOINT-STD_ERR)
#define RHL (SETPOINT+STD_ERR)

// Gain
#define GAIN 0.8

// delay after upset in ms
#define UPSET_DELAY 20000

// %wt of the salt in the salty container
#define PWT_SALT = 0.25


// lcd
LiquidCrystal lcd(2, 3, 4, 5, 6, 7, 10);

// pin that powers salinity sensor
int salinity_trigger = 11;
// analog pin that measures voltage drop over 10kOhm resistor
int salinity_pin = A0;

// soleniod salt
int ss = 9;
// solenoid di
int sdi = 8;


void setup()
{
    lcd.begin(20, 4);
    Serial.begin(9600);
}

void loop()
{
    static double salinity;
    static double target;
    // for the main loop
    // we want to check the salinity
    // if salinity is below LHL OR above RHL, correct it

    salinity = analogToSalinity(getSalinity());
    // Serial.println(salinity);

    if (salinity < LHL || salinity > RHL) {

        delay(UPSET_DELAY);
        salinity = analogToSalinity(getSalinity());


        // salinity is in %wt
        if (salinity < LHL) {
            // needs salt

            // calulate target using gain
            target = (salinity + (salinity-SETPOINT)*GAIN);

            openSaltForSeconds(getSaltSecondsForSalinityAndSetpoint(salinity, target));

        } else if (salinity > RHL) {
            // needs DI water

            // calc target
            target = (salinity - (salinity-SETPOINT)*GAIN);

            openDIForSeconds(getDISecondsForSalinityAndSetpoint(salinity, target));

        } else {
            // eh. whatver, it fixed itself I guess.
        }

    } else {

    }

}




double getSalinity()
{
    // averages salinity over course of 2 seconds.
    float x = 0;
    int samples = 10;

    for(int i=0; i<samples; i++){
        digitalWrite(salinity_trigger, HIGH);
        delay(100);
        x += analogRead(salinity_pin);
        digitalWrite(salinity_trigger, LOW);
        delay(100);
    }
    return (double)(x/samples);
}

void openDIForSeconds(float seconds)
{
    digitalWrite(sdi, HIGH);
    delay(seconds*1000);
    digitalWrite(sdi, LOW);
}

void openSaltForSeconds(float seconds)
{
    digitalWrite(ss, HIGH);
    delay(seconds*1000);
    digitalWrite(ss, LOW);
}

float salinityToAnalog(float s)
{
    return 22.9465601051 * log(s) + 197.0368452225;
}

float analogToSalinity(float a)
{
    return 0.7419333138 * log(a) - 3.6131359758;
}

float getSaltSecondsForSalinityAndSetpoint(double salinity, double target)
{
    // using equation developed in HW #7
}
float getDISecondsForSalinityAndSetpoint(double salinity, double target)
{
    // using equation developed in HW #7
}