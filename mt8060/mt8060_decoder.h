#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HUMIDITY    = 0x41,
    TEMPERATURE = 0x42,
    CO2_PPM     = 0x50,
} dataType;

typedef struct {
    dataType type;
    uint16_t value;
    bool checksumIsValid;
} mt8060_message;

// Вызывается на каждый задний фронт тактового сигнала, возвращает ссылку на структуру сообщения, если оно полностью считано
mt8060_message* mt8060_process(unsigned long ms, bool data);

