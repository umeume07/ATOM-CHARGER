#include <M5Unified.h>
#include <M5AtomicMotion.h>
#include <Wire.h>
#include <math.h>

// ATOM S3 + Atomic Motion Base V1.2
// Charger status is inferred from measured battery current.
// Full / no external power cannot be reliably distinguished without extra sensing.
M5AtomicMotion motion;
constexpr int SDA_PIN=38, SCL_PIN=39;
constexpr float DEAD_BAND_A=0.025f;
constexpr int CHARGING_CURRENT_SIGN=1; // Change to -1 if charge current is negative.
uint32_t lastUpdate=0;
bool connected=false;

int estimateSoc(float v) {
  // Li-ion approximate voltage table. Not accurate during charging / under load.
  const float voltage[]={3.00,3.30,3.50,3.60,3.70,3.75,3.80,3.85,3.90,4.00,4.08,4.14};
  const int percent[]={0,5,10,20,30,40,50,60,70,85,95,100};
  if(v<=voltage[0])return 0;
  for(int i=1;i<12;i++)
    if(v<=voltage[i])return percent[i-1]+roundf((percent[i]-percent[i-1])*(v-voltage[i-1])/(voltage[i]-voltage[i-1]));
  return 100;
}

void render(float v,float a,bool valid) {
  auto& d=M5.Display;
  d.fillScreen(TFT_BLACK);
  d.setTextDatum(textdatum_t::top_center);
  d.setTextColor(TFT_WHITE);
  d.drawString("ATOM CHARGER",64,5,2);
  if(!valid) {
    d.setTextColor(TFT_RED);
    d.drawString("SENSOR ERROR",64,35,2);
    d.setTextColor(TFT_WHITE);
    d.drawString("Check I2C/power",64,65,2);
    return;
  }
  float chargeA=a*CHARGING_CURRENT_SIGN;
  const char* state="IDLE / UNKNOWN";
  uint32_t color=TFT_YELLOW;
  if(chargeA>DEAD_BAND_A){state="CHARGING";color=TFT_GREEN;}
  else if(chargeA< -DEAD_BAND_A){state="DISCHARGING";color=TFT_ORANGE;}
  d.setTextColor(color);
  d.drawString(state,64,25,2);
  int soc=estimateSoc(v);
  d.drawRoundRect(15,48,94,36,5,TFT_WHITE);
  d.fillRect(110,59,5,14,TFT_WHITE);
  d.fillRoundRect(19,52,86*soc/100,28,3,soc<=20?TFT_RED:(soc<=50?TFT_YELLOW:TFT_GREEN));
  d.setTextDatum(textdatum_t::middle_center);
  d.setTextColor(TFT_WHITE,TFT_BLACK);
  d.drawString(String(soc)+"%*",62,66,4);
  d.setTextDatum(textdatum_t::top_center);
  d.setTextColor(TFT_WHITE);
  d.drawString(String(v,3)+" V",64,92,2);
  d.drawString(String(a*1000,1)+" mA",64,109,2);
}

void setup(){
  Serial.begin(115200);
  auto cfg=M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(0);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.drawString("Starting...",10,50,2);
  connected=motion.begin(&Wire,M5_ATOMIC_MOTION_I2C_ADDR,SDA_PIN,SCL_PIN,100000);
}
void loop(){
  M5.update();
  if(millis()-lastUpdate<1000)return;
  lastUpdate=millis();
  if(!connected)connected=motion.begin(&Wire,M5_ATOMIC_MOTION_I2C_ADDR,SDA_PIN,SCL_PIN,100000);
  if(!connected){render(NAN,NAN,false);return;}
  float v=motion.ina226.readBusVoltage();
  float a=motion.ina226.readShuntCurrent();
  bool valid=isfinite(v)&&isfinite(a)&&v>=2.0f&&v<=5.0f;
  Serial.printf("battery V=%.3f current A=%.4f estimated SOC=%d%%\n",v,a,estimateSoc(v));
  render(v,a,valid);
}
