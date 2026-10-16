#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Código de identificação único para cada uma das 49 teclas da matriz 7x7
typedef enum {
  KEY_NONE = 0,

  KEY_SHIFT,
  KEY_ALPHA,
  KEY_HOME,
  KEY_PHOTO,
  KEY_SOLVE,
  KEY_MODE,
  KEY_SETUP,

  KEY_SIN,
  KEY_COS,
  KEY_TAN,
  KEY_LOG,
  KEY_LN,
  KEY_SQRT,
  KEY_SQUARE,

  KEY_POWER,
  KEY_NTH_ROOT,
  KEY_LPAREN,
  KEY_RPAREN,
  KEY_COMMA,
  KEY_PI,
  KEY_E,

  KEY_NUM_7,
  KEY_NUM_8,
  KEY_NUM_9,
  KEY_DEL,
  KEY_AC,
  KEY_LEFT,
  KEY_RIGHT,

  KEY_NUM_4,
  KEY_NUM_5,
  KEY_NUM_6,
  KEY_MUL,
  KEY_DIV,
  KEY_UP,
  KEY_DOWN,

  KEY_NUM_1,
  KEY_NUM_2,
  KEY_NUM_3,
  KEY_ADD,
  KEY_SUB,
  KEY_ANS,
  KEY_EXP,

  KEY_NUM_0,
  KEY_DOT,
  KEY_NEG,
  KEY_FRAC,
  KEY_SD,
  KEY_BACK,
  KEY_EQUALS,
  KEY_COUNT
} key_code_t;

#define KEY_ON KEY_SETUP

typedef enum {
  KEY_EVENT_NONE = 0,
  KEY_EVENT_PRESS,
  KEY_EVENT_RELEASE,
} key_event_type_t;

typedef struct {
  key_code_t key;
  key_event_type_t type;
} key_event_t;

/**
 * @brief Inicializa o expansor MCP23017 no barramento I2C para a matriz 7x7.
 *
 * @param bus_handle Handle do barramento mestre I2C já inicializado.
 * @return esp_err_t ESP_OK em caso de sucesso, ou código de erro.
 */
esp_err_t keypad_init(i2c_master_bus_handle_t bus_handle);

/**
 * @brief Obtém o próximo evento debounced da matriz (Press ou Release).
 *
 * @param[out] event Ponteiro para estrutura que receberá o evento.
 * @return true se um evento foi detectado, false se nenhum evento.
 */
bool keypad_poll_event(key_event_t *event);

/**
 * @brief Executa um ciclo completo de varredura (7 linhas x 7 colunas).
 *
 * @return key_code_t Retorna a tecla debounced na transição de pressão, ou
 * KEY_NONE.
 */
key_code_t keypad_scan_once(void);

/**
 * @brief Converte o enum do código da tecla para uma string legível para
 * depuração.
 *
 * @param key Código da tecla.
 * @return const char* Nome textual da tecla (ex: "sin", "7", "PHOTO", "=").
 */
const char *keypad_get_name(key_code_t key);

#ifdef __cplusplus
}
#endif
