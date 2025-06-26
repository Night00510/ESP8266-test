#define BLYNK_TEMPLATE_ID                                 "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME                               "YOUR_BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN                                  "YOUR_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//alarm WiFi connection
wl_status_t alarm_wifi_status(wl_status_t current_wifi_status){
  switch (current_wifi_status){
    case WL_CONNECTED:
      Serial.print("WiFi: ");
      Serial.print(WiFi.SSID());
      Serial.println("\tCONNECTED");
      return WL_CONNECTED;

    case WL_CONNECT_FAILED:
      Serial.println("FAILED TO CONNECT");
      return WL_CONNECT_FAILED;

    case WL_CONNECTION_LOST:
      Serial.print("WiFi: ");
      Serial.print(WiFi.SSID());
      Serial.println("\tCONNECTION LOST");
      return WL_CONNECTION_LOST;

    case WL_NO_SHIELD:
      Serial.println("WiFi: Module not found");
      return WL_NO_SHIELD;

    case WL_NO_SSID_AVAIL:
      Serial.println("WiFi: No SSID available");
      return WL_NO_SSID_AVAIL;

    case WL_IDLE_STATUS:
      Serial.println("WiFi: Idle Status (Not connected/processing)");
      return WL_IDLE_STATUS;

    case WL_DISCONNECTED:
      Serial.println("WiFi: Disconnected");
      return WL_DISCONNECTED;

    default:
      Serial.print("WiFi: Unknown Status (Code: ");
      Serial.print(current_wifi_status);
      Serial.println(")");
      return current_wifi_status;
  }
}

//alarm blynk.Cloud connection
bool alarm_Blynk_status(){
  bool blynk_connection = Blynk.connected(); 
  if(blynk_connection)
  {
    Serial.println("Blynk CONNECTED");
    return blynk_connection;
  }else{
    Serial.println("Blynk NOT CONNECTED");
    return blynk_connection;
  }
}

//connection wifi
void wifi_connect() {
  struct Wifi_data {
    const char* ssid;
    const char* pass;
  };

  Wifi_data wifi_list[] = {
    {"YOUR_SSID 1", "YOUR_PASSWORD 1"},
    {"YOUR_SSID 2", "YOUR_PASSWORD 2"},
    //...
    {"YOUR_SSID n", "YOUR_PASSWORD n"},
  };

  const int num_wifi = sizeof(wifi_list) / sizeof(wifi_list[0]);//คำนวล wifi list idx 

  for (int i = 0; i < num_wifi; i++) {
    WiFi.begin(wifi_list[i].ssid, wifi_list[i].pass);
    Serial.print("CONNECTING TO: ");
    Serial.println(wifi_list[i].ssid);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 10000)) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    wl_status_t result = WiFi.status();
    alarm_wifi_status(result);

    if (result == WL_CONNECTED || result == WL_NO_SHIELD) {
      break;
    }
  }
}

//LED blibking
void LEDin_blink(){
  static unsigned long list_millis = 0;
  static bool state = false;
  if(millis() - list_millis >= 500){
    list_millis = millis();
    state = !state;
    digitalWrite(LED_BUILTIN, state ? LOW : HIGH);
    digitalWrite(D0, state ? HIGH : LOW);
    digitalWrite(D1, state ? LOW : HIGH);
  }
}

// V0 control LED blink_mode 
bool blink_mode = false;
BLYNK_WRITE(V0){
  blink_mode = param.asInt(); 
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(D0, LOW); // เริ่มต้นปิด LED D0
  digitalWrite(D1, LOW); // เริ่มต้นปิด LED D1
  pinMode(LED_BUILTIN,LOW);//เริ่มต้นปิด LED_BUILTIN (active low)
  Blynk.config(BLYNK_AUTH_TOKEN);

  Serial.begin(115200);
  Serial.println("\nESP8266 is Ready");
  wifi_connect();//ต่อ WiFi
  Blynk.connect(10000);//ต่อ Blynk.Cloud มี timeout 10 วิ
  alarm_Blynk_status();//เช็ค ต่อ Blynk.Cloud หรือยัง
}

void loop() {
  Blynk.run();
  if(blink_mode){
    LEDin_blink();
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(D0, LOW);
    digitalWrite(D1, LOW);
  }
}
