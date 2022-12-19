/**********************************************************************************************************************
 * 
 * Демонстрационный проект публикации состояния входа на MQTT-брокер для ESP8266
 * 
 * Важно: проект собран на Arduino-платформе, но для VSCode + PlatformIO (не для Arduino IDE)
 * Как установить и настроить PlatformIO, вы можете узнать на моем сайте: https://kotyara12.ru/iot/crawl-to-pio/
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

// Номер выводов для подключения кнопок или датчиков
const int gpioInput1 = 12;
const int gpioInput2 = 13;
const int gpioInput3 = 14;

// Параметры подключения к WiFi
const char* wifiSSID = "k12iot";
const char* wifiPASS = "b4RJqsuW";

// Параметры подключения к MQTT брокеру 
// Примечание: использовать статический mqttClientId оптимальнее с точки зрения фрагментации кучи, только не забывайте изменять его на разных устройствах
const char* mqttServer   = "m5.wqtt.ru";
const int   mqttPort     = 2633;
const char* mqttClientId = "esp8266_inputs";
const char* mqttUser     = "u_LWAF1J";
const char* mqttPass     = "743WfNdM";

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

// Топики для публикации состояния входов
const char* mqttTopicStatusInput1    = "demo/input1/status";
const char* mqttTopicStatusInput2    = "demo/input2/status";
const char* mqttTopicStatusInput3    = "demo/input3/status";

// Текстовое отображение для состояния входов
const char* mqttInputStatusLow       = "0";
const char* mqttInputStatusHigh      = "1";
const bool  mqttInputStatusRetained  = false; 

/***********************************************************************************************************************
 * Глобальные переменные
 **********************************************************************************************************************/

// Корневой сертификат
BearSSL::X509List certISRG(ISRG_Root_x1);
BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Последнее опубликованное состояние реле
// Здесь маленькая хитрость: публикация в топик происходит только при наличии изменений
// (то есть когда gpioStatus1 != digitalRead()), то при первом подключении ничего не будет опубликовано
// Дабы форсировать события, проинициализируем переменные заведомо ложными данными, которых не будет при любом состоянии входов
byte gpioStatus1 = 2;
byte gpioStatus2 = 2;
byte gpioStatus3 = 2;

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
    Serial.print(" ");
    
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
    Serial.println(" ок");
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
    Serial.println(" ок");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    Serial.print(asctime(&timeinfo));
    
    // Теперь можно привязать корневой сертификат к клиенту WiFi
    wifiClient.setTrustAnchors(&certISRG);
  };
  return true;
}

// Подключение к MQTT брокеру :: версия для PubSubClient от knolleary ("стандартная")
bool mqttConnected()
{
  if (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker: ");
    // Настраиваем MQTT клиент
    mqttClient.setServer(mqttServer, mqttPort);
    
    // Пробуем подключиться с LWT сообщением "offline"
    if (mqttClient.connect(mqttClientId, mqttUser, mqttPass, mqttTopicDeviceStatus, mqttDeviceStatusQos, mqttDeviceStatusRetained, mqttDeviceStatusOff)) {
      Serial.println("ok");
      
      // Публикуем статус устройства в тот же топик, что и LWT, но с содержимым "online"
      mqttClient.publish(mqttTopicDeviceStatus, mqttDeviceStatusOn, mqttDeviceStatusRetained);
    } else {
      Serial.print("failed, error code: ");
      Serial.print(mqttClient.state());
      Serial.println("!");
    };
    return mqttClient.connected();
  };
  return true;
}

// Контроль состояния входов
void mqttReadInputsState()
{
  // Читаем вход 1
  byte gpioNewState1 = digitalRead(gpioInput1);
  if (gpioNewState1 != gpioStatus1) {
    gpioStatus1 = gpioNewState1;
  
    // gpioStatus1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow
    // это то же самое, что и
    // if (gpioStatus1 == HIGH) { mqttInputStatusHigh } else { mqttInputStatusLow }
    mqttClient.publish(mqttTopicStatusInput1, (gpioStatus1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow), mqttInputStatusRetained);

    Serial.print("Input 1 has changed its state: ");
    Serial.println(gpioStatus1 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow);
  };

  // Читаем вход 2
  byte gpioNewState2 = digitalRead(gpioInput2);
  if (gpioNewState2 != gpioStatus2) {
    gpioStatus2 = gpioNewState2;
  
    // gpioStatus2 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow
    // это то же самое, что и
    // if (gpioStatus2 == HIGH) { mqttInputStatusHigh } else { mqttInputStatusLow }
    mqttClient.publish(mqttTopicStatusInput2, (gpioStatus2 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow), mqttInputStatusRetained);

    Serial.print("Input 2 has changed its state: ");
    Serial.println(gpioStatus2 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow);
  };

  // Читаем вход 3
  byte gpioNewState3 = digitalRead(gpioInput3);
  if (gpioNewState3 != gpioStatus3) {
    gpioStatus3 = gpioNewState3;
  
    // gpioStatus3 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow
    // это то же самое, что и
    // if (gpioStatus3 == HIGH) { mqttInputStatusHigh } else { mqttInputStatusLow }
    mqttClient.publish(mqttTopicStatusInput3, (gpioStatus3 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow), mqttInputStatusRetained);

    Serial.print("Input 3 has changed its state: ");
    Serial.println(gpioStatus3 == HIGH ? mqttInputStatusHigh : mqttInputStatusLow);
  };
}

/***********************************************************************************************************************
 * Настройка устройства и основной цикл
 **********************************************************************************************************************/
void setup() {
  // Настройка вывода логов в порт serial 0
  Serial.begin(9600);
  Serial.println();
  Serial.println("Demo project FOR ESP8266");

  // Настройка выводов на вход
  pinMode(gpioInput1, INPUT);
  pinMode(gpioInput2, INPUT);
  pinMode(gpioInput3, INPUT);
}

void loop() {
  // Проверяем подключение к WiFi и MQTT
  if (wifiConnected() && mqttConnected()) {
    // Чтение состояния входов (настолько часто, как позволит длительность выполнения mqttClient.loop())
    mqttReadInputsState();

    // Основной цикл клиента MQTT
    mqttClient.loop();
  } else {
    // Даже если ничего не вышло с WiFi подключением, вы можете что-то сделать автономно
    // Например переключить реле по датчику температуры
    // Ваш роутер может просто выйти из строя, но автоматика должна продолжать работать

    // Но в данном случае мы просто чуть-чуть подождем и попробуем подключиться снова
    delay(1000);
  };
}