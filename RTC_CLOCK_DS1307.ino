#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

// Define I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the I2C address and LCD size as needed

// Define RTC module
RTC_DS3231 rtc;

// Define pins for buttons
const int setHourPin = 2;
const int setMinutePin = 3;
const int increasePin = 4;
const int decreasePin = 5;

// Define buzzer pin
const int buzzerPin = 8;

// Variables to store current time and alarm time
int setHour = 0;
int setMinute = 0;
int alarmHour = 8;  // Default bell time set to 8:00 AM
int alarmMinute = 0;

// State variables
bool settingTime = false;

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("School Bell");

  // Initialize the RTC
  if (!rtc.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("RTC not found");
    while (1);
  }
  if (rtc.lostPower()) {
    lcd.setCursor(0, 1);
    lcd.print("RTC lost power");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set to compile time
  }

  // Initialize buttons
  pinMode(setHourPin, INPUT_PULLUP);
  pinMode(setMinutePin, INPUT_PULLUP);
  pinMode(increasePin, INPUT_PULLUP);
  pinMode(decreasePin, INPUT_PULLUP);

  // Initialize buzzer
  pinMode(buzzerPin, OUTPUT);

  // Print initial status
  delay(2000);
  lcd.clear();
}

void loop() {
  // Get the current time
  DateTime now = rtc.now();

  // Display the current time
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());

  // Check if it's time to ring the bell
  if (now.hour() == alarmHour && now.minute() == alarmMinute) {
    ringBell();
  }

  // Check button states for setting the time or alarm
  if (digitalRead(setHourPin) == LOW) {
    settingTime = !settingTime;
    delay(500);  // Debounce delay
  }

  if (settingTime) {
    setTime();
  } else {
    // If not setting time, display alarm time
    lcd.setCursor(0, 1);
    lcd.print("Bell at: ");
    lcd.print(alarmHour);
    lcd.print(":");
    lcd.print(alarmMinute);

    // Check buttons for changing alarm time
    if (digitalRead(increasePin) == LOW) {
      updateAlarmTime(true);  // Increase alarm time
      delay(500);  // Debounce delay
    }

    if (digitalRead(decreasePin) == LOW) {
      updateAlarmTime(false);  // Decrease alarm time
      delay(500);  // Debounce delay
    }
  }

  delay(1000);  // Update every second
}

void setTime() {
  // Display time setting mode
  lcd.setCursor(0, 1);
  lcd.print("Set Time:");

  if (digitalRead(increasePin) == LOW) {
    if (digitalRead(setHourPin) == LOW) {
      setHour = (setHour + 1) % 24;
    } else if (digitalRead(setMinutePin) == LOW) {
      setMinute = (setMinute + 1) % 60;
    }
    delay(500);  // Debounce delay
  }

  if (digitalRead(decreasePin) == LOW) {
    if (digitalRead(setHourPin) == LOW) {
      setHour = (setHour + 23) % 24;  // Decrement hour
    } else if (digitalRead(setMinutePin) == LOW) {
      setMinute = (setMinute + 59) % 60;  // Decrement minute
    }
    delay(500);  // Debounce delay
  }

  // Display the setting time
  lcd.setCursor(10, 1);
  lcd.print(setHour);
  lcd.print(":");
  lcd.print(setMinute);

  // Update RTC with new time if setting is done
  if (settingTime && digitalRead(setHourPin) == LOW && digitalRead(setMinutePin) == LOW) {
    rtc.adjust(DateTime(2024, 6, 18, setHour, setMinute, 0));
    settingTime = false;
    delay(500);  // Debounce delay
  }
}

void updateAlarmTime(bool increase) {
  if (increase) {
    // Increase alarm time
    if (alarmMinute < 59) {
      alarmMinute++;
    } else {
      alarmMinute = 0;
      if (alarmHour < 23) {
        alarmHour++;
      } else {
        alarmHour = 0;
      }
    }
  } else {
    // Decrease alarm time
    if (alarmMinute > 0) {
      alarmMinute--;
    } else {
      alarmMinute = 59;
      if (alarmHour > 0) {
        alarmHour--;
      } else {
        alarmHour = 23;
      }
    }
  }

  // Update LCD with new alarm time
  lcd.setCursor(10, 1);
  lcd.print(alarmHour);
  lcd.print(":");
  lcd.print(alarmMinute);
}

void ringBell() {
  // Sound the buzzer for a set duration
  digitalWrite(buzzerPin, HIGH);
  delay(1000);  // Ring for 1 second
  digitalWrite(buzzerPin, LOW);
}
