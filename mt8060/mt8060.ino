#include <SPI.h>
#include <stdint.h>
#include "mt8060_decoder.h"

// Номера пинов, с которых будем считывать данные
#define PIN_CLOCK  2
#define PIN_DATA   3

void setup(void)
{
  // Настройка пинов
  pinMode(PIN_CLOCK, INPUT);
  pinMode(PIN_DATA, INPUT);

  // Открытие последовательного порта
  Serial.begin(19200);
}

void loop(void)
{
  // Ждем низкого уровня сигнала синхронизации
  while (digitalRead(PIN_CLOCK) != LOW);

  // Считываем текущее значение сигнала данных
  bool dataBit = (digitalRead(PIN_DATA) == HIGH);
  unsigned long ms = millis();
  // Обрабатываем его как один бит, пока не получим полное сообщение
  mt8060_message* message = mt8060_process(ms, dataBit);
  if (message) {
    if (!message->checksumIsValid)
    {
        Serial.println("Checksum validation error.");
    }
    else
    {
        switch (message->type) {
          // Вывод относительной влажности
          case HUMIDITY: Serial.print("HUM:"); Serial.println((double)message->value / 100, 2); break;
          // Преобразование и вывод температуры
          case TEMPERATURE: Serial.print("TMP:"); Serial.println((double)message->value / 16 - 273.15, 1); break;
          // Вывод уровня CO2
          case CO2_PPM: Serial.print("CO2:"); Serial.println(message->value, DEC); break;
          default: break;
        }
    }
  }

  // Ожидание низкого уровня сигнала синхронизации
  while (digitalRead(PIN_CLOCK) == LOW);
}
