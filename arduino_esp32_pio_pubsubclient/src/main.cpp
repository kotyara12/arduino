#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include "esp_sntp.h"
#include "PubSubClient.h"
#include <Preferences.h>

// -------------------------------------------------------
// Параметры подключения к WiFi
// -------------------------------------------------------

const char* wifi_ssid           = "uuuuuu";         // Замените на имя вашей WiFi сети
const char* wifi_password       = "11111111";       // Замените на пароль вашей WiFi сети

const char* timezone            = "MSK-3";          // Часовой пояс
const char* sntp_server1        = "pool.ntp.org";   // Сервер синхронизации времени 1
const char* sntp_server2        = "time.nist.gov";  // Сервер синхронизации времени 2

// -------------------------------------------------------
// Параметры подключения к MQTT брокеру 
// -------------------------------------------------------

const char* mqtt_server         = "xx.wqtt.ru";     // Адрес MQTT сервера
const int   mqtt_port           = 8888;             // Номер порта без шифрования
const int   mqtt_port_tls       = 9999;             // Номер порта с шифрованием
const char* mqtt_client_id      = "esp32_demo";     // Использовать статический mqtt_clientId оптимальнее с точки зрения фрагментации кучи, только не забывайте изменять его на разных устройствах
const char* mqtt_user           = "ssssss";         // Имя пользователя
const char* mqtt_pass           = "22222222";       // Пароль

#define CONFIG_MQTT_USE_TLS 1                      // Будем ли мы использовать mbedTLS для MQTT-подключения
#define CONFIG_MQTT_USE_SERVER_CERT 1              // Будем ли мы проверять сертификат сервера

// -------------------------------------------------------
// Топики MQTT 
// -------------------------------------------------------

const char* mqtt_topic_status   = "dzen/esp32_arduino/status";
const char* mqtt_topic_config   = "dzen/esp32_arduino/config";

// -------------------------------------------------------
// Сертификаты
// -------------------------------------------------------

#if CONFIG_MQTT_USE_TLS && CONFIG_MQTT_USE_SERVER_CERT

// Корневой сертификат ISRG Root x1, действителен до 4 июня 2035 года
const char* cert_ISRG_Root_x1 = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

#endif // CONFIG_MQTT_USE_SERVER_CERT

// Корневой сертификат для Telegram API, действителен до 29 июня 2034
const char* cert_TG_API_Root = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIEADCCAuigAwIBAgIBADANBgkqhkiG9w0BAQUFADBjMQswCQYDVQQGEwJVUzEh\n" \
  "MB8GA1UEChMYVGhlIEdvIERhZGR5IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBE\n" \
  "YWRkeSBDbGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTA0MDYyOTE3\n" \
  "MDYyMFoXDTM0MDYyOTE3MDYyMFowYzELMAkGA1UEBhMCVVMxITAfBgNVBAoTGFRo\n" \
  "ZSBHbyBEYWRkeSBHcm91cCwgSW5jLjExMC8GA1UECxMoR28gRGFkZHkgQ2xhc3Mg\n" \
  "MiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTCCASAwDQYJKoZIhvcNAQEBBQADggEN\n" \
  "ADCCAQgCggEBAN6d1+pXGEmhW+vXX0iG6r7d/+TvZxz0ZWizV3GgXne77ZtJ6XCA\n" \
  "PVYYYwhv2vLM0D9/AlQiVBDYsoHUwHU9S3/Hd8M+eKsaA7Ugay9qK7HFiH7Eux6w\n" \
  "wdhFJ2+qN1j3hybX2C32qRe3H3I2TqYXP2WYktsqbl2i/ojgC95/5Y0V4evLOtXi\n" \
  "EqITLdiOr18SPaAIBQi2XKVlOARFmR6jYGB0xUGlcmIbYsUfb18aQr4CUWWoriMY\n" \
  "avx4A6lNf4DD+qta/KFApMoZFv6yyO9ecw3ud72a9nmYvLEHZ6IVDd2gWMZEewo+\n" \
  "YihfukEHU1jPEX44dMX4/7VpkI+EdOqXG68CAQOjgcAwgb0wHQYDVR0OBBYEFNLE\n" \
  "sNKR1EwRcbNhyz2h/t2oatTjMIGNBgNVHSMEgYUwgYKAFNLEsNKR1EwRcbNhyz2h\n" \
  "/t2oatTjoWekZTBjMQswCQYDVQQGEwJVUzEhMB8GA1UEChMYVGhlIEdvIERhZGR5\n" \
  "IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBEYWRkeSBDbGFzcyAyIENlcnRpZmlj\n" \
  "YXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQAD\n" \
  "ggEBADJL87LKPpH8EsahB4yOd6AzBhRckB4Y9wimPQoZ+YeAEW5p5JYXMP80kWNy\n" \
  "OO7MHAGjHZQopDH2esRU1/blMVgDoszOYtuURXO1v0XJJLXVggKtI3lpjbi2Tc7P\n" \
  "TMozI+gciKqdi0FuFskg5YmezTvacPd+mSYgFFQlq25zheabIZ0KbIIOqPjCDPoQ\n" \
  "HmyW74cNxA9hi63ugyuV+I6ShHI56yDqg+2DzZduCLzrTia2cyvk0/ZM/iZx4mER\n" \
  "dEr/VxqHD3VILs9RaRegAhJhldXRQLIQTO7ErBBDpqWeCtWVYpoNz4iCxTIM5Cuf\n" \
  "ReYNnyicsbkqWletNw+vHX/bvZ8=\n" \
  "-----END CERTIFICATE-----\n";

// -------------------------------------------------------
// Глобальные переменные
// -------------------------------------------------------

#if CONFIG_MQTT_USE_TLS
  WiFiClientSecure wifi_client;                    // Указатель на WiFi-клиента c поддержкой mbedTLS
#else
  WiFiClient wifi_client;                          // Указатель на WiFi-клиента без TLS
#endif // CONFIG_MQTT_USE_TLS

PubSubClient mqtt_client(wifi_client);             // Указатель на MQTT-клиента

int config_value = 0;                              // Некая переменная для настройки извне

Preferences app_settings;                          // Параметры устройства в NVS-разделе

// -------------------------------------------------------
// SNTP-синхронизация
// -------------------------------------------------------

// Обработчик синхронизации времени
void cbSntpSync(struct timeval *tv)
{
  struct tm timeinfo;
  localtime_r(&tv->tv_sec, &timeinfo);
  if (timeinfo.tm_year < (1970 - 1900)) {
    Serial.println("Time synchronization failed!");
  } else {
    Serial.print("Time synchronization completed, current time: ");
    Serial.print(asctime(&timeinfo));
  };
}

// Настройка и запуск синхронизации времени
void sntpSyncStart()
{
  // Инициализация ESP-IDF netif
  esp_netif_init();
  // Останавливаем SNTP, если она была запущена
  if(sntp_enabled()){
      sntp_stop();
  };
  // Настройка SNTP-синхронизации
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, (char*)sntp_server1);
  sntp_setservername(1, (char*)sntp_server2);
  sntp_set_time_sync_notification_cb(cbSntpSync);
  sntp_init();
  Serial.println("Time synchronization start...");
  // Устанавливаем часовой пояс системы
  setenv("TZ", timezone, 1);
  tzset();
}

// -------------------------------------------------------
// Подключение к MQTT брокеру :: версия для PubSubClient от knolleary
// -------------------------------------------------------

// Функция обратного вызова при поступлении входящего сообщения от брокера
void mqttOnIncomingMessage(char* topic, uint8_t* payload, unsigned int length)
{
  // Преобразуем поступившие данные в String
  String _topic = topic;
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

  // Сравниваем топик с эталонным значением
  if (_topic.equalsIgnoreCase(mqtt_topic_config)) {
    // Сохраняем полученное значение в переменной
    config_value = _payload.toInt();
    app_settings.putInt("config", config_value);
    Serial.print("New value for variable config_value=");
    Serial.print(config_value);
    Serial.print(" received and saved");
  };
}

// Подключение к MQTT брокеру
bool mqttConnect()
{
  if (!mqtt_client.connected()) {
    Serial.print("Connecting to MQTT broker: ");
    // Настраиваем MQTT клиент
    #if CONFIG_MQTT_USE_TLS
      // Безопасное подключение
      mqtt_client.setServer(mqtt_server, mqtt_port_tls);

      #if CONFIG_MQTT_USE_SERVER_CERT
        // Прикрепляем корневой сертификат сервера - безопасно, но нужно следить за сроками действия
        wifi_client.setCACert(cert_ISRG_Root_x1);
      #else
        // Не проверять сертификат сервера - не безопасно, но просто и не нужно заботиться о сроках действия сертификатов
        wifi_client.setInsecure(); 
      #endif // CONFIG_MQTT_USE_SERVER_CERT
    #else
      // Обычное подключение
      mqtt_client.setServer(mqtt_server, mqtt_port);
    #endif // CONFIG_MQTT_USE_TLS

    // Пробуем подключиться и публикуем LWT сообщение "offline"
    if (mqtt_client.connect(mqtt_client_id, mqtt_user, mqtt_pass, mqtt_topic_status, 1, false, "offline")) {
      Serial.println("ok");

      // Устанавливаем сallback и подписываемся на топик некоего параметра
      mqtt_client.setCallback(mqttOnIncomingMessage);
      mqtt_client.subscribe(mqtt_topic_config);

      // Публикуем статус устройства
      String payload = "online";
      mqtt_client.publish(mqtt_topic_status, payload.c_str(), false);
    } else {
      Serial.print("failed, error code: ");
      Serial.print(mqtt_client.state());
      Serial.println("!");
    };
    return mqtt_client.connected();
  };
  return true;
}

void mqttDisconnect()
{
  if (mqtt_client.connected()) {
    mqtt_client.disconnect();
  };
}

void mqttExec()
{
  if (WiFi.status() == WL_CONNECTED) {
    // Есть подключение к WiFi
    if (mqtt_client.connected()) {
      // Обрабатываем внутренние операции MQTT клиента
      mqtt_client.loop();
    } else {
      // Проверяем время
      if (time(NULL) > 1000000000) {
        // Время успешно синхронизировано, можно подключаться
        mqttConnect();
      };
    };
  } else {
    // Нет подключения к WiFi
    mqttDisconnect();
  };
}

// -------------------------------------------------------
// Подключение к WiFi
// -------------------------------------------------------

/* 2.3.1. Отслеживаем состояние WiFi в рабочем цикле
wl_status_t wifi_status_prev   = WL_IDLE_STATUS;   // Здесь мы будем хранить предыдущее состояние подключения
wl_status_t wifi_status_now    = WL_IDLE_STATUS;   // Здесь мы будем хранить текущее состояние подключения

// Проверка состояния WiFi подключения в рабочем цикле
bool wifiCheck()
{
  // Считываем состояние подключения
  wifi_status_now = WiFi.status();
  // Если состояние изменилось...
  if (wifi_status_prev != wifi_status_now) {
    // Выводим сообщение
    switch (wifi_status_now) {
      case WL_NO_SSID_AVAIL:
        Serial.println("WiFi SSID is not available"); 
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("WiFi scan completed"); 
        break;
      case WL_CONNECTED:
        Serial.print("WiFi connected, IP address: ");
        Serial.println(WiFi.localIP());
        // Запускаем синхронизацию времени
        sntpSyncStart();
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Failed to connect to access point"); 
        break;
      case WL_CONNECTION_LOST:
        Serial.println("Lost connection to WiFi access point"); 
        if (wifi_status_prev == WL_CONNECTED) {
          // Закрываем соединение с MQTT-брокером
          mqttDisconnect();
        };
        break;
      case WL_DISCONNECTED:
        Serial.println("Disconnected from WiFi access point"); 
        if (wifi_status_prev == WL_CONNECTED) {
          // Закрываем соединение с MQTT-брокером
          mqttDisconnect();
        };
        break;
      default:
        break;
    };
    // Сохраняем новое значение
    wifi_status_prev = wifi_status_now;
  };
  // Возвращаем true, если соединение есть...
  return wifi_status_now == WL_CONNECTED;
}
*/

// Этот обработчик вызывается при различных WiFi-событиях
void cbWiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("WiFi connected, IP address: ");
      Serial.println(WiFi.localIP());
      // Запускаем синхронизацию времени
      sntpSyncStart();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      break;
    default:
      break;
  }
}

// -------------------------------------------------------
// Отправка данных на народный мониторинг
// -------------------------------------------------------

/*********************************************************
// 5.1.2 Отправка GET-запроса с использованием WiFiClient
const char* requestNarodmon = "GET /get?ID=00FF00FF00FF00FF&sensor1=%f&sensor2=%f&sensor3=%f HTTP/1.1\r\n"
                              "Host: narodmon.ru\r\n"
                              "User-Agent: ESP32\r\n"
                              "Connection: close\r\n\r\n";

void sendNaronmonData(const float value1, const float value2, const float value3)
{
  WiFiClientSecure wifi_client;
  Serial.print("Send data to narodmon.ru: ");
  // Не проверять сертификат сервера - не безопасно, но просто и не нужно заботиться о сроках действия сертификатов
  // wifi_client.setInsecure(); 
  // Или прикрепляем корневой сертификат сервера - безопасно, но нужно следить за сроками действия
  wifi_client.setCACert(cert_ISRG_Root_x1);
  if (wifi_client.connect("narodmon.ru", 443)) {
    // Формируем и отправляем GET-запрос для отправки данных
    wifi_client.printf(requestNarodmon, value1, value2, value3);
    // Читаем ответ сервера (только первую строку)
    Serial.print("responce code: ");
    Serial.println(wifi_client.readStringUntil('\n'));
    // Закрываем соединение
    wifi_client.stop();
  } else {
    Serial.println("failed to connect");
  };
}
*********************************************************/  

// 5.2.1 Отправка GET-запроса с использованием HTTPClient
void sendNaronmonData(const float value1, const float value2, const float value3)
{
  Serial.print("Send data to narodmon.ru: ");
  // Объявляем переменную HTTP Client
  HTTPClient http_tg;
  // Формируем динамическую URI
  String sURI = "https://narodmon.ru/get?ID=00FF00FF00FF00FF&sensor1=" + String(value1) + "&sensor2=" + String(value2) + "&sensor3=" + String(value3);
  // Пробуем подключиться к серверу с указанным сертификатом
  if (http_tg.begin(sURI.c_str(), cert_ISRG_Root_x1)) {
    // Отправляем GET-запрос
    int httpCode = http_tg.GET();
    // Выводим в лог код ответа сервера
    Serial.printf("Responce code: %d \"%s\"\r\n", httpCode, http_tg.errorToString(httpCode));
    // Закрываем соединение
    http_tg.end();
  } else {
    Serial.println("Failed to connect to narodmon.ru");
  };
}

// -------------------------------------------------------
// Отправка сообщения в Telegram
// -------------------------------------------------------

const char* tgToken = "токен вашего бота";    // Укажите здесь токен вашего бота
const char* chatId  = "идентификатор чата";   // Укажите здесь номер чата получателя

/*********************************************************
// 5.1.3 Отправка POST-запроса с использованием WiFiClient
const char* resuestTgMessage = "POST https://api.telegram.org/bot%s/sendMessage HTTP/1.1\r\n"
                               "Host: api.telegram.org\r\n"
                               "User-Agent: ESP8266\r\n"
                               "Content-Type: application/json\r\n"
                               "Content-Length: %d\r\n\r\n"
                               "%s\r\n\r\n";

void sendTelegramMessage(const char* message)
{
  WiFiClientSecure wifi_client;
  Serial.print("Send message to telegram: ");
  Serial.print(message);
  Serial.print(": ");
  // Формируем JSON-пакет с данными
  String sJSON = "{\"chat_id\":" + String(chatId) + ",\"parse_mode\":\"HTML\",\"disable_notification\":false,\"text\":\"" + String(message) + "\"}";
  // Не проверять сертификат сервера - не безопасно, но просто и не нужно заботиться о сроках действия сертификатов
  // wifi_client.setInsecure(); 
  // Или прикрепляем корневой сертификат сервера - безопасно, но нужно следить за сроками действия
  wifi_client.setCACert(cert_TG_API_Root);
  if (wifi_client.connect("api.telegram.org", 443)) {
    // Формируем и отправляем POST-запрос
    wifi_client.printf(resuestTgMessage, tgToken, sJSON.length(), sJSON.c_str());
    // Читаем ответ сервера (только первую строку)
    Serial.print("responce code: ");
    Serial.println(wifi_client.readStringUntil('\n'));
    // Закрываем соединение
    wifi_client.stop();
  } else {
    Serial.println("failed to connect");
  };
}
*********************************************************/

// 5.2.2 Отправка POST-запроса с использованием HTTPClient
void sendTelegramMessage(char* message)
{
  Serial.print("Send message to telegram: ");
  Serial.print(message);
  Serial.print(": ");
  // Формируем динамическую URI
  String sURI = "https://api.telegram.org/bot" + String(tgToken) + "/sendMessage";
  // Формируем JSON-пакет с данными
  String sJSON = "{\"chat_id\":" + String(chatId) + ",\"parse_mode\":\"HTML\",\"disable_notification\":false,\"text\":\"" + String(message) + "\"}";
  Serial.println(sJSON.c_str());
  // Объявляем переменную HTTP Client
  HTTPClient http_tg;
  // Пробуем подключиться к Telegram API по сгенерированному URI и с указанным сертификатом
  if (http_tg.begin(sURI.c_str(), cert_TG_API_Root)) {
    // Добавляем заголовок с типом данных
    http_tg.addHeader("Content-Type", "application/json");
    // Формируем и отправляем POST-запрос
    int httpCode = http_tg.POST(sJSON);
    // Выводим в лог код ответа сервера
    Serial.printf("API responce code: %d \"%s\"\r\n", httpCode, http_tg.errorToString(httpCode));
    // Закрываем соединение
    http_tg.end();
  } else {
    Serial.println("Failed to connect to telegram API");
  };
}

// -------------------------------------------------------
// Основные функции скетча
// -------------------------------------------------------

// Функция настройки, выполняется только один раз после запуска микроконтроллера
void setup() {
  // Настраиваем UART0-порт на скорость 115200
  Serial.begin(115200);
  Serial.println();

  // Открываем пространство имен
  app_settings.begin("settings", false); 
  // Считываем последнее сохраненное значение
  config_value = app_settings.getInt("config", config_value);
  Serial.print("Read config_value from NVS: ");
  Serial.println(config_value);

  // Выводим сообщение о том, что мы собираемся подключиться к сети 
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  // Регистрируем обработчик событий WiFi
  WiFi.onEvent(cbWiFiEvent);

  // Настраиваем новое подключение
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
  WiFi.setHostname("esp32_test");

  // Запускаем процесс подключения
  WiFi.begin(wifi_ssid, wifi_password);

  // И без какого-либо ожидания идем дальше 
  Serial.println("Waiting for WiFi..."); 
}

// Рабочий цикл, повторяется непрерывно
void loop() {
  // Проверка состояния подключения только для варианта 2.3.1
  // wifiCheck();

  // Рабочий цикл PubSubClient
  mqttExec();

  // Делаем какую-то основную полезную работу, не связанную с WiFi
  // sendTelegramMessage("Привет мир");
  
  // Ожидание
  delay(10000);
}
