#define BLYNK_TEMPLATE_ID "TMPL662ZdQwnE"
#define BLYNK_TEMPLATE_NAME "Sensor Suhu Infrared MLX90614"
#define BLYNK_AUTH_TOKEN "nxDe-pc31Oab438U-IV-mkAb1Z72xkZj"
#define BLYNK_PRINT Serial
#define BUZZER_PIN D5       // Connect buzzer to D5

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const float THRESHOLD = 80.0;  // Trigger buzzer above this temperature

// Sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// WiFi dan Token
char ssid[] = "numpang lewat _plus";
char pass[] = "passwordnya";
char auth[] = BLYNK_AUTH_TOKEN;

// Suhu mentah & kalibrasi
double suhu_amb_raw, suhu_obj_raw;
double suhu_amb_kalibrasi, suhu_obj_kalibrasi;

// Koefisien kalibrasi
const float a = 1.02;
const float b = 0.3;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // make sure buzzer is off initially

  if (!mlx.begin()) {
    Serial.println("Sensor gagal diinisialisasi. Cek koneksi.");
    while (1);
  }

  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Tersambung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println("Terhubung ke Blynk.");
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);  

  Blynk.run();

  // Ambil data suhu mentah
  suhu_amb_raw = mlx.readAmbientTempC();
  suhu_obj_raw = mlx.readObjectTempC();

  // Kalibrasi suhu
  suhu_amb_kalibrasi = a * suhu_amb_raw + b;
  suhu_obj_kalibrasi = a * suhu_obj_raw + b;

    // 🔊 Passive Buzzer Logic
  if (suhu_obj_kalibrasi > THRESHOLD) {
    tone(BUZZER_PIN, 2000);  // 2kHz sound
  } else {
    noTone(BUZZER_PIN);
  }

  // Validasi (opsional, non-blocking)
  if (suhu_amb_raw < -40 || suhu_amb_raw > 125) {
    Serial.println("❌ Suhu tidak valid.");
    return;
  }

  // Serial Monitor (Raw dan Kalibrasi)
  Serial.print("Ambient raw: ");
  Serial.print(suhu_amb_raw);
  Serial.print(" | kalibrasi: ");
  Serial.print(suhu_amb_kalibrasi);
  Serial.print("  ||  Object raw: ");
  Serial.print(suhu_obj_raw);
  Serial.print(" | kalibrasi: ");
  Serial.println(suhu_obj_kalibrasi);

  // Kirim ke Blynk
  Blynk.virtualWrite(V1, suhu_amb_raw);
  Blynk.virtualWrite(V2, suhu_obj_raw);
  Blynk.virtualWrite(V3, suhu_amb_kalibrasi);
  Blynk.virtualWrite(V4, suhu_obj_kalibrasi);

  // 🔊 Passive Buzzer Logic
  if (suhu_obj_kalibrasi > THRESHOLD) {
    tone(BUZZER_PIN, 2000);  // Buzzer ON
  } else {
    noTone(BUZZER_PIN);      // Buzzer OFF
  }

  delay(10000);

}
