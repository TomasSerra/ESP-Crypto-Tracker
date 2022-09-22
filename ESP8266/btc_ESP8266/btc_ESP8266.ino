#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char* ssid     = "username";
const char* password = "password";

int analogInPin  = A0;    
int sensorValue;   
float calibration = 0.36; 
int bat_percentage;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.in.pool.ntp.org", 5.5*3600, 60000);

#define CG_URL "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin%2Cethereum%2Cdogecoin%2Cshiba-inu%2Cpolkadot&vs_currencies=inr%2Cusd&include_last_updated_at=true&include_24hr_change=true"
const char *fingerprint  = "33 C5 7B 69 E6 3B 76 5C 39 3D F1 19 3B 17 68 B8 1B 0A 1F D9";

String payload = "{}";

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(10);

  u8g2.begin();
  u8g2.enableUTF8Print();

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tf);

  u8g2.setCursor(1, 15);
  u8g2.print("Connecting to WiFi ");
  u8g2.setCursor(1, 25);
  u8g2.sendBuffer();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    u8g2.print(".");
    u8g2.sendBuffer();
  }
  timeClient.begin();

 Serial.println("");
 Serial.println("WiFi connected");
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 u8g2.setCursor(1, 45);
    u8g2.print("Connected to ");
    u8g2.print(ssid);
    u8g2.setCursor(1, 55);
    u8g2.print("IP Address ");
    u8g2.print(WiFi.localIP());
    u8g2.sendBuffer();
    delay(5000);
}

void loop()
{ 
  if (WiFi.status() == WL_CONNECTED)
  {  
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    HTTPClient http; 
    delay(200);

    while (!timeClient.update()) {
      timeClient.forceUpdate();
    }
    Serial.println();
    Serial.print("Time     - ");
    Serial.println(timeClient.getFormattedTime());
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB18_tf);
    u8g2.setCursor(23, 40);
    u8g2.print(timeClient.getFormattedTime());
    u8g2.drawFrame(110,1,13,6);
    u8g2.drawFrame(123,3,1,2);
    u8g2.drawBox(111,2,bat_percentage,4);
    u8g2.sendBuffer();
    delay (1000);
    
    sensorValue = analogRead(analogInPin);
    float voltage = (((sensorValue * 3.3) / 1024) * 2 + calibration); //multiply by two as voltage divider network is 100K & 100K Resistor

    bat_percentage = mapfloat(voltage, 2.8, 4.2, 0, 11); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
 
  if (bat_percentage >= 11)
  {
    bat_percentage = 11;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);
  delay(1000);
  delay (20);
  
    payload = "{}";

    http.begin(client, CG_URL);
    Serial.println();
    Serial.print("Coingecko URL - ");
    Serial.println(CG_URL);

    int httpCode = http.GET();
    
    if (httpCode > 0) { 
      payload = http.getString();
      DynamicJsonDocument doc(800);
      DeserializationError error = deserializeJson(doc, payload);
      if (error) 
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        delay(5000);
        return;
      }
     JsonObject bitcoin = doc["bitcoin"];
      double bitcoin_inr = bitcoin["inr"]; 
      double bitcoin_usd_24h_change = bitcoin["usd_24h_change"]; 
      double bitcoin_usd = bitcoin["usd"]; 
     
      JsonObject ethereum = doc["ethereum"];
      double ethereum_inr = ethereum["inr"];
      double ethereum_usd_24h_change = ethereum["usd_24h_change"]; 
      double ethereum_usd = ethereum["usd"]; 
      
      JsonObject dogecoin = doc["dogecoin"];
      double dogecoin_inr = dogecoin["inr"];
      double dogecoin_usd = dogecoin["usd"];
      double dogecoin_usd_24h_change = dogecoin["usd_24h_change"]; 
      long dogecoin_last_updated_at = dogecoin["last_updated_at"]; 

      JsonObject shibainu = doc["shiba-inu"];
      double shibainu_inr = shibainu["inr"]; 
      double shibainu_usd_24h_change = shibainu["usd_24h_change"]; 
      double shibainu_usd = shibainu["usd"]; 

      JsonObject polkadot = doc["polkadot"];
      double polkadot_inr = polkadot["inr"];
      double polkadot_usd_24h_change = polkadot["usd_24h_change"]; 
      double polkadot_usd = polkadot["usd"]; 

      // Edit below your coin holdings here
     double holdings_ethereum  =  0.555;
      double holdings_bitcoin  =  0;
      double holdings_dogecoin =  0.5443;
      double holdings_polkadot    =  0.0324;
      double holdings_shibainu   =  4800000;

      Serial.print("Bitcoin  - $");
      Serial.print("   $");
      Serial.print(bitcoin_usd);
      Serial.println();
      Serial.print("Ethereum  - $");
      Serial.print("   $");
      Serial.print(ethereum_usd);
      Serial.println();
      Serial.print("Dogecoin  - $");
      Serial.print("   $");
      Serial.print(dogecoin_usd);
      Serial.println();
      Serial.print("Polkadot  - $");
      Serial.print("   $");
      Serial.print(polkadot_usd);
      Serial.println();
      Serial.print("shibainu  - $");
      Serial.print("   $");
      Serial.print(shibainu_usd);
      Serial.println();

      double holdings =
        (ethereum_usd   * holdings_ethereum) +
        (bitcoin_usd   * holdings_bitcoin) +
        (dogecoin_usd  * holdings_dogecoin) +
        (polkadot_usd * holdings_polkadot) +
        (shibainu_usd    * holdings_shibainu);

      Serial.println(holdings);
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
  
      draw("BITCOIN - BTC",    bitcoin_usd,   2,  bitcoin_usd_24h_change,   "$");
      draw("ETHEREUM - ETH",   ethereum_usd,  2,  ethereum_usd_24h_change,  "$");
      draw("SHIBA-INU - SHIB", shibainu_usd, 6,  shibainu_usd_24h_change, "$");
      draw("DOGECOIN - DOGE", dogecoin_usd, 4,  dogecoin_usd_24h_change, "$");
      draw("POLKADOT - DOT",     polkadot_usd,  2,  polkadot_usd_24h_change,    "$");
      draw("HOLDINGS  ",       holdings,      0,  0,                        "$");
    }
    http.end();  
  }
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];  // should for screen with up to 256 pixel width
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset) / 8;
    dx = offset + char_offset * 8;
    if ( char_offset >= u8g2.getDisplayWidth() / 8 )
      return;
    visible = u8g2.getDisplayWidth() / 8 - char_offset + 1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset * 8 - dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return; // nothing visible
    dx = offset - char_offset * 8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth() / 8 + 1 )
      visible = u8g2.getDisplayWidth() / 8 + 1;
    strncpy(buf, s + char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
}
void draw(char *s, double coinprice, int prec, double change, String currency)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);

  char pricelen[10];
  dtostrf(coinprice, 4, prec, pricelen);
  int xPos = 64 - (((strlen(pricelen)) * 11) / 2);

  for (;;)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB18_tf);
    u8g2.setCursor(xPos - 11, 30);
    u8g2.print(currency);
    u8g2.print(coinprice, prec);

    if (change != 0) {
      u8g2.setFont(u8g2_font_helvB10_tf);
      u8g2.setCursor(38, 47);
      if (change > 0) {
        u8g2.print("+");
      }
      u8g2.print(change, 2);
      u8g2.print("%");
    }

    drawScrollString(offset, s);
     u8g2.drawFrame(110,1,13,6);
    u8g2.drawFrame(123,3,1,2);
    u8g2.drawBox(111,2,bat_percentage,4);
    u8g2.sendBuffer();
    delay(10);
    offset += 2;
    if ( offset > len * 8 + 1 )
      break;
  }
}
