#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <OneWire.h>

/**************************************************************************
 * Настройки проекта
 *************************************************************************/
// Номера выводов для датчиков
const int gpioDHT22   = 5;
// Номера выводов для цифровых входов
const int gpioInput1  = 12;
// Номера выводов для подключения реле
const int gpioRelay1  = 14;
const int gpioRelay2  = 15;
const int gpioRelay3  = 16;

// Логические уровни для включения реле
// в данном случае предполагается, что реле включается высоким уровнем на выходе
const char lvlRelayOn  = 0x1;
const char lvlRelayOff = 0x0;

// Параметры подключения к WiFi
const char* wifiSSID  = "k12iot";
const char* wifiPASS  = "*********";

// Параметры подключения к MQTT брокеру 
// Примечание: использовать статический mqttClientId оптимальнее с точки зрения фрагментации кучи, только не забывайте изменять его на разных устройствах
const char* mqttServer   = "**.wqtt.ru";
const int   mqttPort     = 1234;
const int   mqttPortTLS  = 1235;
const char* mqttClientId = "esp8266_dzen";
const char* mqttUser     = "u_******";
const char* mqttPass     = "********";

// Корневой сертификат ISRG Root x1, действителен до 4 июня 2035 года
static const char ISRG_Root_x1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Топик LWT, одновременно это топик статуса устройства:
// - в рабочем состоянии в этом топике будет сообщение "online"
// - когда устройство по какой-либо причине отключится от сервера, сервер примерно через 60 секунд опубликует LWT "offline"
const char* mqttTopicDeviceStatus    = "demo/status";
const char* mqttDeviceStatusOn       = "online";    
const char* mqttDeviceStatusOff      = "offline";
const int   mqttDeviceStatusQos      = 1;
const bool  mqttDeviceStatusRetained = true; 

// Топики для OTA обновлений
const char* mqttTopicOTA             = "demo/ota";

// Топики для внешнего управления реле
const char* mqttTopicControlRelay1   = "demo/relay1/control";
const char* mqttTopicControlRelay2   = "demo/relay2/control";
const char* mqttTopicControlRelay3   = "demo/relay3/control";
const int   mqttRelayControlQos      = 1;

// Топики для публикации состояния реле
const char* mqttTopicStatusRelay1    = "demo/relay1/status";
const char* mqttTopicStatusRelay2    = "demo/relay2/status";
const char* mqttTopicStatusRelay3    = "demo/relay3/status";

// Текстовое отображение для состояния реле
const char* mqttRelayStatusOn1       = "1";
const char* mqttRelayStatusOn2       = "on";
const char* mqttRelayStatusOff1      = "0";
const char* mqttRelayStatusOff2      = "off";
const bool  mqttRelayStatusRetained  = false; 

// Топики для публикации состояния цифровых входов
const char* mqttTopicStatusInput1    = "demo/input1/status";
const bool  mqttInputStatusRetained  = false; 
const char* mqttInputStatusHigh      = "high";
const char* mqttInputStatusLow       = "low";

// Топики для публикации температуры и влажности
const char* mqttTopicTemperature     = "demo/dht22/temperature";
const char* mqttTopicHumidity        = "demo/dht22/humidity";
const bool  mqttSensorsRetained      = false; 


/**************************************************************************
 * Глобальные переменные
 *************************************************************************/

// Корневой сертификат
BearSSL::X509List certISRG(ISRG_Root_x1);
BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Текущее состояние реле
char relayStatus1 = lvlRelayOff;
char relayStatus2 = lvlRelayOff;
char relayStatus3 = lvlRelayOff;

// Полученное с MQTT ("новое") состояние реле
char relayCommand1 = lvlRelayOff;
char relayCommand2 = lvlRelayOff;
char relayCommand3 = lvlRelayOff;

// Текущее состояние цифровых входов
// Последнее опубликованное состояние реле
// Здесь маленькая хитрость: публикация в топик происходит только при наличии изменений
// (то есть когда inputStatus1 != digitalRead()), то при первом подключении ничего не будет опубликовано
// Дабы форсировать события, проинициализируем переменные заведомо ложными данными, которых не будет при любом состоянии входов
char inputStatus1 = 2;

// Датчик температуры и влажности
DHT dht(gpioDHT22, DHT22);


/**************************************************************************
 * Сервисные функции
 *************************************************************************/

// Подключение к WiFi точке доступа
bool wifiConnected()
{
  // Если подключение активно, то просто выходим и возвращаем true
  if (WiFi.status() != WL_CONNECTED) {
    // ... иначе пробуем подключиться к сети
    Serial.print("Connecting to WiFi AP ");
    Serial.print(wifiSSID);
    Serial.print(" ");
    
    // Настраиваем WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPASS);

    // И ждем подключения 60 циклов по 0,5 сек - это 30 секунд
    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      i++;
      if (i > 60) {
        // Если в течение этого времени не удалось подключиться - выходим с false
        // Бескорнечно ждать подключения опасно - если подключение было разорвано во время работы
        // нужно всё равно "обслуживать" реле и датчики, иначе может случиться беда
        Serial.println("");
        Serial.println("Connection failed!");
        return false;
      };
      Serial.print(".");
      delay(500);
    };

    // Подключение успешно установлено
    Serial.println(" ок");
    Serial.print("WiFi connected, obtained IP address: ");
    Serial.println(WiFi.localIP());

    // Для работы TLS-соединения нужны корректные дата и время, получаем их с NTP серверов
    configTime("MSK-3", "pool.ntp.org", "time.nist.gov");
    // Ждем, пока локальное время синхронизируется
    Serial.print("Waiting for NTP time sync: ");
    i = 0;
    time_t now = time(nullptr);
    while (now < 1000000000) {
      now = time(nullptr);
      i++;
      if (i > 60) {
        // Если в течение этого времени не удалось подключиться - выходим с false
        // Бескорнечно ждать подключения опасно - если подключение было разорвано во время работы
        // нужно всё равно "обслуживать" реле и датчики, иначе может случиться беда
        Serial.println("");
        Serial.println("Time sync failed!");
        return false;
      };
      Serial.print(".");
      delay(500);
    }

    // Время успешно синхронизировано, выводим его в монитор порта
    // Только для целей отладки, этот блок можно исключить
    Serial.println(" ok");
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));

    // Теперь можно привязать корневой сертификат к клиенту WiFi
    wifiClient.setTrustAnchors(&certISRG);
  };
  return true;
}

// Контроль состояния входов
void readDigitalInputs()
{
  // Читаем вход 1
  char gpioNewStatus1 = digitalRead(gpioInput1);
  if (gpioNewStatus1 != inputStatus1) {
    inputStatus1 = gpioNewStatus1;
  
    mqttClient.publish(mqttTopicStatusInput1, (inputStatus1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow), mqttInputStatusRetained);

    Serial.print("Input 1 has changed its state: ");
    Serial.println(inputStatus1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow);
  };
}

// Чтение температуры и влажности
void readTemperature()
{
  // Читаем данные с сенсора
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Вывод считанных данных в лог
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  // Публикуем данные на сервере
  String str_temp(t);
  mqttClient.publish(mqttTopicTemperature, str_temp.c_str(), mqttSensorsRetained);
  String str_humd(h);
  mqttClient.publish(mqttTopicHumidity, str_humd.c_str(), mqttSensorsRetained);
}

// Управление реле
void relayControl()
{
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (relayCommand1 != relayStatus1) {
    relayStatus1 = relayCommand1;
    digitalWrite(gpioRelay1, relayStatus1);
    
    // Отправляем подтверждение, что команда получена и обработана
    mqttClient.publish(mqttTopicStatusRelay1, (relayStatus1 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    
    // Можно вывести сообщение в лог
    Serial.print("Relay 1 has changed its state: ");
    Serial.println(relayStatus1 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2);
  };

  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (relayCommand2 != relayStatus2) {
    relayStatus2 = relayCommand2;
    digitalWrite(gpioRelay2, relayStatus2);

    // Отправляем подтверждение, что команда получена и обработана
    mqttClient.publish(mqttTopicStatusRelay2, (relayStatus2 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    
    // Можно вывести сообщение в лог
    Serial.print("Relay 2 has changed its state: ");
    Serial.println(relayStatus2 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2);
  };

  // Новое состояние реле 3 отличается от текущего, требуется переключение
  if (relayCommand3 != relayStatus3) {
    relayStatus3 = relayCommand3;
    digitalWrite(gpioRelay3, relayStatus3);

    // Отправляем подтверждение, что команда получена и обработана
    mqttClient.publish(mqttTopicStatusRelay3, (relayStatus3 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    
    // Можно вывести сообщение в лог
    Serial.print("Relay 3 has changed its state: ");
    Serial.println(relayStatus3 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2);
  };
}

// ОТА обновление
void otaStart(char* link)
{

}

// Функция обратного вызова при поступлении входящего сообщения от брокера
void mqttOnIncomingMsg(char* topic, byte* payload, unsigned int length)
{
  // Для более корректного сравнения строк приводим их к нижнему регистру и обрезаем пробелы с краев
  String _payload;
  for (unsigned int i = 0; i < length; i++) {
    _payload += String((char)payload[i]);
  };
  _payload.toLowerCase();
  _payload.trim();

  // Вывод поступившего сообщения в лог, больше никакого смысла этот блок кода не несет, можно исключить
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.print(_payload.c_str());
  Serial.println();

  // Сравниваем с топиками
  String _topic(topic);
  if (_topic.equals(mqttTopicOTA)) {
    // это топик со ссылкой на OTA обновление
    otaStart(_payload.c_str());
  } else if (_topic.equals(mqttTopicControlRelay1)) {
    // это топик управления реле 1
    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      relayCommand1 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOff2) || _payload.equals(mqttRelayStatusOff2)) {
      relayCommand1 = lvlRelayOff;
    };
  } else if (_topic.equals(mqttTopicControlRelay2)) {
    // это топик управления реле 2
    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      relayCommand2 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOff2) || _payload.equals(mqttRelayStatusOff2)) {
      relayCommand2 = lvlRelayOff;
    };
  } else if (_topic.equals(mqttTopicControlRelay3)) {
    // это топик управления реле 3
    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      relayCommand3 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOff2) || _payload.equals(mqttRelayStatusOff2)) {
      relayCommand3 = lvlRelayOff;
    };
  } else {
    Serial.println("Failed to recognize incoming topic!");
  };
}

// Подключение к MQTT брокеру :: версия для PubSubClient от knolleary ("стандартная")
bool mqttConnected()
{
  if (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker: ");
    // Настраиваем MQTT клиент
    mqttClient.setServer(mqttServer, mqttPortTLS);
    mqttClient.setCallback(mqttOnIncomingMsg);

    // Пробуем подключиться с LWT сообщением "offline"
    if (mqttClient.connect(mqttClientId, mqttUser, mqttPass, 
                           mqttTopicDeviceStatus, mqttDeviceStatusQos, mqttDeviceStatusRetained, mqttDeviceStatusOff)) {
      Serial.println("ok");

      // Публикуем статус устройства в тот же топик, что и LWT, но с содержимым "online"
      mqttClient.publish(mqttTopicDeviceStatus, mqttDeviceStatusOn, mqttDeviceStatusRetained);

      // Подписываемся на топики управления реле: поскольку топики заранее известны, никаких # и + здесь не нужно!
      mqttClient.subscribe(mqttTopicControlRelay1, mqttRelayControlQos);
      mqttClient.subscribe(mqttTopicControlRelay2, mqttRelayControlQos);
      mqttClient.subscribe(mqttTopicControlRelay3, mqttRelayControlQos);

      // Опубликуем текущие состояния реле
      mqttClient.publish(mqttTopicStatusRelay1, (relayStatus1 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
      mqttClient.publish(mqttTopicStatusRelay2, (relayStatus2 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
      mqttClient.publish(mqttTopicStatusRelay3, (relayStatus3 == 1 ? mqttRelayStatusOn2 : mqttRelayStatusOff2), mqttRelayStatusRetained);
    } else {
      Serial.print("failed, error code: ");
      Serial.print(mqttClient.state());
      Serial.println("!");
    };
    return mqttClient.connected();
  };
  return true;
}

/**************************************************************************
 * Основные функции
 *************************************************************************/

void setup() {
  // Настройка COM-порта
  Serial.begin(9600);
  Serial.println();
  Serial.println("Demo project FOR ESP8266");

  // Настройка выводов на вход
  pinMode(gpioInput1, INPUT);

  // Настройка выводов на выход
  pinMode(gpioRelay1, OUTPUT);
  pinMode(gpioRelay2, OUTPUT);
  pinMode(gpioRelay3, OUTPUT);

  // Сброс всех реле в исходное состояние "отключено"
  digitalWrite(gpioRelay1, relayStatus1);
  digitalWrite(gpioRelay2, relayStatus2);
  digitalWrite(gpioRelay3, relayStatus3);

  // Инициализация датчика температуры
  dht.begin();
}

void loop() {
  // Проверяем подключение к WiFi
  if (wifiConnected()) {
    // Подключение к WiFi установлено 
    if (mqttConnected()) {
      // Подключение к MQTT установлено 
      mqttClient.loop();
    };
  };

  // Управление реле
  relayControl();
  // Контроль состояния входов
  readDigitalInputs();

  // Чтение и публикация температуры каждые 10 секунд
  static unsigned long lastTempRead = 0;
  if ((millis() - lastTempRead) >= 10000) {
    lastTempRead = millis();
    readTemperature();
  };
}

