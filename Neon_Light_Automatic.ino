#include <Wire.h>
#include <RtcDS1302.h>

ThreeWire myWire(7, 6, 8); // Пины для DS1302: IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

const int relayPin = 9; // Пин управления реле

// Определение временных интервалов
const int morningStartHour = 6;
const int morningStartMinute = 0;
const int morningEndHour = 8;
const int morningEndMinute = 30;

const int eveningStartHour = 17;
const int eveningStartMinute = 30;
const int eveningEndHour = 2; // Следующий день
const int eveningEndMinute = 0;

void setup() {
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid() || !Rtc.GetIsRunning()) {
      Rtc.SetDateTime(compiled);
  }
}

bool isInTimeRange(int startHour, int startMinute, int endHour, int endMinute, const RtcDateTime& now) {
  // Специальная логика для периодов, охватывающих полночь
  if (endHour < startHour) {
    return (now.Hour() > startHour || now.Hour() < endHour) ||
           (now.Hour() == startHour && now.Minute() >= startMinute) ||
           (now.Hour() == endHour && now.Minute() < endMinute);
  }

  // Сравнение с началом периода
  bool afterStart = (now.Hour() > startHour) || (now.Hour() == startHour && now.Minute() >= startMinute);
  // Сравнение с концом периода
  bool beforeEnd = (now.Hour() < endHour) || (now.Hour() == endHour && now.Minute() < endMinute);
  return afterStart && beforeEnd;
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  // Показ текущей даты и времени в Serial Monitor
  Serial.print(now.Year());
  Serial.print('/');
  Serial.print(now.Month());
  Serial.print('/');
  Serial.print(now.Day());
  Serial.print(" ");
  Serial.print(now.Hour());
  Serial.print(':');
  Serial.print(now.Minute());
  Serial.print(':');
  Serial.println(now.Second());

  // Проверка, включен ли светильник
  bool isLightOn = isInTimeRange(morningStartHour, morningStartMinute, morningEndHour, morningEndMinute, now) ||
                   isInTimeRange(eveningStartHour, eveningStartMinute, eveningEndHour, eveningEndMinute, now);

  digitalWrite(relayPin, isLightOn ? LOW : HIGH);

  delay(1000); // Задержка 1 секунда
}
