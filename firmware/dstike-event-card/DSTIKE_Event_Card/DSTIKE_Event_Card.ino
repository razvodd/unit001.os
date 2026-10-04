/*
 UNIT001 WATCH / CALENDAR SYNC
 Short OK: TIME / NEXT / UNIT001. Hold OK 3 sec: phone setup.
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <U8g2lib.h>

constexpr uint8_t OLED_SDA=5,OLED_SCL=4,BUTTON_OK=14,RGB_PIN=15,RTC=0x68;
constexpr uint8_t MAX_EVENTS=10,TITLE_SIZE=13;
constexpr uint32_t HOLD_MS=3000,SETUP_MS=300000,SYNC_SECONDS=21600,MAGIC=0x554E4954;
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0,U8X8_PIN_NONE);
ESP8266WebServer web(80);
struct DateTime{uint16_t year;uint8_t month,day,hour,minute,second,weekday;};
struct Event{char title[TITLE_SIZE];uint16_t year;uint8_t month,day,hour,minute;};
struct Store{uint32_t magic;char ssid[33],password[65],url[241];uint32_t lastSync;uint8_t count;Event event[MAX_EVENTS];};
Store store; DateTime nowTime;
enum Page:uint8_t{TIME,NEXT,UNIT,PAGE_COUNT}; Page page=TIME;
bool setupMode=false,buttonWasDown=false; uint32_t setupAt=0,buttonAt=0,restartAt=0,lastRtc=0,lastSyncAttempt=0; uint8_t drawnSecond=255; Page drawnPage=PAGE_COUNT;

void wifiOff(){WiFi.disconnect(true);WiFi.mode(WIFI_OFF);WiFi.forceSleepBegin();delay(1);}
void wifiWake(){WiFi.forceSleepWake();delay(1);}
void saveStore(){EEPROM.put(0,store);EEPROM.commit();}
void loadStore(){EEPROM.begin(2048);EEPROM.get(0,store);if(store.magic!=MAGIC||store.count>MAX_EVENTS){memset(&store,0,sizeof(store));store.magic=MAGIC;saveStore();}}
uint8_t dec(uint8_t b){return((b>>4)*10)+(b&15);}
bool readRTC(DateTime&t){Wire.beginTransmission(RTC);Wire.write(0);if(Wire.endTransmission()!=0||Wire.requestFrom(RTC,uint8_t(7))!=7)return false;t.second=dec(Wire.read()&127);t.minute=dec(Wire.read()&127);t.hour=dec(Wire.read()&63);t.weekday=dec(Wire.read());t.day=dec(Wire.read());t.month=dec(Wire.read()&31);t.year=2000+dec(Wire.read());return t.second<60&&t.minute<60&&t.hour<24&&t.month>0&&t.month<13&&t.day>0&&t.day<32;}
bool leap(uint16_t y){return y%400==0||(y%4==0&&y%100!=0);}
uint8_t dim(uint16_t y,uint8_t m){static const uint8_t d[]={31,28,31,30,31,30,31,31,30,31,30,31};return m==2&&leap(y)?29:d[m-1];}
uint32_t seconds(uint16_t y,uint8_t m,uint8_t d,uint8_t h,uint8_t n,uint8_t s){uint32_t days=0;for(uint16_t i=2000;i<y;i++)days+=leap(i)?366:365;for(uint8_t i=1;i<m;i++)days+=dim(y,i);return(days+d-1)*86400UL+h*3600UL+n*60UL+s;}
uint32_t seconds(const DateTime&t){return seconds(t.year,t.month,t.day,t.hour,t.minute,t.second);}
uint32_t seconds(const Event&e){return seconds(e.year,e.month,e.day,e.hour,e.minute,0);}
const char*month(uint8_t n){static const char*m[]={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};return n>0&&n<13?m[n-1]:"---";}
const char*weekday(uint8_t n){static const char*d[]={"---","SUN","MON","TUE","WED","THU","FRI","SAT"};return n<8?d[n]:"---";}

bool connectWiFi(){if(!store.ssid[0])return false;wifiWake();WiFi.persistent(false);WiFi.mode(WIFI_STA);WiFi.begin(store.ssid,store.password);uint32_t started=millis();while(WiFi.status()!=WL_CONNECTED&&millis()-started<8000)delay(100);if(WiFi.status()!=WL_CONNECTED){wifiOff();return false;}return true;}
bool parseRows(const String&body){if(body.startsWith("ERROR|"))return false;Event next[MAX_EVENTS];memset(next,0,sizeof(next));uint8_t count=0;int from=0;while(from<body.length()&&count<MAX_EVENTS){int to=body.indexOf('\n',from);if(to<0)to=body.length();String row=body.substring(from,to);row.trim();if(row.length()>=18&&row.charAt(4)=='-'&&row.charAt(7)=='-'&&row.charAt(10)=='|'&&row.charAt(13)==':'&&row.charAt(16)=='|'){Event&e=next[count];e.year=row.substring(0,4).toInt();e.month=row.substring(5,7).toInt();e.day=row.substring(8,10).toInt();e.hour=row.substring(11,13).toInt();e.minute=row.substring(14,16).toInt();String title=row.substring(17);title.trim();title.toUpperCase();title.replace("|"," ");title.toCharArray(e.title,TITLE_SIZE);if(e.year>=2024&&e.month>0&&e.month<13&&e.day>0&&e.day<32&&e.hour<24&&e.minute<60&&e.title[0])count++;}from=to+1;}memcpy(store.event,next,sizeof(next));store.count=count;store.lastSync=seconds(nowTime);saveStore();return true;}
bool syncCalendar(){if(!store.url[0]||!connectWiFi())return false;BearSSL::WiFiClientSecure client;client.setInsecure();HTTPClient http;http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);bool ok=false;if(http.begin(client,store.url)){if(http.GET()==HTTP_CODE_OK)ok=parseRows(http.getString());http.end();}wifiOff();return ok;}
bool syncDue(){uint32_t latest=store.lastSync>lastSyncAttempt?store.lastSync:lastSyncAttempt;return latest==0||seconds(nowTime)-latest>=SYNC_SECONDS;}

void pageSetup(){const char*html=R"HTML(<!doctype html><meta name=viewport content="width=device-width,initial-scale=1"><style>body{margin:28px;max-width:520px;font:16px Arial;color:#111}h1{font-size:24px;font-weight:400}label{display:block;margin:20px 0 6px}input{box-sizing:border-box;width:100%;padding:12px;border:1px solid #111;font:16px monospace}button{margin-top:28px;padding:12px 18px;background:#111;color:white;border:0;font-size:16px}</style><h1>UNIT001 WATCH</h1><p>CALENDAR SYNC / SETUP</p><form action=/save><label>HOME WI-FI NAME</label><input name=ssid required maxlength=32><label>HOME WI-FI PASSWORD</label><input name=pass type=password maxlength=64><label>CALENDAR URL</label><input name=url type=url required maxlength=240 placeholder="https://script.google.com/..."><p>Google Calendar remains the editor. The watch saves the next 10 events and goes offline.</p><button>SAVE</button></form>)HTML";web.send(200,"text/html; charset=utf-8",html);}
void pageSave(){String ssid=web.arg("ssid"),pass=web.arg("pass"),url=web.arg("url");url.trim();if(!ssid.length()||!url.startsWith("https://")){web.send(400,"text/plain","Enter Wi-Fi and an HTTPS URL.");return;}ssid.toCharArray(store.ssid,sizeof(store.ssid));pass.toCharArray(store.password,sizeof(store.password));url.toCharArray(store.url,sizeof(store.url));store.lastSync=0;saveStore();web.send(200,"text/html","<meta name=viewport content='width=device-width,initial-scale=1'><p style='font:18px Arial;margin:28px'>SAVED. WATCH IS RESTARTING.</p>");restartAt=millis()+1000;}
void setupStart(){wifiWake();WiFi.persistent(false);WiFi.mode(WIFI_AP);WiFi.softAP("UNIT001 SETUP","unit001watch");web.on("/",HTTP_GET,pageSetup);web.on("/save",HTTP_GET,pageSave);web.begin();setupMode=true;setupAt=millis();}
void setupStop(){web.stop();setupMode=false;wifiOff();}

const Event*nextEvent(){const Event*result=nullptr;uint32_t now=seconds(nowTime);for(uint8_t i=0;i<store.count;i++)if(seconds(store.event[i])>=now&&(!result||seconds(store.event[i])<seconds(*result)))result=&store.event[i];return result;}
void drawTime(){char a[16],b[16];snprintf(a,sizeof(a),"%02u %s",nowTime.day,month(nowTime.month));snprintf(b,sizeof(b),"%02u:%02u:%02u",nowTime.hour,nowTime.minute,nowTime.second);display.drawStr(0,19,a);display.drawStr(0,41,b);display.drawStr(0,63,weekday(nowTime.weekday));}
void drawNext(){const Event*e=nextEvent();if(!e){display.drawStr(0,19,"NO EVENTS");display.drawStr(0,41,"CALENDAR");display.drawStr(0,63,"OFFLINE");return;}char a[20],b[16];uint32_t left=seconds(*e)-seconds(nowTime);snprintf(a,sizeof(a),"%02u %s %02u:%02u",e->day,month(e->month),e->hour,e->minute);snprintf(b,sizeof(b),"T-%02lu:%02lu:%02lu",left/3600UL,(left%3600UL)/60UL,left%60UL);display.drawStr(0,19,e->title);display.drawStr(0,41,a);display.drawStr(0,63,b);}
void drawUnit(){display.drawStr(0,19,"UNIT001");display.drawStr(0,41,"CALENDAR");display.drawStr(0,63,setupMode?"192.168.4.1":"HOLD OK");}
void render(){display.clearBuffer();display.setFont(u8g2_font_10x20_tf);if(page==TIME)drawTime();else if(page==NEXT)drawNext();else drawUnit();display.sendBuffer();}

void setup(){pinMode(BUTTON_OK,INPUT_PULLUP);pinMode(RGB_PIN,OUTPUT);digitalWrite(RGB_PIN,LOW);loadStore();Wire.begin(OLED_SDA,OLED_SCL);display.begin();display.setContrast(1);if(readRTC(nowTime))render();else{display.clearBuffer();display.setFont(u8g2_font_10x20_tf);display.drawStr(0,19,"RTC ERROR");display.sendBuffer();}if(store.url[0]&&syncDue()){lastSyncAttempt=seconds(nowTime);syncCalendar();}else wifiOff();}
void loop(){if(restartAt&&millis()>=restartAt)ESP.restart();if(setupMode){web.handleClient();if(millis()-setupAt>=SETUP_MS)setupStop();}bool down=digitalRead(BUTTON_OK)==LOW;if(down&&!buttonWasDown)buttonAt=millis();if(!down&&buttonWasDown){uint32_t held=millis()-buttonAt;if(held>=HOLD_MS&&!setupMode)setupStart();else if(held<HOLD_MS&&!setupMode)page=static_cast<Page>((page+1)%PAGE_COUNT);}buttonWasDown=down;if(millis()-lastRtc>=1000){lastRtc=millis();DateTime fresh;if(readRTC(fresh)){nowTime=fresh;if(!setupMode&&store.url[0]&&syncDue()){lastSyncAttempt=seconds(nowTime);syncCalendar();}}}if(nowTime.second!=drawnSecond||page!=drawnPage||setupMode){render();drawnSecond=nowTime.second;drawnPage=page;}delay(25);}
