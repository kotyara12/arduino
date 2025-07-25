#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
<<<<<<< HEAD
=======
#include <ESP8266HTTPClient.h>
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
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
const char* wifiSSID  = "K12";
const char* wifiPASS  = "K1234567";

// Параметры подключения к MQTT брокеру 
// Примечание: использовать статический mqttClientId оптимальнее с точки зрения фрагментации кучи, только не забывайте изменять его на разных устройствах
<<<<<<< HEAD
const char* mqttServer   = "**.wqtt.ru";
const int   mqttPort     = 1234;
const int   mqttPortTLS  = 1235;
const char* mqttClientId = "esp8266_ota";
const char* mqttUser     = "u_******";
const char* mqttPass     = "********";

// Корневой сертификат ISRG Root x1, действителен до 4 июня 2035 года
static const char ISRG_Root_x1[] PROGMEM = R"EOF(
=======
const char* mqttServer   = "z9.wqtt.ru";
const int   mqttPort     = 9998;
const int   mqttPortTLS  = 9999;
const char* mqttClientId = "esp8266_telegram";
const char* mqttUser     = "u_test";
const char* mqttPass     = "EaxtnZGg";

// Параметры Telegram бота для отправки сообщений
const char* tgToken      = "9341065673:AAFMkznAhvFsYGKdtILaNUqg93yh_qAQoHk";
const char* tgChatId     = "-4085399558";

// Корневой сертификат ISRG Root x1, действителен до 4 июня 2035 года
static const char ISRG_Root_x1[] PROGMEM = R"CERT(
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
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
<<<<<<< HEAD
)EOF";
=======
)CERT";

// Корневой сертификат для Telegram API, действителен до 29 июня 2034
static const char TG_API_Root[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIEADCCAuigAwIBAgIBADANBgkqhkiG9w0BAQUFADBjMQswCQYDVQQGEwJVUzEh
MB8GA1UEChMYVGhlIEdvIERhZGR5IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBE
YWRkeSBDbGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTA0MDYyOTE3
MDYyMFoXDTM0MDYyOTE3MDYyMFowYzELMAkGA1UEBhMCVVMxITAfBgNVBAoTGFRo
ZSBHbyBEYWRkeSBHcm91cCwgSW5jLjExMC8GA1UECxMoR28gRGFkZHkgQ2xhc3Mg
MiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTCCASAwDQYJKoZIhvcNAQEBBQADggEN
ADCCAQgCggEBAN6d1+pXGEmhW+vXX0iG6r7d/+TvZxz0ZWizV3GgXne77ZtJ6XCA
PVYYYwhv2vLM0D9/AlQiVBDYsoHUwHU9S3/Hd8M+eKsaA7Ugay9qK7HFiH7Eux6w
wdhFJ2+qN1j3hybX2C32qRe3H3I2TqYXP2WYktsqbl2i/ojgC95/5Y0V4evLOtXi
EqITLdiOr18SPaAIBQi2XKVlOARFmR6jYGB0xUGlcmIbYsUfb18aQr4CUWWoriMY
avx4A6lNf4DD+qta/KFApMoZFv6yyO9ecw3ud72a9nmYvLEHZ6IVDd2gWMZEewo+
YihfukEHU1jPEX44dMX4/7VpkI+EdOqXG68CAQOjgcAwgb0wHQYDVR0OBBYEFNLE
sNKR1EwRcbNhyz2h/t2oatTjMIGNBgNVHSMEgYUwgYKAFNLEsNKR1EwRcbNhyz2h
/t2oatTjoWekZTBjMQswCQYDVQQGEwJVUzEhMB8GA1UEChMYVGhlIEdvIERhZGR5
IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBEYWRkeSBDbGFzcyAyIENlcnRpZmlj
YXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQAD
ggEBADJL87LKPpH8EsahB4yOd6AzBhRckB4Y9wimPQoZ+YeAEW5p5JYXMP80kWNy
OO7MHAGjHZQopDH2esRU1/blMVgDoszOYtuURXO1v0XJJLXVggKtI3lpjbi2Tc7P
TMozI+gciKqdi0FuFskg5YmezTvacPd+mSYgFFQlq25zheabIZ0KbIIOqPjCDPoQ
HmyW74cNxA9hi63ugyuV+I6ShHI56yDqg+2DzZduCLzrTia2cyvk0/ZM/iZx4mER
dEr/VxqHD3VILs9RaRegAhJhldXRQLIQTO7ErBBDpqWeCtWVYpoNz4iCxTIM5Cuf
ReYNnyicsbkqWletNw+vHX/bvZ8=
-----END CERTIFICATE-----
)CERT";
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8

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
const int   mqttOTAQos               = 0;

// Топики для внешнего управления реле
const char* mqttTopicControlRelay1   = "demo/relay1/control";
const char* mqttTopicControlRelay2   = "demo/relay2/control";
const char* mqttTopicControlRelay3   = "demo/relay3/control";
const int   mqttRelayControlQos      = 1;

// Топики для публикации состояния реле
const char* mqttTopicStatusRelay1    = "demo/relay1/status";
const char* mqttTopicStatusRelay2    = "demo/relay2/status";
const char* mqttTopicStatusRelay3    = "demo/relay3/status";

// Текстовое отображение для состояния реле (два варианта : 0/1 и on/off)
const char* mqttRelayStatusOnD       = "1";
const char* mqttRelayStatusOnS       = "on";
const char* mqttRelayStatusOffD      = "0";
const char* mqttRelayStatusOffS      = "off";
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

<<<<<<< HEAD
// Корневой сертификат
BearSSL::X509List certISRG(ISRG_Root_x1);
BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
=======
// Подключение к MQTT брокеру (серверу)
// !!! Поскольку у ESP8266 памяти очень мало (~40 kB), нам придется пожертвовать "постоянным" TLS-подключением к MQTT
// BearSSL::X509List certISRG(ISRG_Root_x1);
WiFiClient wifiMqtt; // BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiMqtt);
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8

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
<<<<<<< HEAD
    wifiClient.setTrustAnchors(&certISRG);
=======
    // wifiClient.setTrustAnchors(&certISRG);
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
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
    mqttClient.publish(mqttTopicStatusRelay1, (relayStatus1 == 1 ? mqttRelayStatusOnD : mqttRelayStatusOffD), mqttRelayStatusRetained);
    
    // Можно вывести сообщение в лог
    Serial.print("Relay 1 has changed its state: ");
    Serial.println(relayStatus1 == 1 ? mqttRelayStatusOnS : mqttRelayStatusOffS);
  };

  // Новое состояние реле 2 отличается от текущего, требуется переключение
  if (relayCommand2 != relayStatus2) {
    relayStatus2 = relayCommand2;
    digitalWrite(gpioRelay2, relayStatus2);

    // Отправляем подтверждение, что команда получена и обработана
    mqttClient.publish(mqttTopicStatusRelay2, (relayStatus2 == 1 ? mqttRelayStatusOnD : mqttRelayStatusOffD), mqttRelayStatusRetained);
    
    // Можно вывести сообщение в лог
    Serial.print("Relay 2 has changed its state: ");
    Serial.println(relayStatus2 == 1 ? mqttRelayStatusOnS : mqttRelayStatusOffS);
  };

  // Новое состояние реле 3 отличается от текущего, требуется переключение
  if (relayCommand3 != relayStatus3) {
    relayStatus3 = relayCommand3;
    digitalWrite(gpioRelay3, relayStatus3);

    // Отправляем подтверждение, что команда получена и обработана
    mqttClient.publish(mqttTopicStatusRelay3, (relayStatus3 == 1 ? mqttRelayStatusOnD : mqttRelayStatusOffD), mqttRelayStatusRetained);
    
    // Можно вывести сообщение в лог
    Serial.print("Relay 3 has changed its state: ");
    Serial.println(relayStatus3 == 1 ? mqttRelayStatusOnS : mqttRelayStatusOffS);
  };
}

// ОТА обновление
void otaStart(const char* linkOTA)
{
  Serial.print("OTA :: Receiving OTA: ");
  Serial.print(linkOTA);
  Serial.println("...");

<<<<<<< HEAD
  WiFiClientSecure otaWiFi;
  // Запускаем обновление
  t_httpUpdate_return ret = ESPhttpUpdate.update(otaWiFi, linkOTA);
=======
  // Настраиваем безопасное подключение к серверу
  BearSSL::WiFiClientSecure wifiOTA;
  BearSSL::X509List certISRG(ISRG_Root_x1);
  wifiOTA.setTrustAnchors(&certISRG);

  // Запускаем обновление
  t_httpUpdate_return ret = ESPhttpUpdate.update(wifiOTA, linkOTA);
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
  // Анализируем результат
  switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("OTA :: Update failed");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("OTA :: Update no Updates");
      break;
    case HTTP_UPDATE_OK:
      // А вот это сообщение не факт, что вы увидите, потому что esp будет перезагружена
      Serial.println("OTA :: Update OK");
      break;
  };
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
    if (_payload.equals(mqttRelayStatusOnD) || _payload.equals(mqttRelayStatusOnS)) {
      relayCommand1 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOffS) || _payload.equals(mqttRelayStatusOffS)) {
      relayCommand1 = lvlRelayOff;
    };
  } else if (_topic.equals(mqttTopicControlRelay2)) {
    // это топик управления реле 2
    if (_payload.equals(mqttRelayStatusOnD) || _payload.equals(mqttRelayStatusOnS)) {
      relayCommand2 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOffS) || _payload.equals(mqttRelayStatusOffS)) {
      relayCommand2 = lvlRelayOff;
    };
  } else if (_topic.equals(mqttTopicControlRelay3)) {
    // это топик управления реле 3
    if (_payload.equals(mqttRelayStatusOnD) || _payload.equals(mqttRelayStatusOnS)) {
      relayCommand3 = lvlRelayOn;
    };
    if (_payload.equals(mqttRelayStatusOffS) || _payload.equals(mqttRelayStatusOffS)) {
      relayCommand3 = lvlRelayOff;
    };
  } else {
    Serial.println("Failed to recognize incoming topic!");
  };
}

<<<<<<< HEAD
// Отправка сообщения в Telegram
bool telegramSendMessage(const char* chatId, char* message)
{
  return false;
=======
/*****************************************************************************************
 * Отправка сообщения в Telegram
 * -----------------------------
 * Разными способами - вы можете использовать любой по вкусу и необходимости
 ****************************************************************************************/

// Вариант 1: GET-запросом, без перекодирования служебных символов - вы должны подготовить исходную строку соответствующим образом
// С использованием локальных экземпляров объектов в стеке, с отправкой заголовков отдельными командами wifiTg.print
void telegramSendMessage_Get_Raw(const char* chatId, char* message)
{
  Serial.print("TG :: Send message: ");
  Serial.print(message);
  Serial.println("...");

  // Настраиваем безопасное подключение к API
  WiFiClientSecure wifiTg;
  // wifiTg.setInsecure();
  X509List certTg(TG_API_Root);
  wifiTg.setTrustAnchors(&certTg);

  // Пробуем подключиться к Telegram API: обратите внимание - API принимает входящие запросы только по HTTPS на 443 порту
  if (wifiTg.connect("api.telegram.org", 443)) {
    // Формируем и отправляем GET-запрос для отправки сообщения
    wifiTg.printf("GET https://api.telegram.org/bot%s/sendMessage?chat_id=%s&parse_mode=HTML&text=%s HTTP/1.1\r\n", tgToken, chatId, message);
    // Отправляем служебные HTTP-заголовки
    wifiTg.println(F("Host: api.telegram.org"));
    wifiTg.println(F("User-Agent: ESP8266"));
    // Отправляем пустую строку, которая указывает серверу, что запрос полностью отправлен
    wifiTg.println();
    
    // Читаем ответ сервера (только первую строку)
    Serial.print(F("TG :: API responce: "));
    Serial.println(wifiTg.readStringUntil('\n'));
    // Закрываем соединение
    wifiTg.stop();
  } else {
    Serial.printf("#ERROR# :: Failed to connect to Telegram API: %d\r\n", wifiTg.getLastSSLError());
  };
}

// Вариант 2: GET-запросом, с перекодированием служебных символов - пишите как Вам удобно
// C использованием локальных экземпляров объектов в heap (куче) и формированием запроса "целиком" по шаблону

static const char* tgGetRequest = "GET https://api.telegram.org/bot%s/sendMessage?chat_id=%s&parse_mode=HTML&text=%s HTTP/1.1\r\n"
                                  "Host: api.telegram.org\r\n"
                                  "User-Agent: ESP8266\r\n\r\n";

void telegramSendMessage_Get_Encoded(const char* chatId, char* message)
{
  Serial.print("TG :: Send message: ");
  Serial.print(message);
  Serial.println("...");

  // Percent-encoding символов в URI на HTML-коды %XX
  String msg = String(message);
  msg.replace("\n", "%0A");
  msg.replace("\r", "%0D");
  
  // Настраиваем безопасное подключение к API с размещением временных объектов в куче (heap)
  WiFiClientSecure *wifiTg = new WiFiClientSecure;
  // wifiTg->setInsecure();
  X509List *certTg = new X509List(TG_API_Root);
  wifiTg->setTrustAnchors(certTg);

  // Пробуем подключиться к Telegram API: обратите внимание - API принимает входящие запросы только по HTTPS на 443 порту
  if (wifiTg->connect("api.telegram.org", 443)) {
    // Формируем и отправляем GET-запрос для отправки сообщения (сразу целиком, по шаблону)
    wifiTg->printf(tgGetRequest, tgToken, chatId, msg.c_str());
    
    // Читаем ответ сервера (только первую строку)
    Serial.print(F("TG :: API responce code: "));
    Serial.println(wifiTg->readStringUntil('\n'));
    // Закрываем соединение
    wifiTg->stop();
  } else {
    Serial.printf("#ERROR# :: Failed to connect to Telegram API: %d\r\n", wifiTg->getLastSSLError());
  };

  // Не забываем удалить данные, размещенные в куче
  delete wifiTg;
  delete certTg;
}

// Вариант 3: POST-запросом через WiFiClient
// C использованием локальных экземпляров объектов в куче и формированием запроса "целиком" по шаблону

static const char* tgPostRequest = "POST https://api.telegram.org/bot%s/sendMessage HTTP/1.1\r\n"
                                   "Host: api.telegram.org\r\n"
                                   "User-Agent: ESP8266\r\n"
                                   "Content-Type: application/json\r\n"
                                   "Content-Length: %d\r\n\r\n"
                                   "%s\r\n\r\n";

void telegramSendMessage_Post(const char* chatId, char* message)
{
  Serial.print("TG :: Send message: ");
  Serial.print(message);
  Serial.println("...");

  // Формируем JSON-пакет с данными
  String sJSON = "{\"chat_id\":" + String(chatId) + ",\"parse_mode\":\"HTML\",\"disable_notification\":false,\"text\":\"" + String(message) + "\"}";
  Serial.println(sJSON.c_str());

  // Настраиваем безопасное подключение к API с размещением временных объектов в куче (heap)
  WiFiClientSecure *wifiTg = new WiFiClientSecure;
  // wifiTg->setInsecure();
  X509List *certTg = new X509List(TG_API_Root);
  wifiTg->setTrustAnchors(certTg);

  // Пробуем подключиться к Telegram API: обратите внимание - API принимает входящие запросы только по HTTPS на 443 порту
  if (wifiTg->connect("api.telegram.org", 443)) {
    // Формируем и отправляем POST-запрос для отправки сообщения (сразу целиком, по шаблону)
    wifiTg->printf(tgPostRequest, tgToken, sJSON.length(), sJSON.c_str());
    
    // Читаем ответ сервера (только первую строку)
    Serial.print(F("TG :: API responce code: "));
    Serial.println(wifiTg->readStringUntil('\n'));
    // Закрываем соединение
    wifiTg->stop();
  } else {
    Serial.printf("#ERROR# :: Failed to connect to Telegram API: %d\r\n", wifiTg->getLastSSLError());
  };

  // Не забываем удалить данные, размещенные в куче
  delete wifiTg;
  delete certTg;
}

// Вариант 4: POST-запросом через HttpClient
// C использованием локальных экземпляров объектов в стеке, формированием запроса занимается HTTPClient
void telegramSendMessage_Post_Http(const char* chatId, char* message)
{
  Serial.print("TG :: Send message: ");
  Serial.print(message);
  Serial.println("...");

  // Формируем JSON-пакет с данными
  String sJSON = "{\"chat_id\":" + String(chatId) + ",\"parse_mode\":\"HTML\",\"disable_notification\":false,\"text\":\"" + String(message) + "\"}";
  Serial.println(sJSON.c_str());

  // Настраиваем безопасное подключение к API
  WiFiClientSecure wifiTg;
  HTTPClient tgHttp;
  // wifiTg.setInsecure();
  X509List certTg(TG_API_Root);
  wifiTg.setTrustAnchors(&certTg);

  // Формируем динамическую URI
  String sURI = "https://api.telegram.org/bot" + String(tgToken) + "/sendMessage";
  // Пробуем подключиться к Telegram API
  if (tgHttp.begin(wifiTg, sURI.c_str())) {
    // Добавляем заголовок с типом данных
    tgHttp.addHeader("Content-Type", "application/json");
    // Формируем и отправляем POST-запрос для отправки сообщения
    int httpCode = tgHttp.POST(sJSON);
    // Выводим в лог код ответа сервера (только первую строку)
    Serial.printf("TG :: API responce code: %d\r\n", httpCode);
    // Закрываем соединение
    tgHttp.end();
  } else {
    Serial.printf("#ERROR# :: Failed to connect to Telegram API: %d\r\n", wifiTg.getLastSSLError());
  };
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
}

// Подключение к MQTT брокеру :: версия для PubSubClient от knolleary ("стандартная")
bool mqttConnected()
{
  if (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker: ");
<<<<<<< HEAD
    // Настраиваем MQTT клиент
    mqttClient.setServer(mqttServer, mqttPortTLS);
=======

    // Настраиваем MQTT клиент
    mqttClient.setServer(mqttServer, mqttPort);
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
    mqttClient.setCallback(mqttOnIncomingMsg);

    // Пробуем подключиться с LWT сообщением "offline"
    if (mqttClient.connect(mqttClientId, mqttUser, mqttPass, 
                           mqttTopicDeviceStatus, mqttDeviceStatusQos, mqttDeviceStatusRetained, mqttDeviceStatusOff)) {
      Serial.println("ok");

      // Публикуем статус устройства в тот же топик, что и LWT, но с содержимым "online"
      mqttClient.publish(mqttTopicDeviceStatus, mqttDeviceStatusOn, mqttDeviceStatusRetained);

      // Подписываемся на топик OTA
      mqttClient.subscribe(mqttTopicOTA, mqttOTAQos);

      // Подписываемся на топики управления реле: поскольку топики заранее известны, никаких # и + здесь не нужно!
      mqttClient.subscribe(mqttTopicControlRelay1, mqttRelayControlQos);
      mqttClient.subscribe(mqttTopicControlRelay2, mqttRelayControlQos);
      mqttClient.subscribe(mqttTopicControlRelay3, mqttRelayControlQos);

      // Опубликуем текущие состояния реле
      mqttClient.publish(mqttTopicStatusRelay1, (relayStatus1 == 1 ? mqttRelayStatusOnD : mqttRelayStatusOffD), mqttRelayStatusRetained);
      mqttClient.publish(mqttTopicStatusRelay2, (relayStatus2 == 1 ? mqttRelayStatusOnD : mqttRelayStatusOffD), mqttRelayStatusRetained);
      mqttClient.publish(mqttTopicStatusRelay3, (relayStatus3 == 1 ? mqttRelayStatusOnD : mqttRelayStatusOffD), mqttRelayStatusRetained);
<<<<<<< HEAD
=======

      // Отправляем уведомление в Telegram о том, что подключение успешно выполнено
      // Пробуем все вариантыЖ в реальном приложении используйте только один способ
      telegramSendMessage_Get_Raw(tgChatId, (char*)"Версия telegramSendMessage_Get_Raw (не правильно)\n<b>Жирный шрифт</b>\n<i>Наклонный шрифт</i>\n<u>Подчеркнутый шрифт</u>");
      telegramSendMessage_Get_Raw(tgChatId, (char*)"Версия telegramSendMessage_Get_Raw (правильно)%0A<b>Жирный шрифт</b>%0A<i>Наклонный шрифт</i>%0A<u>Подчеркнутый шрифт</u>");
      telegramSendMessage_Get_Encoded(tgChatId, (char*)"Версия telegramSendMessage_Get_Encoded\n<b>Жирный шрифт</b>\n<i>Наклонный шрифт</i>\n<u>Подчеркнутый шрифт</u>");
      telegramSendMessage_Post(tgChatId, (char*)"Версия telegramSendMessage_Post\n<b>Жирный шрифт</b>\n<i>Наклонный шрифт</i>\n<u>Подчеркнутый шрифт</u>");
      telegramSendMessage_Post_Http(tgChatId, (char*)"Версия telegramSendMessage_Post_Http\n<b>Жирный шрифт</b>\n<i>Наклонный шрифт</i>\n<u>Подчеркнутый шрифт</u>");
>>>>>>> 647be8bd4ab17f363c98bf25f62edd64794181e8
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

