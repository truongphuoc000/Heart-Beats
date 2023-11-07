#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define CommonSenseMetricSystem
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 // Reset pin 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//TX-RX
#include <SoftwareSerial.h>      
SoftwareSerial NANO(19,18);  
//
#define button 15
#define buzzer 4
#define trig_1 33 
#define echo_1 32
#define SOUND_SPEED 0.034
int duration_1; // do thoi gian
int distance_1; // luu do khoang cach
String NhipTim, SpO2;
//FIREBASE
#include <FirebaseESP32.h>
#define FIREBASE_HOST "https://esp32--datn-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "H6eNWrPHfQgwUMnFf1tYn8tj9hs8Or2CCaMbzwKt"
const char *WIFI_SSID = "Phòng 101"; // Tên wifi của bạn
const char *WIFI_PASSWORD = "12345678";// Password wifi của bạn
FirebaseData fbdo;
String data;
//
unsigned long  lasttime;
int mode = 1;
const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 
void setup()
{
  Serial.begin(115200);
  NANO.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
}
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  pinMode(button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(trig_1, OUTPUT);
  pinMode(echo_1, INPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
}
  

void khoangCach_1() 
{ //  hàm tính khoảng cách của cảm biến SRF04
  digitalWrite (trig_1, LOW); // cho chân phát xxung xuống mức thấp
  delayMicroseconds(2);//trễ 2us
  digitalWrite (trig_1, HIGH); // cho chân phát xxung lên mức cao
  delay(10);// trễ 10ms
  digitalWrite(trig_1, LOW); // cho chân phát xxung xuống mức thấp

  duration_1 = pulseIn (echo_1, HIGH, 30000); // tính thời gian từ lúc phát sóng âm đến khi nhận lại
  distance_1 = (duration_1 * 0.034 / 2); //chia thời gian cho vận tốc, ra quãng đường đơn vị là CM
} 
void loop()
{
  while(NANO.available()>0){ // phat hien co du lieu tu cong serial
  data = NANO.readString(); // doc du lieu
  Serial.println(data);  // in du lieu len may tinh
  int bd,kt;
  bd = data.indexOf("@");
  kt = data.indexOf("#");
  NhipTim = data.substring(bd+1,kt);
    
  bd = kt;
  kt = data.indexOf("$");
  SpO2= data.substring(bd+1,kt);
    
  int BPM = NhipTim.toInt();
  int Oxy = SpO2.toInt();
  khoangCach_1();
  Serial.println(distance_1);
  Firebase.setInt(fbdo,"/DATN/BPM" , BPM); 
  Firebase.setInt(fbdo,"/DATN/SpO2" , Oxy);
  Firebase.setInt(fbdo,"/DATN/DISTANCE" , (distance_1)); 

  
    display.display();
    display.clearDisplay();
    
    display.setCursor(2, 32);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println("BPM");
    display.setCursor(10, 50);
    display.println(BPM);
    display.setCursor(45, 32);
    display.println("SpO2");
    display.setCursor(60, 50);
    display.println(Oxy);
    display.setCursor(100, 32);
    display.println("KC");
    display.setCursor(100, 50);
    display.println(distance_1); 
      
    
    display.drawBitmap(50,0, bitmap, 28, 28, 1);
    display.display();
    
    if ( distance_1 > 45 && (millis() - lasttime) > 600000){
          mode = 2;
          display.clearDisplay();
        }
    if ( distance_1 < 50){
          lasttime = millis();
  //van toc song sieu am: v = 343.2m/s = 0.03432cm/us  / 2 la chu ki cua luc phat den luc nhan
}
  while (mode == 2){
  digitalWrite(buzzer,HIGH);
  delay(1000);
  digitalWrite(buzzer,LOW);
  delay(1000);
  display.display();
  display.clearDisplay();
  display.setCursor(20,30);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("WARNING!");
  if (digitalRead(button) == 0){
    mode = 1;
  }
    }
  } 
  // Animation
  
// 
}
