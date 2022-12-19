/**********************************************************************************************************************
 * 
 * Демонстрационный проект управления несколькими реле на ESP8266 через любой MQTT-брокер
 * 
 * Важно: проект собран на Arduino-платформе, но для VSCode + PlatformIO (не для Arduino IDE)
 * Как установить и настроить PlatformIO, вы можете узнать на моем сайте: https://kotyara12.ru/iot/crawl-to-pio/
 * 
 * ВНИМАНИЕ! Данная версия ИСПРОЛЬЗУЕТ TLS-ШИФРОВАНИЕ для подключения к MQTT-брокеру, поэтому она немного посложнее предыдущей
 * 
 * Впрочем, не составит особого труда перенести большую часть кода в обычную классическую ArduinoIDE
 * 
 **********************************************************************************************************************
 *
 * Автор: Александр Разживин aka kotyara12
 * 
 * Вы можете найти меня по следующим контактам:
 * https://kotyara12.ru, https://dzen.ru/kotyara12
 * mailto: kotyara12@yandex.ru, telegram: @kotyara1971
 * 
 *********************************************************************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>

/***********************************************************************************************************************
 * Настройки проекта
 **********************************************************************************************************************/

// Номер выводов для подключения реле
const int gpioRelay1 = 14;
const int gpioRelay2 = 15;
const int gpioRelay3 = 16;

// Параметры подключения к WiFi
const char* wifiSSID = "k12iot";
const char* wifiPASS = "xxxxxxxx";

// Параметры подключения к MQTT брокеру 
// Примечание: использовать статический mqttClientId оптимальнее с точки зрения фрагментации кучи, только не забывайте изменять его на разных устройствах
const char* mqttServer   = "xx.wqtt.ru";
const int   mqttPort     = 1234;
const char* mqttClientId = "esp8266_demo2";
const char* mqttUser     = "u_xxxx";
const char* mqttPass     = "xXxXxXxX";

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
const char* mqttRelayStatusOn        = "on";
const char* mqttRelayStatusOff       = "off";
const bool  mqttRelayStatusRetained  = false; 

/***********************************************************************************************************************
 * Глобальные переменные
 **********************************************************************************************************************/

// Корневой сертификат
BearSSL::X509List certISRG(ISRG_Root_x1);
BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Текущее состояние реле
byte gpioStatus1 = 0;
byte gpioStatus2 = 0;
byte gpioStatus3 = 0;

// Полученное ("новое") состояние реле
byte relaySetStatus1 = 0;
byte relaySetStatus2 = 0;
byte relaySetStatus3 = 0;

/***********************************************************************************************************************
 * Прикладные функции
 **********************************************************************************************************************/

// Подключение к WiFi точке доступа
bool wifiConnected()
{
  // Если подключение активно, то просто выходим и возвращаем true
  if (WiFi.status() != WL_CONNECTED) {
    // ... иначе пробуем подключиться к сети
    Serial.print("Connecting to WiFi AP ");
    Serial.print(wifiSSID);
    
    // Настраиваем объект WiFi
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
    Serial.println("");
    Serial.print("WiFi connected, obtained IP address: ");
    Serial.println(WiFi.localIP());

    // Для работы TLS-соединения нужны корректные дата и время, получаем их с NTP серверов
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
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
    Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
    
    // Теперь можно привязать корневой сертификат к клиенту WiFi
    wifiClient.setTrustAnchors(&certISRG);
  };
  return true;
}

// Функция обратного вызова при поступлении входящего сообщения от брокера
void mqttOnIncomingMsg(char* topic, byte* payload, unsigned int length)
{
  // Для более корректного сравнения строк приводим их к нижнему регистру и обрезаем пробелы с краев
  String _topic = topic;
  _topic.toLowerCase();
  _topic.trim();

  String _payload;
  for (unsigned int i = 0; i < length; i++) {
    _payload += String((char)payload[i]);
  };
  _payload.toLowerCase();
  _payload.trim();

  // Вывод поступившего сообщения в лог, больше никакого смысла этот блок кода не несет, можно исключить
  Serial.print("Message arrived [");
  Serial.print(_topic.c_str());
  Serial.print("]: ");
  Serial.print(_payload.c_str());
  Serial.println();

  // Сравниваем с топиками
  if (_topic.equals(mqttTopicControlRelay1)) {
    // это топик управления реле 1
    if (_payload.equals("1") || _payload.equals(mqttRelayStatusOn)) relaySetStatus1 = 1;
    if (_payload.equals("0") || _payload.equals(mqttRelayStatusOff)) relaySetStatus1 = 0;
  } else if (_topic.equals(mqttTopicControlRelay2)) {
    // это топик управления реле 2
    if (_payload.equals("1") || _payload.equals(mqttRelayStatusOn)) relaySetStatus2 = 1;
    if (_payload.equals("0") || _payload.equals(mqttRelayStatusOff)) relaySetStatus2 = 0;
  } else if (_topic.equals(mqttTopicControlRelay3)) {
    // это топик управления реле 3
    if (_payload.equals("1") || _payload.equals(mqttRelayStatusOn)) relaySetStatus3 = 1;
    if (_payload.equals("0") || _payload.equals(mqttRelayStatusOff)) relaySetStatus3 = 0;
  } else {
    Serial.println("Failed to recognize incoming topic!");
  };
}

// Переключение состояния реле
void mqttChangeRelaysState()
{
  // Новое состояние реле 1 отличается от текущего, требуется переключение
  if (relaySetStatus1 != gpioStatus1) {
    gpioStatus1 = relaySetStatus1;
    digitalWrite(gpioRelay1, gpioStatus1);
    
    // gpioStatus1 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff 
    // это то же самое, что и
    // if (gpioStatus1 == 1) { mqttRelayStatusOn } else { mqttRelayStatusOff }
    mqttClient.publish(mqttTopicStatusRelay1, (gpioStatus1 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff), mqttRelayStatusRetained);
    
    Serial.print("Relay 1 has changed its state: ");
    Serial.println(gpioStatus1 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff);
  };

  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (relaySetStatus2 != gpioStatus2) {
    gpioStatus2 = relaySetStatus2;
    digitalWrite(gpioRelay2, gpioStatus2);

    // gpioStatus2 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff 
    // это то же самое, что и
    // if (gpioStatus2 == 1) { mqttRelayStatusOn } else { mqttRelayStatusOff }
    mqttClient.publish(mqttTopicStatusRelay2, (gpioStatus2 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff), mqttRelayStatusRetained);
    
    Serial.print("Relay 2 has changed its state: ");
    Serial.println(gpioStatus2 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff);
  };

  // Новое состояние реле 3 отличается от текущего, требуется переключение
  if (relaySetStatus3 != gpioStatus3) {
    gpioStatus3 = relaySetStatus3;
    digitalWrite(gpioRelay3, gpioStatus3);

    // gpioStatus3 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff 
    // это то же самое, что и
    // if (gpioStatus3 == 1) { mqttRelayStatusOn } else { mqttRelayStatusOff }
    mqttClient.publish(mqttTopicStatusRelay3, (gpioStatus3 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff), mqttRelayStatusRetained);
    
    Serial.print("Relay 3 has changed its state: ");
    Serial.println(gpioStatus3 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff);
  };
}

// Подключение к MQTT брокеру :: версия для PubSubClient от knolleary ("стандартная")
bool mqttConnected()
{
  if (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker: ");
    // Настраиваем MQTT клиент
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(mqttOnIncomingMsg);
    
    // Пробуем подключиться с LWT сообщением "offline"
    if (mqttClient.connect(mqttClientId, mqttUser, mqttPass, mqttTopicDeviceStatus, mqttDeviceStatusQos, mqttDeviceStatusRetained, mqttDeviceStatusOff)) {
      Serial.println("ok");
      
      // Публикуем статус устройства в тот же топик, что и LWT, но с содержимым "online"
      mqttClient.publish(mqttTopicDeviceStatus, mqttDeviceStatusOn, mqttDeviceStatusRetained);

      // Подписываемся на топики управления реле: поскольку топики зарашее известны, никаких # и + здесь не нужно!
      mqttClient.subscribe(mqttTopicControlRelay1, mqttRelayControlQos);
      mqttClient.subscribe(mqttTopicControlRelay2, mqttRelayControlQos);
      mqttClient.subscribe(mqttTopicControlRelay3, mqttRelayControlQos);

      // Публикуем текущее состояние реле
      // gpioStatus1 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff 
      // это то же самое, что и
      // if (gpioStatus1 == 1) { mqttRelayStatusOn } else { mqttRelayStatusOff }
      mqttClient.publish(mqttTopicStatusRelay1, (gpioStatus1 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff), mqttRelayStatusRetained);
      mqttClient.publish(mqttTopicStatusRelay2, (gpioStatus2 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff), mqttRelayStatusRetained);
      mqttClient.publish(mqttTopicStatusRelay3, (gpioStatus3 == 1 ? mqttRelayStatusOn : mqttRelayStatusOff), mqttRelayStatusRetained);
    } else {
      Serial.print("failed, error code: ");
      Serial.print(mqttClient.state());
      Serial.println("!");
    };
    return mqttClient.connected();
  };
  return true;
}

/***********************************************************************************************************************
 * Настройка устройства и основной цикл
 **********************************************************************************************************************/
void setup() {
  // Настройка вывода логов в порт serial 0
  Serial.begin(9600);
  Serial.println();
  Serial.println("Demo project FOR ESP8266");

  // Настройка выводов
  pinMode(gpioRelay1, OUTPUT);
  pinMode(gpioRelay2, OUTPUT);
  pinMode(gpioRelay3, OUTPUT);

  // Сброс всех реле в исходное состояние "отключено"
  digitalWrite(gpioRelay1, gpioStatus1);
  digitalWrite(gpioRelay2, gpioStatus2);
  digitalWrite(gpioRelay3, gpioStatus3);
}

void loop() {
  // Проверяем подключение к WiFi и MQTT
  if (wifiConnected() && mqttConnected()) {
    // Основной цикл клиента MQTT
    mqttClient.loop();

    ///////////////////////////////////////////////////////////////
    // Ваша прошивка здесь может делать что-то еще...
    // Состояние: Когда все подключено и все работает
    ///////////////////////////////////////////////////////////////
  } else {
    // Даже если ничего не вышло с WiFi подключением, вы можете что-то сделать автономно
    // Например переключить реле по датчику температуры
    // Ваш роутер может просто выйти из строя, но автоматика должна продолжать работать

    ///////////////////////////////////////////////////////////////
    // Ваша прошивка здесь может делать что-то еще...
    // Состояние: Когда с подключениями приключилась беда
    ///////////////////////////////////////////////////////////////

    // Но в данном случае мы просто чуть-чуть подождем и попробуем подключиться снова
    delay(1000);
  };

  mqttChangeRelaysState();

  ///////////////////////////////////////////////////////////////
  // Ваша прошивка здесь может делать что-то еще...
  // Состояние: Вообще пофигу на всё
  ///////////////////////////////////////////////////////////////
}