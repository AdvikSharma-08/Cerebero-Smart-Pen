#include "esp_camera.h"
#include <WiFi.h>
#include <IPAddress.h>
#include <HardwareSerial.h>


HardwareSerial MySerial1(0);
/*
const char* ssid = "";
const char* password = "";

const char* server_addr="";
int server_port=0;
*/
//  variables 
const char* ssid = ".";
const char* password = "12345678";

//const char* ssid = "PPC185D2-2.4G";
//const char* password = "12345678";

//const char* ssid = "VivoS";
//const char* password = "12345678";

const char* server_addr = "192.168.95.209";
//const char* server_addr = "192.168.1.8";
//const char* server_addr = "192.168.42.157";
//const char* server_addr = "192.168.43.209";
int server_port=7700;

IPAddress server_addr_ip;
WiFiClient client;
    
sensor_t * s_cam_attr;

// Camera configuration
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  10
#define SIOD_GPIO_NUM  40
#define SIOC_GPIO_NUM  39
#define Y9_GPIO_NUM    48
#define Y8_GPIO_NUM    11
#define Y7_GPIO_NUM    12
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    16
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    17
#define Y2_GPIO_NUM    15
#define VSYNC_GPIO_NUM 38
#define HREF_GPIO_NUM  47
#define PCLK_GPIO_NUM  13

void printType(const __FlashStringHelper* ifsh) {
  Serial.print(F("Type: "));
  Serial.println(ifsh);
}

template<typename T>
void printType(const T& val) {
  Serial.print(F("Type: "));
  Serial.println(F("Unknown"));
}

String string_n(String abc){
  String r="";
  for(int i=0;i<abc.length()-1;i++){
    if(abc[i]!='\n' && abc[i]!=' ' && abc[i]!='\0'){
      r=r+abc[i];
    }
  }
  return r;
}

void cameraConfig(framesize_t frame_size, int jpeg_quality){
  camera_config_t config;
  Serial.println("i'm getting called");
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  //config.frame_size = FRAMESIZE_VGA; //640 x 480
  config.frame_size=frame_size;
  //config.frame_size = FRAMESIZE_QQVGA; //160 x 120
  //config.frame_size = FRAMESIZE_96X96; //96 x 96
  config.jpeg_quality = jpeg_quality;
  config.fb_count = 2;
  // Camera init


  esp_err_t err = esp_camera_init(&config);
  /*
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  */
    if (err != ESP_OK) {
      Serial.printf("Camera reinit failed with error 0x%x", err);
    } else {
      Serial.println("Camera reinitialized successfully with new frame size");
  }
}

void setup() {
  String received1="";
  String received2="";
  String received3="";
  String received4="";
  String received5="";

  Serial.begin(9600);
  MySerial1.begin(9600, SERIAL_8N1, -1, -1);


 
/*
 delay(2000);
  while(1){
    if (MySerial1.available()) {
      received1 = MySerial1.readStringUntil('\n');
      break;
    }
  }
  while(1){
    if (MySerial1.available()) {
      received2 = MySerial1.readStringUntil('\n');
      break;
    }
  }
  while(1){
    if (MySerial1.available()) {
      received3 = MySerial1.readStringUntil('\n');
      break;
    }
  }
  while(1){
    if (MySerial1.available()) {
      received4 = MySerial1.readStringUntil('\n');
      break;
    }
  }

  delay(1000);
  while(1){
    if (MySerial1.available()) {
      received5 = MySerial1.readStringUntil('\n');
      Serial.println(received5);
      if(string_n(received5)=="1230"){
        break;
      }
    }
  }
*/

  //Serial.println(received1.length());
  //Serial.println(received2.length());
  //Serial.println(received3.length());
  //Serial.println(received4.length());

/*
  String rec1=string_n(received1);
  String rec2=string_n(received2);
  String rec3=string_n(received3);
  String rec4=string_n(received4);


  ssid=rec1.c_str();
  password=rec2.c_str();
  server_addr=rec3.c_str();
  server_port=rec4.toInt();


  if (!server_addr_ip.fromString(server_addr)) {
    Serial.println("Error: Invalid IP address format");
  }
*/

  //Serial.setDebugOutput(true);

  //Serial.println(received1);
  //Serial.println(received2);
  //Serial.println(received3);
  //Serial.println(received4);

  //Serial.println(ssid);
  //Serial.println(password);
  //Serial.println(server_addr);
  //Serial.println(server_port);

  //printType(ssid);
  //printType(password);
  //printType(server_addr);
  //printType(server_port);

  //Serial.println(String(ssid).length());
  //Serial.println(String(password).length());
  //Serial.println(String(server_addr).length());
  //Serial.println(String(server_port).length());

  //for(int i=0;i<String(ssid).length();i++){
  //  Serial.println(String(i)+"-"+String(ssid)[i]+"-");
  //}

  delay(3000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  MySerial1.println("1231");
  // Camera initialization

  cameraConfig(FRAMESIZE_QQVGA,40);


 
}


void de_int_esp(){
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    esp_camera_fb_return(fb);  // Return the frame buffer if one is in use
  }
  esp_err_t err = esp_camera_deinit();
  if (err != ESP_OK) {
    Serial.printf("Camera deinit failed with error 0x%x\n", err);
  } else {
    Serial.println("Camera deinitialized successfully");
  }
}
void loop() {
    String button_val="";
    int button=0;
    if (MySerial1.available()) {
      button_val = MySerial1.readStringUntil('\n');
      button_val=string_n(button_val);
      button = button_val.toInt();
      //Serial.println(button);
      if(button==1){
        de_int_esp();
        cameraConfig(FRAMESIZE_VGA,2);
        s_cam_attr = esp_camera_sensor_get();
        s_cam_attr->set_special_effect(s_cam_attr, 0);
        s_cam_attr->set_brightness(s_cam_attr, 2);
        s_cam_attr->set_contrast(s_cam_attr, 2);
        s_cam_attr->set_saturation(s_cam_attr, -2);
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {

          Serial.println("Camera capture failed");
          return;
        }
        Serial.println("image taken");
        sendImageRec(fb->buf, fb->len);
        esp_camera_fb_return(fb);
      }
      else if(button==2){
        de_int_esp();
        cameraConfig(FRAMESIZE_VGA,2);
        s_cam_attr = esp_camera_sensor_get();
        s_cam_attr->set_special_effect(s_cam_attr, 0);
        s_cam_attr->set_brightness(s_cam_attr, 2);
        s_cam_attr->set_contrast(s_cam_attr, 2);
        s_cam_attr->set_saturation(s_cam_attr, -2);
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {

          Serial.println("Camera capture failed");
          return;
        }
        Serial.println("image taken");
        sendImageCal(fb->buf, fb->len);
        esp_camera_fb_return(fb);
      }
      else if(button==4){
        de_int_esp();
        cameraConfig(FRAMESIZE_QQVGA,40);
        s_cam_attr = esp_camera_sensor_get();
        s_cam_attr->set_special_effect(s_cam_attr, 2);
        s_cam_attr->set_brightness(s_cam_attr, 0);
        s_cam_attr->set_contrast(s_cam_attr, 0);
        s_cam_attr->set_saturation(s_cam_attr,0);
        String stylus_end="";
        while(1){
          if (MySerial1.available()) {
            stylus_end = MySerial1.readStringUntil('\n');
            //Serial.println("---------------------------------------");
            //Serial.println(stylus_end);
            //Serial.println("---------------------------------------");
            stylus_end=string_n(stylus_end);
            if(stylus_end=="41"){
              break;
            }
          }
          camera_fb_t * fb = esp_camera_fb_get();
          if (!fb) {
            Serial.println("Camera capture failed");
            return;
          }
          Serial.println("image taken");
          sendImageStylus(fb->buf, fb->len);
          esp_camera_fb_return(fb);

        }
      }
      else if(button==3){
        de_int_esp();
        cameraConfig(FRAMESIZE_QQVGA,40);
        s_cam_attr = esp_camera_sensor_get();
        s_cam_attr->set_special_effect(s_cam_attr, 2);
        s_cam_attr->set_brightness(s_cam_attr, 0);
        s_cam_attr->set_contrast(s_cam_attr, 0);
        s_cam_attr->set_saturation(s_cam_attr, 0);
        String draw_end="";
        while(1){
          if (MySerial1.available()) {
            draw_end = MySerial1.readStringUntil('\n');
            //Serial.println("---------------------------------------");
            //Serial.println(draw_end);
            //Serial.println("---------------------------------------");
            draw_end=string_n(draw_end);
            if(draw_end=="31"){
              break;
            }
          }
          camera_fb_t * fb = esp_camera_fb_get();
          if (!fb) {
            Serial.println("Camera capture failed");
            return;
          }
          Serial.println("image taken");
          sendImageDraw(fb->buf, fb->len);
          esp_camera_fb_return(fb);

        }
      }
      else if(button==5){
        de_int_esp();
        cameraConfig(FRAMESIZE_VGA,2);
        s_cam_attr = esp_camera_sensor_get();
        s_cam_attr->set_special_effect(s_cam_attr, 0);
        s_cam_attr->set_brightness(s_cam_attr, 2);
        s_cam_attr->set_contrast(s_cam_attr, 2);
        s_cam_attr->set_saturation(s_cam_attr, -2);
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {

          Serial.println("Camera capture failed");
          return;
        }
        Serial.println("image taken");
        sendImagetranslateEH(fb->buf, fb->len);
        esp_camera_fb_return(fb);
      }
      else if(button==6){
        de_int_esp();
        cameraConfig(FRAMESIZE_VGA,2);
        s_cam_attr = esp_camera_sensor_get();
        s_cam_attr->set_special_effect(s_cam_attr, 0);
        s_cam_attr->set_brightness(s_cam_attr, 2);
        s_cam_attr->set_contrast(s_cam_attr, 2);
        s_cam_attr->set_saturation(s_cam_attr, -2);
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {

          Serial.println("Camera capture failed");
          return;
        }
        Serial.println("image taken");
        sendImagetranslateHe(fb->buf, fb->len);
        esp_camera_fb_return(fb);
      }
    }
}




void sendImageRec(uint8_t *image, size_t length) {
  if(WiFi.status() == WL_CONNECTED){
    if(client.connect(server_addr, server_port)){
      Serial.println("0 Connected to server");
      MySerial1.println("1 Image Send");
      Serial.println("1 Image Send");
      client.println("POST /upload_image_rec HTTP/1.1");
      client.println("Host: "+String(server_addr));
      client.println("Content-Type: image/jpeg");
      client.print("Content-Length: ");
      client.println(length);
      client.println();
      client.write(image, length);
      client.println();
      //client.stop();
    }
    else{
      MySerial1.println("0 Connection server failed");
      Serial.println("0 Connection to server failed");
    }
  }
  else {
    Serial.println("WiFi not connected");
  }
}

void sendImageCal(uint8_t *image, size_t length) {
  if(WiFi.status() == WL_CONNECTED){
    if(client.connect(server_addr, server_port)){
      Serial.println("0 Connected to server");
      MySerial1.println("1 Image Send");
      Serial.println("1 Image Send");
      client.println("POST /upload_image_rec_cal HTTP/1.1");
      client.println("Host: "+String(server_addr));
      client.println("Content-Type: image/jpeg");
      client.print("Content-Length: ");
      client.println(length);
      client.println();
      client.write(image, length);
      client.println();
      //client.stop();
    }
    else{
      MySerial1.println("0 Connection server failed");
      Serial.println("0 Connection to server failed");
    }
  }
  else {
    Serial.println("WiFi not connected");
  }
}

void sendImagetranslateEH(uint8_t *image, size_t length) {
  if(WiFi.status() == WL_CONNECTED){
    if(client.connect(server_addr, server_port)){
      Serial.println("0 Connected to server");
      MySerial1.println("1 Image Send");
      Serial.println("1 Image Send");
      client.println("POST /upload_image_translate_EH HTTP/1.1");
      client.println("Host: "+String(server_addr));
      client.println("Content-Type: image/jpeg");
      client.print("Content-Length: ");
      client.println(length);
      client.println();
      client.write(image, length);
      client.println();
      //client.stop();
    }
    else{
      MySerial1.println("0 Connection server failed");
      Serial.println("0 Connection to server failed");
    }
  }
  else {
    Serial.println("WiFi not connected");
  }
}

void sendImagetranslateHe(uint8_t *image, size_t length) {
  if(WiFi.status() == WL_CONNECTED){
    if(client.connect(server_addr, server_port)){
      Serial.println("0 Connected to server");
      MySerial1.println("1 Image Send");
      Serial.println("1 Image Send");
      client.println("POST /upload_image_translate_HE HTTP/1.1");
      client.println("Host: "+String(server_addr));
      client.println("Content-Type: image/jpeg");
      client.print("Content-Length: ");
      client.println(length);
      client.println();
      client.write(image, length);
      client.println();
      //client.stop();
    }
    else{
      MySerial1.println("0 Connection server failed");
      Serial.println("0 Connection to server failed");
    }
  }
  else {
    Serial.println("WiFi not connected");
  }
}
void sendImageStylus(uint8_t *image, size_t length) {
  if(WiFi.status() == WL_CONNECTED){
    if(client.connect(server_addr, server_port)){
      Serial.println("0 Connected to server");
      MySerial1.println("1 Image Send");
      Serial.println("1 Image Send");
      client.println("POST /upload_image_stylus HTTP/1.1");
      client.println("Host: "+String(server_addr));
      client.println("Content-Type: image/jpeg");
      client.print("Content-Length: ");
      client.println(length);
      client.println();
      client.write(image, length);
      client.println();
      //client.stop();
    }
    else{
      MySerial1.println("0 Connection server failed");
      Serial.println("0 Connection to server failed");
    }
  }
  else {
    Serial.println("WiFi not connected");
  }
}
void sendImageDraw(uint8_t *image, size_t length) {
  if(WiFi.status() == WL_CONNECTED){
    if(client.connect(server_addr, server_port)){
      Serial.println("0 Connected to server");
      MySerial1.println("1 Image Send");
      Serial.println("1 Image Send");
      client.println("POST /upload_image_draw HTTP/1.1");
      client.println("Host: "+String(server_addr));
      client.println("Content-Type: image/jpeg");
      client.print("Content-Length: ");
      client.println(length);
      client.println();
      client.write(image, length);
      client.println();
      //client.stop();
    }
    else{
      MySerial1.println("0 Connection server failed");
      Serial.println("0 Connection to server failed");
    }
  }
  else {
    Serial.println("WiFi not connected");
  }
}



