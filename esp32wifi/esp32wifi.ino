#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <stdio.h>

// 定义定时器的时间间隔，单位为毫秒XXX
#define TIMER_INTERVAL xxxx
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// 定义定时器对象
hw_timer_t * timer = NULL;
// WiFi和MQTT的设置
const char* ssid = "XXXX"; // 你的WiFi名称
const char* password = "XXXX"; // 你的WiFi密码
const char* mqtt_server = "XXXXX"; // MQTT服务器地址
const char* mqtt_username = "XXXX"; // MQTT用户名
const char* mqtt_password = "XXXX"; // MQTT密码

// 通过MQTT发送消息的主题
const char* mqtt_topic = "/fish/status";

const char* sub_topic="/fish";

WiFiClient espClient;
PubSubClient client(espClient);

// 定义定时器中断处理函数
void IRAM_ATTR onTimer() {
  static bool ledOn = false;
  ledOn = !ledOn;
  digitalWrite(5, ledOn ? HIGH : LOW);
  Serial.println("iewudhweudhewi");
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // 连接WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // 连接到MQTT服务器
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("vegetableFish", mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected");
      client.subscribe(sub_topic);
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  dht.begin();
  // 初始化定时器
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, TIMER_INTERVAL * 1000, true);
  timerAlarmEnable(timer);
}

void loop() {
  // 保持MQTT连接
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  char buffer[100];
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C, Humidity: ");
  Serial.print(hum);
  Serial.println("%");
  sprintf(buffer,"{\"light\":\"on\",\"humidity\":\"%.2f\",\"temperature\":\"%.2f\"}",hum,temp);
  // 发布一条消息到MQTT主题
  client.publish(mqtt_topic, buffer);
  delay(5000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if (strcmp(topic, "/fish") == 0) {
    if ((char)payload[0] == '1') {
      digitalWrite(2, HIGH);    //  置 LED 所在引脚为低电平
      Serial.println("led on");
    } else {
      digitalWrite(2, LOW);   // 置 LED 所在引脚为高电平
      Serial.println("led off");
    }
  }
}

void reconnect() {
  // 重新连接到MQTT服务器
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected");
      client.subscribe(sub_topic);
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
