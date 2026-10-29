#include <WiFi.h>
#include <PubSubClient.h>

// ====== Konfigurasi WiFi ======
const char* ssid     = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

// ====== Konfigurasi MQTT ======
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;
const char* mqtt_topic  = "/data/sensor1";

WiFiClient espClient;
PubSubClient client(espClient);

String clientId;   // Client ID dibuat dari MAC address

// ====== Koneksi WiFi ======
void setup_wifi() {
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi terhubung!");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}

// ====== Callback saat pesan diterima ======
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("==============================");
  Serial.print("Pesan diterima dari topic: ");
  Serial.println(topic);

  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Isi pesan : ");
  Serial.println(pesan);
  Serial.println("==============================");
}

// ====== Reconnect ke broker MQTT ======
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT broker sebagai ");
    Serial.print(clientId);
    Serial.print("...");

    if (client.connect(clientId.c_str())) {
      Serial.println(" terhubung!");
      client.subscribe(mqtt_topic);
      Serial.print("Subscribe ke topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print(" gagal, rc=");
      Serial.print(client.state());
      Serial.println(" -> coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();

  // Buat Client ID dari MAC address (tanpa tanda ":")
  clientId = "ESP32Client-" + WiFi.macAddress();
  clientId.replace(":", "");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();   // Wajib dipanggil agar pesan masuk bisa diproses
}
