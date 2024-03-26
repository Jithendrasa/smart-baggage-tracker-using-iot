#include <SoftwareSerial.h>

// GSM module configuration
#define GSM_RX_PIN 2
#define GSM_TX_PIN 3
#define BAUDRATE 9600
SoftwareSerial gsmSerial(GSM_RX_PIN, GSM_TX_PIN);

// GPS module configuration
#define GPS_RX_PIN 4
#define GPS_TX_PIN 5
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

// Latitude and Longitude threshold for triggering alert
#define LAT_THRESHOLD 30.0  // Example latitude threshold
#define LON_THRESHOLD -80.0 // Example longitude threshold

// User's phone number
#define USER_NUMBER "+1234567890"

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(BAUDRATE);
  gpsSerial.begin(BAUDRATE);
}

void sendSMS(const char* destinationNumber, const char* message) {
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(destinationNumber);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.print(message);
  gsmSerial.write(26); // ASCII code for Ctrl+Z
  delay(1000);
}

void loop() {
  // Read GPS coordinates
  gpsSerial.println("AT+CGPSINF=0");
  delay(1000);
  if (gpsSerial.available() > 0) {
    String response = gpsSerial.readStringUntil('\n');
    if (response.indexOf("CGPSINF") != -1) {
      char *latPtr = strtok((char *)response.c_str(), ",");
      latPtr = strtok(NULL, ",");
      char *lngPtr = strtok(NULL, ",");
      float latitude = atof(latPtr);
      float longitude = atof(lngPtr);

      // Check if luggage is beyond the predefined area
      if (latitude > LAT_THRESHOLD || longitude > LON_THRESHOLD) {
        char message[100];
        snprintf(message, sizeof(message), "Your luggage has moved beyond the designated area. Current coordinates: Latitude %.6f, Longitude %.6f", latitude, longitude);
        sendSMS(USER_NUMBER, message);
      }
    }
  }
  delay(60000); // Check coordinates every 60 seconds
}
