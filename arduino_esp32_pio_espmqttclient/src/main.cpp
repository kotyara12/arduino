#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include "esp_sntp.h"
#include "mqtt_client.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_http_client.h"


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

#define CONFIG_MQTT_USE_TLS 1                       // Будем ли мы использовать mbedTLS для MQTT-подключения

// -------------------------------------------------------
// Топики MQTT 
// -------------------------------------------------------

const char* mqtt_topic_status   = "dzen/esp32_arduino/status";
const char* mqtt_topic_config   = "dzen/esp32_arduino/config";

// -------------------------------------------------------
// Сертификаты
// -------------------------------------------------------

#if CONFIG_MQTT_USE_TLS

// Корневой сертификат ISRG Root x1, действителен до 4 июня 2035 года
static const char cert_ISRG_Root_x1[] = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

#endif // CONFIG_MQTT_USE_SERVER_CERT

// Корневой сертификат для Telegram API, действителен до 29 июня 2034
static const char cert_TG_API_Root[] = \
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

esp_mqtt_client_handle_t mqtt_client = NULL;       // Указатель на MQTT-клиента

int config_value = 0;                              // Некая переменная для настройки извне

nvs_handle_t config_handle;                        // Указатель на NVS-пространство имен

// -------------------------------------------------------
// Инициализация NVS раздела
// -------------------------------------------------------

void nvsInit()
{
  esp_err_t err = nvs_flash_init();
  if ((err == ESP_ERR_NVS_NO_FREE_PAGES) || (err == ESP_ERR_NVS_NEW_VERSION_FOUND)) {
    Serial.println("Erasing NVS partition...");
    nvs_flash_erase();
    err = nvs_flash_init();
  };
  if (err == ESP_OK) {
    Serial.println("NVS partition initilized");
  } else {
    Serial.print("NVS partition initialization error: ");
    Serial.print(err);
    Serial.print(", ");
    Serial.println(esp_err_to_name(err));
  };
}

bool nvsOpen(const char* nspace)
{
  // Открываем нужное пространство имен
  esp_err_t err = nvs_open(nspace, NVS_READWRITE, &config_handle); 
  if (err != ESP_OK) {
    Serial.print("Error opening NVS namespace \"");
    Serial.print(nspace);
    Serial.print("\": ");
    Serial.print(err);
    Serial.print(", ");
    Serial.println(esp_err_to_name(err));
    return false;
  };
  return true;
}

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
// Подключение к MQTT брокеру :: версия для ESP MQTT Client
// -------------------------------------------------------

// Обработчик событий MQTT клиента
void cbMqttEvent(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  // Извлекаем нужные данные
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  esp_mqtt_client_handle_t client = event->client;

  // В зависимости от идентификатора события действуем по разному...
  switch ((esp_mqtt_event_id_t)event_id) {
    // Подключились к серверу - публикуем некие данные и подписываемся
    case MQTT_EVENT_CONNECTED:
        Serial.println("Successfully connected to MQTT server");
        // Публикуем состояние устройства
        esp_mqtt_client_publish(client, mqtt_topic_status, "online", 0, 1, 0);
        // Подписываемся на топик параметра
        esp_mqtt_client_subscribe(client, mqtt_topic_config, 0);
        break;
    // Отключились от сервера
    case MQTT_EVENT_DISCONNECTED:
        Serial.println("Connection to MQTT server lost");
        // Отписываемся от топика
        esp_mqtt_client_unsubscribe(client, mqtt_topic_config);
        break;
    // Подписка выполнена
    case MQTT_EVENT_SUBSCRIBED:
        Serial.println("Subscription successfully completed");
        break;
    // Сообщение опубликовано
    case MQTT_EVENT_PUBLISHED:
        Serial.print("Message sent successfully, id: ");
        Serial.println(event->msg_id);
        break;
    // Поступил блок данных
    case MQTT_EVENT_DATA:
        Serial.print("Incoming message received: topic=");
        Serial.write(event->topic, event->topic_len);
        Serial.print(", data=");
        Serial.write(event->data, event->data_len);
        Serial.println();
        // Сравниваем топики 
        if (strncasecmp(mqtt_topic_config, (const char*)event->topic, event->topic_len) == 0) {
          // Сохраняем полученное значение в переменной
          config_value = atoi(event->data);
          Serial.print("New value for variable config_value=");
          Serial.print(config_value);
          Serial.println(" received");
          // Записываем новое значение в память
          nvs_set_i32(config_handle, "config", config_value);
          // Сохраняем изменения
          nvs_commit(config_handle);
        };
        break;
    // Ошибка
    case MQTT_EVENT_ERROR:
        Serial.print("MQTT error");
        break;
    // Все остальное
    default:
        break;
  }
}

// Запуск MQTT клиента
bool mqttStart()
{
  Serial.print("Start MQTT client: ");
  if (mqtt_client == NULL) {
    // Инициализация ESP-IDF netif
    esp_netif_init();

    // Структура настройки - заполним нулями
    esp_mqtt_client_config_t mqttCfg = {0};
    
    // Настраиваем MQTT сервер
    mqttCfg.host = mqtt_server;
    #if CONFIG_MQTT_USE_TLS
      // Безопасное подключение
      mqttCfg.transport = MQTT_TRANSPORT_OVER_SSL;
      mqttCfg.port = mqtt_port_tls;
      // Прикрепляем корневой сертификат сервера
      mqttCfg.cert_pem = (const char*)cert_ISRG_Root_x1;
      mqttCfg.cert_len = strlen(cert_ISRG_Root_x1)+1;
    #else
      // Обычное подключение
      mqttCfg.transport = MQTT_TRANSPORT_OVER_TCP;
      mqttCfg.port = mqtt_port;
    #endif // CONFIG_MQTT_USE_TLS

    // Автоматическое переподключение к серверу
    mqttCfg.disable_auto_reconnect = false;

    // Логин / пароль
    mqttCfg.username = mqtt_user;
    mqttCfg.password = mqtt_pass;
    // Для автогенерации ESP32_%CHIPID% укажите NULL
    mqttCfg.client_id = NULL; 
    
    // Параметры сеанса
    mqttCfg.disable_clean_session = false;
    mqttCfg.keepalive = 60;
    
    // Завещание
    mqttCfg.lwt_topic = mqtt_topic_status;
    mqttCfg.lwt_msg = "offline";
    mqttCfg.lwt_msg_len = strlen(mqttCfg.lwt_msg);
    mqttCfg.lwt_qos = 1;
    mqttCfg.lwt_retain = false;

    // Создаем задачу MQTT клиента
    mqtt_client = esp_mqtt_client_init(&mqttCfg);
    // Регистрируем обработчик событий
    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, cbMqttEvent, NULL);
  };
  
  // Запускаем задачу MQTT клиента
  esp_err_t err = esp_mqtt_client_start(mqtt_client);
  // Проверяем результаты работы
  if (err == ESP_OK) {
    Serial.println("ok");
  } else {
    Serial.print("failed, err=");
    Serial.println(err);
  };
  return err == ESP_OK;
}

// Останавливаем mqtt клиент
void mqttStop()
{
  if (mqtt_client) {
    esp_mqtt_client_stop(mqtt_client);
  };
}

// -------------------------------------------------------
// Подключение к WiFi
// -------------------------------------------------------

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
      // Запускаем MQTT-клиента
      mqttStart();
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      mqttStop();
      break;
    default:
      break;
  }
}

// -------------------------------------------------------
// Отправка данных на народный мониторинг
// -------------------------------------------------------

void sendNaronmonData(const float value1, const float value2, const float value3)
{
  Serial.print("Send data to narodmon.ru: ");
  
  // Формируем динамическую URI
  String sURI = "https://narodmon.ru/get?ID=00FF00FF00FF00FF&sensor1=" + String(value1) + "&sensor2=" + String(value2) + "&sensor3=" + String(value3);
  
  // Настраиваем HTTP подключение
  esp_http_client_config_t cfgHttp = {0};
  cfgHttp.method = HTTP_METHOD_GET;
  cfgHttp.url = sURI.c_str();
  cfgHttp.transport_type = HTTP_TRANSPORT_OVER_SSL;
  cfgHttp.cert_pem = cert_ISRG_Root_x1;

  // Создаем http_client
  esp_http_client_handle_t client = esp_http_client_init(&cfgHttp);
  if (client) {
    // Запускаем на выполнение
    esp_err_t ret = esp_http_client_perform(client);
    if (ret == ESP_OK) {
      // Запрашиваем код возврата сервера
      int retCode = esp_http_client_get_status_code(client);
      if (retCode == HttpStatus_Ok) {
        Serial.println("ok");
      } else {
        Serial.printf("failed to send data, error code: #%d!\n", retCode);
      };
    } else {
      Serial.printf("failed to complete http request, error code: 0x%x!\n", ret);
    };
    // Не забываем удалить клиента после использования
    esp_http_client_cleanup(client);
  };
}

// -------------------------------------------------------
// Отправка сообщения в telegram
// -------------------------------------------------------

const char* tgToken = "token";    // Укажите здесь токен вашего бота
const char* chatId  = "chat_id";  // Укажите здесь номер чата получателя

void sendTelegramMessage(char* message)
{
  Serial.print("Send message to telegram: ");
  Serial.print(message);
  Serial.print(": ");

  // Формируем динамическую URI
  String sURI = "https://api.telegram.org/bot" + String(tgToken) + "/sendMessage";
  // Формируем JSON-пакет с данными
  String sJSON = "{\"chat_id\":" + String(chatId) + ",\"parse_mode\":\"HTML\",\"disable_notification\":false,\"text\":\"" + String(message) + "\"}";

  // Настраиваем HTTP подключение
  esp_http_client_config_t cfgHttp = {0};
  cfgHttp.method = HTTP_METHOD_POST;
  cfgHttp.url = sURI.c_str();
  // Настраиваем TLS через cert_TG_API_Root
  cfgHttp.transport_type = HTTP_TRANSPORT_OVER_SSL;
  cfgHttp.cert_pem = cert_TG_API_Root;

  // Создаем http_client
  esp_http_client_handle_t client = esp_http_client_init(&cfgHttp);
  if (client) {
    // Указываем тип POST-данных
    esp_http_client_set_header(client, "Content-Type", "application/json");
    // Добавляем JSON в тело запроса
    esp_http_client_set_post_field(client, sJSON.c_str(), sJSON.length());
    // Запускаем на выполнение
    esp_err_t ret = esp_http_client_perform(client);
    if (ret == ESP_OK) {
      // Запрашиваем код возврата API
      int retCode = esp_http_client_get_status_code(client);
      if (retCode == HttpStatus_Ok) {
        // Все хорошо, сообщение отправлено
        Serial.println("ok");
      } else if (retCode == HttpStatus_Forbidden) {
        // Слишком много запросов, нужно подождать
        Serial.println("failed to send message, too many messages, please wait");
      } else {
        // Ошибка
        Serial.printf("failed to send message, API error code: #%d!\n", retCode);
      };
    } else {
      Serial.printf("failed to complete request to Telegram API, error code: 0x%x!\n", ret);
    };
    // Не забываем удалить клиента после использования
    esp_http_client_cleanup(client);
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

  // Инициализация NVS
  nvsInit();
  if (nvsOpen("settings")) {
    nvs_get_i32(config_handle, "config", &config_value);
    Serial.print("Read config_value from NVS: ");
    Serial.println(config_value);
  };

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
  // Делаем какую-то основную полезную работу, не связанную с WiFi
  // sendTelegramMessage("Привет мир");
  
  // Ожидание
  delay(10000);
}
