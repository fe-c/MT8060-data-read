#include "mt8060_decoder.h"
// Таймаут по которому считаем, что началось новое сообщение
#define MT8060_MAX_MS  2

// В одном полном сообшщении 5 байт
#define MT8060_MSG_LEN 5

#define MT8060_MSG_TYPE_BYTE_IDX          0
#define MT8060_MSG_VAL_HIGH_BYTE_IDX 1
#define MT8060_MSG_VAL_LOW_BYTE_IDX       2
#define MT8060_MSG_CHECKSUM_BYTE_IDX      3
#define MT8060_MSG_CR_BYTE_IDX            4

#define BITS_IN_BYTE 8


// Буфер для хранения считанных данных
static uint8_t buffer[MT8060_MSG_LEN];
static int num_bits = 0;
static unsigned long prev_ms;

static mt8060_message _msg;
static mt8060_message *msg = &_msg;

// Декодирует сообщение
void mt8060_decode(void)
{
    // Вычисление контрольной суммы
    uint8_t checksum = buffer[MT8060_MSG_TYPE_BYTE_IDX] + buffer[MT8060_MSG_VAL_HIGH_BYTE_IDX] + buffer[MT8060_MSG_VAL_LOW_BYTE_IDX];
    // Проверка контрольной суммы
    msg->checksumIsValid = (checksum == buffer[MT8060_MSG_CHECKSUM_BYTE_IDX] && buffer[MT8060_MSG_CR_BYTE_IDX] == 0xD);
    if (!msg->checksumIsValid) return;

    // Получение типа показателя
    msg->type = (dataType)buffer[MT8060_MSG_TYPE_BYTE_IDX];
    // Получение значения показателя
    msg->value = buffer[MT8060_MSG_VAL_HIGH_BYTE_IDX] << BITS_IN_BYTE | buffer[MT8060_MSG_VAL_LOW_BYTE_IDX];
}

// Вызывается на каждый задний фронт тактового сигнала, возвращает ссылку на структуру сообщения, если оно полностью считано
mt8060_message* mt8060_process(unsigned long ms, bool data)
{
    // Определение начала нового сообщение, на основании времени, прошедшего с момента получения последнего бита
    if ((ms - prev_ms) > MT8060_MAX_MS) {
        num_bits = 0;
    }
    
    prev_ms = ms;
    if (num_bits < MT8060_MSG_LEN * BITS_IN_BYTE) {
        // Вычисление индекса байта
        int idx = num_bits / BITS_IN_BYTE;
        //Добавление к текущему байту очередного бита
        buffer[idx] = (buffer[idx] << 1) | (data ? 1 : 0);
        // Увеличение счётчика прочитанных битов
        num_bits++;
        // Возвращаем true, если сообщение прочитано целиком
        if (num_bits == MT8060_MSG_LEN * BITS_IN_BYTE) {
            mt8060_decode();
            return msg;
        }
    }

    // Ждем следущего бита
    return nullptr;
}
