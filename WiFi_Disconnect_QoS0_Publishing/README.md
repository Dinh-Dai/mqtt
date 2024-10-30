# Thí nghiệm MQTT với ESP32

## Mục tiêu
Lặp lại thí nghiệm mất kết nối WiFi (Thí nghiệm 2) sử dụng HiveMQ Broker với kết nối TLS.

## Thiết lập môi trường

### Bước 1: Tạo tài khoản trên HiveMQ
1. Truy cập [hivemq.com](https://www.hivemq.com/).
2. Đăng ký tài khoản miễn phí.

### Bước 2: Tạo Broker miễn phí
1. Đăng nhập vào tài khoản HiveMQ.
2. Tạo một broker mới với thông tin:
   - **URL**: `a13dcfcaabef4c32acf611db7d43e897.s1.eu.hivemq.cloud`
   - **Port**: `8883`
3. Thiết lập bảo mật bằng cách thêm username và password:
   - **Username**: `nakare`
   - **Password**: `12345678`

### Bước 3: Thiết lập kết nối mã hóa TLS
1. Tải chứng chỉ CA từ [Let's Encrypt](https://letsencrypt.org/certs/isrgrootx1.pem) và lưu vào dự án với tên `ca_cert.h`.
2. Cập nhật file `main.cpp` như sau:

```cpp
const char *mqttBroker = "a13dcfcaabef4c32acf611db7d43e897.s1.eu.hivemq.cloud";
const int mqttPort = 8883;

void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        Serial.println("Attempting MQTT connection...");
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        
        // Kết nối với broker bằng username và password
        if (mqttClient.connect(client_id.c_str(), "nakare", "12345678")) 
        {
            Serial.print(client_id);
            Serial.println(" connected");
            mqttClient.subscribe(echo_topic);
        }
        else
        {
            Serial.print("MQTT connect failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 1 seconds");
            delay(1000);
        }
    }
}
