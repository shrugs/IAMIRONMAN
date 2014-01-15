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

bool saltyState = false;
bool diState = true;
double salinity = 0;

void setup()
{

    pinMode(salinity_trigger, OUTPUT);
    pinMode(salinity_pin, INPUT);
    pinMode(ss, OUTPUT);
    pinMode(sdi, OUTPUT);

    lcd.begin(20, 4);
    Serial.begin(9600);

    initLCD();
}

void loop()
{
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
    diState = true;
    digitalWrite(sdi, HIGH);
    delay(seconds*1000);
    digitalWrite(sdi, LOW);
    diState = false;
}

void openSaltForSeconds(float seconds)
{
    saltyState = true;
    digitalWrite(ss, HIGH);
    delay(seconds*1000);
    digitalWrite(ss, LOW);
    saltyState = false;
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

void initLCD()
{
    lcd.setCursor(1,0);
    lcd.print("LCL    SP     UCL");
    lcd.setCursor(1,1);
    lcd.print(LHL, 3);
    lcd.setCursor(7,1);
    lcd.print(SETPOINT, 3);
    lcd.setCursor(14,1);
    lcd.print(RHL, 3);

    // salty valve state
    lcd.setCursor(1,2);
    lcd.print("OFF");

    // current salinity
    lcd.setCursor(7,2);
    lcd.print("0.000");

    // DI state
    lcd.setCursor(15,2);
    lcd.print("OFF");

    lcd.setCursor(0,3);
    lcd.print("SALTY CONCENTRATION  DI");

}

void updateLCD()
{
    // salty valve state
    lcd.setCursor(1,2);
    lcd.print("   ");
    lcd.setCursor(1,2);
    lcd.print(saltyState?"ON":"OFF");

    // current salinity
    lcd.setCursor(7,2);
    lcd.print("     ");
    lcd.setCursor(7,2);
    lcd.print(salinity, 3);


    // DI state
    lcd.setCursor(15,2);
    lcd.print("   ");
    lcd.setCursor(15,2);
    lcd.print(diState?"ON":"OFF");
}