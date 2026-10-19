#include <Arduino.h>

// MPXV7002 wired to GND, 5V, and A0 on 5V compatable board
// https://www.mouser.com/datasheet/2/302/MPXV7002-1127332.pdf?srsltid=AU7gw4Uh4M076TQ5EvhHnlSyQPgwFfRW4hxkDl53pVjkxX1cm6Puz-uy

// assumes airspeed is initially zero for calibration
// assumes fixed ambient air density

// user inputs
double Vsupply_V = 5.0;
double rho_slugft3 = 0.00194; // Tehachapi, CA
int nSamples = 500;

// constants and inits
double kPa2psf = 20.885434;
double resolution = 1023.0;
int value = 0;
double Vin = 0.0;
double V0 = 0.0;
double dP_kPa = 0.0;
double dP_psf = 0.0;
double v_fps = 0.0;
double dP_dV = 0.0;

void setup()
{
  Serial.begin(115200);

  // average out initial velocity
  for (int ii = 0; ii < nSamples; ii++)
  {
    value = analogRead(A0);
    V0 += static_cast<double>(value) * Vsupply_V / resolution;
  }
  V0 /= nSamples;

  // calculate velocity gradient
  dP_dV = 2.0 / (4.5 - V0);
}

void loop()
{
  value = analogRead(A0);                                    // read analog input on pin A0
  Vin = static_cast<double>(value) * Vsupply_V / resolution; // convert to voltage
  dP_kPa = dP_dV * (Vin - V0);                               // convert to differential pressure in kPa
  dP_psf = kPa2psf * dP_kPa;

  // negative airspeed protection
  if (dP_psf < 0.0)
  {
    v_fps = -sqrt(2.0 * -dP_psf / rho_slugft3);
  }
  else
  {
    v_fps = sqrt(2.0 * dP_psf / rho_slugft3);
  }

  // report
  Serial.print("vel[fps]= ");
  Serial.println(v_fps);
  delay(100);
}
