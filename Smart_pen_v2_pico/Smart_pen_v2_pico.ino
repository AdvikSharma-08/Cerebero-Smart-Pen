#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <driver/i2s.h>
#include "driver/dac.h"
#include <HardwareSerial.h>


// define
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels 
#define OLED_RESET    -1 
#define TOUCH_PIN_1 T0 // GPIO4
#define TOUCH_PIN_2 T2 // GPIO2
#define MPU6050_I2C_ADDRESS 0x68

#define I2S_WS 13
#define I2S_SD 15
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0

#define bufferLen 24000


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) SDA = 21, SCL = 22
WiFiClient client;
HardwareSerial SerialY(1); 
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

//  variables 
//const char* ssid = "FiberS_2.4G";
//const char* password = "44444444";
//const char* ssid = ".";
//const char* password = "92345679";

const char* ssid = ".";
const char* password = "12345678";

//const char* ssid = "PPC185D2-2.4G";
//const char* password = "12345678";

//const char* ssid = "VivoS";
//const char* password = "12345678";

const char* server_addr = "192.168.95.209";
//const char* server_addr = "192.168.1.8";
//const char* server_addr = "192.168.42.157";
//const char* server_addr = "192.168.64.209";
//const char* server_addr = "192.168.43.209";


const int AUDIO_PIN = DAC_CHANNEL_1;

int server_port = 7700;
int press_status_1 = 0;
int press_count_1 = 0;
int press_millis_1 = 0;
int press_status_2 = 0;
int press_count_2 = 0;
int press_millis_2 = 0;
int mic_value=0;
int x_cursor=0;
int y_cursor=0;
int menus=0; //0->main menu,
int menu_option_num=1;
int current_millis=0;
String combine_data="";
int16_t sBuffer[bufferLen];
String dataTotal="";
String pen_status="";
bool in_mode= false;
bool button_2_alr=false;



// functions
void i2s_install() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 8000,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false
  };
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}
void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };
  i2s_set_pin(I2S_PORT, &pin_config);
}
String client_get_text(){
  if (client.connect(server_addr, server_port)) {
    client.println("GET /send_audio HTTP/1.1");
    client.println("Host: your_ip_address");
    client.println("Connection: close");
    client.println();
    String header_response = "";
    String body_response="";
    bool headersEnded = false;
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        header_response += c;
        if (header_response.endsWith("\r\n\r\n") && !headersEnded) {
          headersEnded = true;
        }
        if (headersEnded) {
          if(c!='\n' && c!='\0'){
            body_response += c;
          }
        }
      }
    }
    client.stop();
    return body_response;
  } else {
    Serial.println("Connection to server failed");
  }
}
String client_get_data(){
  if (client.connect(server_addr, server_port)) {
    client.println("GET /send_data_client HTTP/1.1");
    client.println("Host: your_ip_address");
    client.println("Connection: close");
    client.println();
    Serial.println("hihi");
    String header_response = "";
    String body_response="";
    bool headersEnded = false;
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        header_response += c;
        if (header_response.endsWith("\r\n\r\n") && !headersEnded) {
          headersEnded = true;
        }
        if (headersEnded) {
          if(c!='\n' && c!='\0'){
            body_response += c;
          }
        }
      }
    }
    client.stop();
    return body_response;
  } else {
    Serial.println("Connection to server failed");
  }
}
void server_to_buffer_audio(){
  String temp="";
  int j_index=-1;
  for(int i=0;i<dataTotal.length();i++){
    if(dataTotal[i]==','){
      j_index=j_index+1;
      sBuffer[j_index]=(int16_t)temp.toInt();
      temp="";
    }
    else{
      temp=temp+dataTotal[i];
    }
  }
}

void playAudio() {
  int total_index=sBuffer[0];
  for(int i=1;i<=total_index;i++){
    uint8_t dac_value = (uint8_t)(sBuffer[i] + 128);
    dac_output_voltage(DAC_CHANNEL_1,dac_value);
    delayMicroseconds(100);
  }
}

void ready_to_play_audio(){
   while (1) {
    dataTotal=client_get_text();
    if(dataTotal!="0"){
      Serial.println("---"+dataTotal+"---");
      break;
    }
  }
  server_to_buffer_audio();
}

void storeAudio(){
  size_t bytes_read;
  Serial.println("Speak!!!");
  delay(500);
  oled_println("Speak!!!");
  i2s_read(I2S_PORT, &sBuffer, bufferLen * 2, &bytes_read, portMAX_DELAY);
  oled_println("Stop!!!");
  Serial.println("Data read from I2S and stored in sBuffer");
}
void sendAudio(){
  if (!client.connect(server_addr, server_port)) {
    Serial.println("Connection failed");
    while (1); // Stop if connection fails
  }
  for(int i=0;i<bufferLen/3000;i++){
    dataTotal="";
    for(int j=0;j<3000;j++){
      dataTotal=dataTotal+String(",")+String(sBuffer[i*3000+j]);
      String(dataTotal);
    }
    if (client.connect(server_addr, server_port)) {
      Serial.println("Connected to server.");
      client.print("POST /upload_audio HTTP/1.1\r\n");
      client.print("Host: ");
      client.print(server_addr);
      client.print("\r\n");
      client.print("Content-Length: ");
      client.print(dataTotal.length());
      client.print("\r\n\r\n");
      client.print(dataTotal);
      client.stop();
      Serial.println("Text data sent to server.");
    } else {
      Serial.println("Connection to server failed.");
    }
  }  
}
void sendPenStatus(){
  if (client.connect(server_addr, server_port)) {
    Serial.println("Connected to server.");
    client.print("POST /upload_pen_status HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(server_addr);
    client.print("\r\n");
    client.print("Content-Length: ");
    client.print(pen_status.length());
    client.print("\r\n\r\n");
    client.print(pen_status);
    client.stop();
    Serial.println("Text data sent to server.");
  }
  else {
    Serial.println("Connection to server failed.");
  }  
}

void display_initially() {
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);    // (x,y) x-> left, y->down
  display.invertDisplay(true);
  display.println(F("SMART PEN"));
  display.display();
  delay(1500);
  display.clearDisplay();
  display.setCursor(20, 10);    // (x,y) x-> left, y->down
  display.println(F("CEREBRO"));
  display.display();
  delay(1500);
  display.clearDisplay();
  display.display();
  display.invertDisplay(false);
}
void oled_print(String abc){
    display.clearDisplay();        // Clear the display
    display.setCursor(0, 0);       // Set cursor to top-left
    display.setTextSize(1);        // Set text size to 1:1
    display.setTextColor(SSD1306_WHITE); // Set text color to white
    display.println("-> "+abc);            // Print the value of 'a'
    display.display();   
}                                     
void oled_println(String abc){  
    if(y_cursor>=32){
      y_cursor=0;
      display.clearDisplay(); 
    }  
    display.setCursor(0,y_cursor);
    display.setTextSize(1);        // Set text size to 1:1
    display.setTextColor(SSD1306_WHITE); // Set text color to white
    display.println(""+abc);            // Print the value of 'a'
    display.display();  
    y_cursor=y_cursor+8;
}
void led_color(int R,int G,int B,int V){
  analogWrite(26,V);
  analogWrite(0,R); // 37 -> Red
  analogWrite(12,G); // 38 -> Green
  analogWrite(27,B); // 34 -> Blue
  //delay(300);
}
void mode_blue_print(){
  y_cursor=1;
  const char* menu_option[] = {
    "1) Text Recognize",
    "2) Calculation" ,
    "3) Draw",
    "4) Stylus",
    "5) Text Translate En-Hi",
    "6) Text Translate Hi-En",
    "7) Chat Bot",
    "8) Aud Translate En-Hi",
    "9) Aud Translate Hi-En",
    "10) Setting",
    "11) Info"
};
  String temp_no=" ";
  String temp_val=menu_option[menu_option_num-1];
  for(int i=3;i<temp_val.length();i++){
    temp_no=temp_no+menu_option[menu_option_num-1][i];
  }
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, 10, SSD1306_WHITE); 
  display.setTextColor(SSD1306_BLACK); 
  display.setCursor(0,y_cursor);
  display.setTextSize(1);        // Set text size to 1:1
  display.println(temp_no); 
  display.display();
  Serial.println(temp_no);
  y_cursor=y_cursor+8+1;
}



void main_menu(int a){
  const char* menu_option[] = {
    "1) Text Recognize",
    "2) Calculation" ,
    "3) Draw",
    "4) Stylus",
    "5) Text Translate En-Hi",
    "6) Text Translate Hi-En",
    "7) Chat Bot",
    "8) Aud Translate En-Hi",
    "9) Aud Translate Hi-En",
    "10) Setting",
    "11) Info"
};
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 0);  
  display.println(F("SELECT OPTION --->"));
  display.display();

  int b=a-1;
  int r=b%2;
  int b2= b;
  if(r==0){
    b2=b+1;
  }
  else{
    b2=b2-1;
  }

  for(int i=0;i<2;i++){
    if(i==r){
      display.fillRect(0, (8+2)*(i+1)+2, SCREEN_WIDTH, 8+2, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 
      display.setCursor(10, (8+2)*(i+1)+3); 
      display.print(menu_option[b]); 
    }
    else{
      display.setTextColor(SSD1306_WHITE); 
      display.setCursor(10, (8+2)*(i+1)+3); 
      display.print(menu_option[b2]); 
    }
    display.display();
  }
}

void long_button_1(){

}
void long_button_2(){
  
}
void X1_button_1(){
  if(menus==0){
    button_2_alr=false;
    if(in_mode==true){
      if(menu_option_num==3){
        SerialY.println("31");
        oled_println("31");
        delay(500);
      }
      else if(menu_option_num==4){
        SerialY.println("41");
        oled_println("41");
        delay(500);
      }
      in_mode=false;
    }
    menu_option_num=menu_option_num+1;
    if(menu_option_num<=11){
      main_menu(menu_option_num);
    }
    else if(menu_option_num>11){
      menu_option_num=1;
      main_menu(1);
    }
  }
}
void print_mean_cal(){
String display_data_str="-0";
      while(display_data_str=="-0"){
        display_data_str=client_get_data();
      }
      Serial.println(display_data_str);
      display.clearDisplay();
      y_cursor=0;
      String head_te="";
      String body_te="";
      bool tell_te= false;
      for(int i=0;i<display_data_str.length();i++){
        if(i==display_data_str.length()-1){
          if(tell_te){
            body_te=body_te+display_data_str[i];
          }
          else{
            head_te=head_te+display_data_str[i];
          }
        }
        else if(display_data_str[i]=='_' && display_data_str[i+1]=='_' ){
            i=i+1;
            tell_te=true;
        }
        else{
          if(tell_te){
            body_te=body_te+display_data_str[i];
          }
          else{
            head_te=head_te+display_data_str[i];
          }
      }
      }
      oled_println("----"+head_te+"----");
      oled_println(body_te);
}
void X1_button_2(){
  if(button_2_alr==false  ){
    if(menu_option_num==1 || menu_option_num==2 || menu_option_num==5 || menu_option_num==6){
      button_2_alr=false;
    }
    else{
      button_2_alr=true;
    }
    
    mode_blue_print();
    in_mode=true;
    if(menu_option_num==1){
      SerialY.println(1);
      pen_status="img_send_text_recognize";
      sendPenStatus();
      //delay(1000);
      String received_a="";
      while(1){
        if (SerialY.available()) {
          received_a = SerialY.readStringUntil('\n');
          if(string_n(received_a)[0]=='0' || string_n(received_a)[0]=='1'){
            oled_println(string_n(received_a));
            break;
          }
        }
      }
      print_mean_cal();
    }
    else if(menu_option_num==2){
      SerialY.println(2);
      pen_status="img_send_text_calculate";
      sendPenStatus();
      //delay(1000);
      String received_a="";
      while(1){
        if (SerialY.available()) {
          received_a = SerialY.readStringUntil('\n');
          if(string_n(received_a)[0]=='0' || string_n(received_a)[0]=='1'){
            oled_println(string_n(received_a));
            break;
          }
        }
      } 
      print_mean_cal();
    }
    else if(menu_option_num==3){
      pen_status="draw";
      sendPenStatus();
      SerialY.println(3);
    }
    else if(menu_option_num==4){
      SerialY.println(4);
      pen_status="stylus";
      sendPenStatus();

    }
    else if(menu_option_num==5){
      SerialY.println(5);
      pen_status="img_send_text_translate_en_hi";
      sendPenStatus();
      //delay(1000);
      String received_a="";
      while(1){
        if (SerialY.available()) {
          received_a = SerialY.readStringUntil('\n');
          if(string_n(received_a)[0]=='0' || string_n(received_a)[0]=='1'){
            oled_println(string_n(received_a));
            break;
          }

        }
      } 
    }
    else if(menu_option_num==6){
      SerialY.println(6);
      pen_status="img_send_text_translate_hi_en";
      sendPenStatus();
      //delay(1000);
      String received_a="";
      while(1){
        if (SerialY.available()) {
          received_a = SerialY.readStringUntil('\n');
          if(string_n(received_a)[0]=='0' || string_n(received_a)[0]=='1'){
            oled_println(string_n(received_a));
            break;
          }
        }
      } 
      print_mean_cal();
    }
    else if(menu_option_num==7){
      storeAudio();
      sendAudio();
      pen_status="audio_sent";
      sendPenStatus();
      print_mean_cal();
      /*
      ready_to_play_audio();
      for(int i=0;i<3;i++){
        playAudio();
      }
      */
    }
    else if(menu_option_num==9){
      storeAudio();
      sendAudio();
      pen_status="audio_hi_sent";
      sendPenStatus();
      print_mean_cal();
      /*
      ready_to_play_audio();
      for(int i=0;i<3;i++){
        playAudio();
      }
      */
    }
  }
  else{
      pen_status="right_stylus_button_clicked";
      sendPenStatus();
  }

}

void X2_button_1(){
  
}
void X2_button_2(){
  
}

String string_n(String abc){
  String r="";
  for(int i=0;i<abc.length()-1;i++){
    r=r+abc[i];
  }
  return r;
}
void initial_data_seeed(){
  SerialY.println(String(ssid));
  SerialY.println(String(password));
  SerialY.println(String(server_addr));
  SerialY.println(String(server_port));
}
// void setup
void setup() {
  pinMode(0,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT); // heptic

  Serial.begin(9600);
  SerialY.begin(9600, SERIAL_8N1, 9,10);// rx,tx  - 9 , 10
  //SerialY.begin(9600, SERIAL_8N1, 3,1);// rx,tx  - 3 , 1


  led_color(0,0,0,0);
  delay(1000);
  led_color(5,0,0,0);
  delay(500);
  led_color(0,5,10,0);
  delay(500);
  led_color(0,10,5,0);
  delay(500);
  led_color(0,15,5,0);
  delay(500);
  led_color(0,5,15,0);
  delay(500);
  led_color(0,5,0,0);
  delay(500);
  led_color(0,0,5,0);
  delay(500);
  led_color(0,0,0,200);
  delay(500);
  led_color(0,0,0,0);

  //initial_data_seeed();

  // Initialize the display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    oled_println("SSD1306 allocation failed"); 
  }
  display_initially();

  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    oled_println("Connecting to WiFi...");
    Serial.println("Connecting to wifi");
    
  }
  oled_println("1-> Connected to WiFi");
  Serial.println("Connected to wifi");
  //SerialY.println(1230);

  //delay(3000);
  
  String received1="";
  //while(1){
    if (SerialY.available()) {
      received1 = SerialY.readStringUntil('\n');
      Serial.println(received1);
      if(string_n(received1)==String(1231)){
        Serial.println("2-> Connected to WiFi");
        oled_println("2-> Connected to WiFi");
        //break;
      }

    }
  //}


  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  dac_output_enable(DAC_CHANNEL_1);


  // Initialize the MPU6050
  if (!mpu.begin(MPU6050_I2C_ADDRESS)) {
    oled_println("Failed to find MPU6050 chip"); 
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

 
  y_cursor=0;
  display.clearDisplay(); 
  delay(1000);

  main_menu(1);
  
  current_millis=millis();

}

void gyro_work(){
  mpu.getEvent(&a, &g, &temp);

  //String rotation="";
  //String acceleration="";
 
  //rotation=String(g.gyro.x)+","+String(g.gyro.y)+","+String(g.gyro.z);
  //acceleration=String(a.acceleration.x)+","+String(a.acceleration.y)+","+String(a.acceleration.z);
  //combine_data=rotation+"+"+acceleration;
  combine_data=String(g.gyro.x)+","+String(g.gyro.y)+","+String(g.gyro.z)+"+"+String(a.acceleration.x)+","+String(a.acceleration.y)+","+String(a.acceleration.z);

  //Serial.print("Gyro X: ");
  //Serial.println(g.gyro.x);
  //Serial.print("Gyro Y: "); 
  //Serial.println(g.gyro.y);
  //Serial.print("Gyro Z: "); 
  //Serial.println(g.gyro.z);

  //Serial.print("Accel X: "); 
  //Serial.println(a.acceleration.x);
  //Serial.print("Accel Y: "); 
  //Serial.println(a.acceleration.y);
  //Serial.print("Accel Z: "); 
  //Serial.println(a.acceleration.z);

    if (!client.connected()) {
    //Serial.println("Connecting to server...");
    if (client.connect(server_addr, server_port)) {
      //Serial.println("Connected to server.");
      // Send text data to server
      client.print("POST /upload_gyro HTTP/1.1\r\n");
      client.print("Host: ");
      client.print(server_addr);
      client.print("\r\n");
      client.print("Content-Length: ");
      client.print(String(combine_data).length());
      client.print("\r\n\r\n");
      client.print(combine_data);
      client.stop(); // Close connection after sending data
      //Serial.println("Text data sent to server.");
    } else {
      //Serial.println("Connection to server failed.");
    }
  }

}


// void loop
void loop() {

  int touchState_1 = touchRead(TOUCH_PIN_1);
  int touchState_2 = touchRead(TOUCH_PIN_2);

/*
  for(;;){
    if(millis()-current_millis>=1000){
      break;
    }
    gyro_work();
  }
*/

  if(touchState_1<40){
    led_color(0,0,5,200);
    delay(200);
    led_color(0,0,0,0);
    X1_button_1();
  }

  if(touchState_2<40){
    led_color(0,5,0,200);
    delay(200);
    led_color(0,0,0,0);
    X1_button_2();
  }
  
  /*
  if (press_millis_1 == 0) {
    if (touchState_1 < 50 ) {
      press_status_1 = 1;
      press_count_1 = 1;
      press_millis_1 = millis();
    }
  } 
  else if (millis() - press_millis_1 < 1000) {
    if (touchState_1 < 50 && press_status_1 == 0) {
      press_count_1 = press_count_1 + 1;
    } else if (touchState_1 > 50) {
      press_status_1 = 0;
    }
  } 
  else if (millis() - press_millis_1 >= 1000) {
    if (touchState_1 < 50 && press_count_1 == 1) {
      led_color(191,0,255,200);
      delay(20);
      led_color(0,0,0,0);
      //oled_println("Long Button 1 Pressed");
      delay(500);
    } else if (touchState_1 < 50 && press_count_1 >= 2) {
      led_color(191,0,255,200);
      delay(20);
      led_color(0,0,0,0);
      //oled_println("Button 1 Clicked 2x ");
    } else if (touchState_1 > 50 && press_count_1 == 1) {
      led_color(191,0,255,200);
      delay(20);
      led_color(0,0,0,0);
      X1_button_1();
      //oled_println("Button 1 Clicked");
    } else if (touchState_1 > 50 && press_count_1 >= 2) {
      led_color(191,0,255,200);
      delay(20);
      led_color(0,0,0,0);
      //oled_println("Button 1 Clicked 2x ");
    }
    press_status_1 = 0;
    press_count_1 = 0;
    press_millis_1 = 0;
  }
  */
/*
 if (press_millis_2 == 0) {
    if (touchState_2 < 50 ) {
      press_status_2 = 1;
      press_count_2 = 1;
      press_millis_2 = millis();
    }
  } 
  else if (millis() - press_millis_2 < 1000) {
    if (touchState_2 < 50 && press_status_2 == 0) {
      press_count_2 = press_count_2 + 1;
    } else if (touchState_2 > 50) {
      press_status_2 = 0;
    }
  } 
  else if (millis() - press_millis_2 >= 1000) {
    if (touchState_2 < 50 && press_count_2 == 1) {
      led_color(0,5,0,200);
      delay(20);
      led_color(0,0,0,0);
      //oled_println("Long Button 2 Pressed");
      delay(500);
    } else if (touchState_2 < 50 && press_count_2 >= 2) {
      led_color(0,5,0,200);
      delay(20);
      led_color(0,0,0,0);
      //oled_println("Button 2 Clicked 2x");
    } else if (touchState_2 > 50 && press_count_2 == 1) {
      led_color(0,5,0,200);
      delay(20);
      led_color(0,0,0,0);
      X1_button_2();
      //oled_println("Button 2 Clicked");
    } else if (touchState_2 > 50 && press_count_2 >= 2) {
      led_color(0,5,0,200);
      delay(20);
      led_color(0,0,0,0);
      //oled_println("Button 2 Clicked 2x");
    }
    press_status_2 = 0;
    press_count_2 = 0;
    press_millis_2 = 0;
  }
*/

}








