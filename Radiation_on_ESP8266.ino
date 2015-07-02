//Geiger counter SI-180G
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const int tachPIN=14; //на каком пине замеряем импульсы
const unsigned long sampleTime=300000; //Время подсчета импульсов в милисекундах (5 мин)
const int maxRPM = 10200; 
//постоянные для связи
const char* ssid = "..."; //SSID Беспроводной сети
const char* password = "..."; //Пароль беспроводной сети
const char* host = "yourhost.com"; //Адрес сервера, куда передаются данные
const char* key   = "yourkey"; //Ключ доступа к серверу
const int led = 13;
///
void setup() {
Serial.begin(9600);
   Serial.println("Geiger counter"); //Выводим информацию о функции устройство
   Serial.println("Sensor: SI-180G"); //Выводим информацию о сенсоре
   Serial.println("Measuring time: 5 min"); //Выводим информацию о времени замера импульсов с трубки гейгера
   Serial.println("Developer: VGC (https://twitter.com/generator_cher)"); //Выводим информацию о разработчике
 // Подключаемся к сети
   WiFi.mode(WIFI_STA); //Режим Wi-Fi клиент
   WiFi.begin(ssid, password); //Подключение в режиме клиента
  
  // Ожидаем подключения
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(led, 1);
    Serial.print(".");
    digitalWrite(led, 0);
    }
  Serial.println("");
  Serial.println("Client mode");// Говорим что мы в режиме клиент
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(led, 1);
  delay(5000);
  digitalWrite(led, 0);
}
void sendData()
{
  // Подсчитываем количество импульсов за выбранный промежуток времени(в милисекундах)
  int kount=0;
  boolean kflag=0;
  unsigned long currentTime=0;
  unsigned long startTime=millis();
  while (currentTime<=sampleTime)
{
    Serial.print(".");
   if (digitalRead(tachPIN)==HIGH)
    {
     kflag=HIGH;
    }

if (digitalRead(tachPIN)==LOW && kflag==HIGH)
    {
    kount++;
    kflag=LOW;
    }

currentTime=millis()-startTime;
}
Serial.print("Count: ");
Serial.print(kount);
Serial.println("");
Serial.print("Radiation: ");
Serial.print((kount/(sampleTime/60000))*0.31);
Serial.println("uRpH");
delay (5000);
//Формируем GET запрос к серверу
  Serial.print("connecting to ");
  Serial.println(host);
  // Используем WiFiClient класс для выполнения TCP подключения
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  delay(500);
  // Создаём URL запрос
  String url = "/temper/indata.php";
  //url += streamId;
  url += "?key=";
  url += key;
  url += "&RADI=";
  url += String((kount/(sampleTime/60000))*0.31);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  digitalWrite(led, 1);
  // Здесь мы отправляем запрос к удаленному серверу
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
 // Выводим то, что отправили к серверу
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
 digitalWrite(led, 0); 
  

}

void loop() {
 // Запускаем функцию 
sendData();
}
