#include "i2c_keypad.h"
#include "board_pins.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "I2C_KEYPAD";

// Endereços dos Registradores do MCP23017
#define MCP_REG_IODIRA 0x00
#define MCP_REG_IODIRB 0x01
#define MCP_REG_IOCON 0x0A
#define MCP_REG_GPPUA 0x0C
#define MCP_REG_GPPUB 0x0D
#define MCP_REG_GPIOB 0x13
#define MCP_REG_OLATA 0x14

#define NUM_ROWS 7
#define NUM_COLS 7

#define DEBOUNCE_STABLE_THRESHOLD 2

static i2c_master_dev_handle_t s_mcp_dev = NULL;
static uint8_t s_debounced_state[NUM_ROWS] = {0x7F, 0x7F, 0x7F, 0x7F,
                                              0x7F, 0x7F, 0x7F};
static uint8_t s_debounce_count[NUM_ROWS][NUM_COLS] = {0};

static const key_code_t s_keymap[NUM_ROWS][NUM_COLS] = {
    // ROW 0: SW1 (SHIFT), SW2 (HOME), SW3 (LEFT), SW4 (UP), SW5 (RIGHT), SW6
    // (PHOTO), SW7 (SETUP/ON)
    {KEY_SHIFT, KEY_HOME, KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_PHOTO, KEY_SETUP},
    // ROW 1: SW8 (SOLVE), SW9 (ALPHA), SW10 (FRAC), SW11 (DOWN), SW12 (SQRT),
    // SW13 (MODE), SW14 (x^y)
    {KEY_SOLVE, KEY_ALPHA, KEY_FRAC, KEY_DOWN, KEY_SQRT, KEY_MODE, KEY_POWER},
    // ROW 2: SW15 (LOG), SW16 (LN), SW17 (SIN), SW18 (COS), SW19 (x²), SW20
    // (TAN), SW21 (π)
    {KEY_LOG, KEY_LN, KEY_SIN, KEY_COS, KEY_SQUARE, KEY_TAN, KEY_PI},
    // ROW 3: SW22 (e), SW23 (y√x), SW24 (')'), SW25 ('9'), SW26 (S-D), SW27
    // (','), SW28 (BACK)
    {KEY_E, KEY_NTH_ROOT, KEY_RPAREN, KEY_NUM_9, KEY_SD, KEY_COMMA, KEY_BACK},
    // ROW 4: SW29 ('7'), SW30 ('('), SW31 ('8'), SW32 ('6'), SW33 (DEL), SW34
    // (AC), SW35 ('(-)')
    {KEY_NUM_7, KEY_LPAREN, KEY_NUM_8, KEY_NUM_6, KEY_DEL, KEY_AC, KEY_NEG},
    // ROW 5: SW36 ('4'), SW37 ('5'), SW38 ('2'), SW39 ('3'), SW40 ('*'), SW41
    // ('+'), SW42 ('/')
    {KEY_NUM_4, KEY_NUM_5, KEY_NUM_2, KEY_NUM_3, KEY_MUL, KEY_ADD, KEY_DIV},
    // ROW 6: SW43 ('1'), SW44 ('0'), SW45 ('.'), SW46 (EXP), SW47 (Ans), SW48
    // ('='), SW49 ('-')
    {KEY_NUM_1, KEY_NUM_0, KEY_DOT, KEY_EXP, KEY_ANS, KEY_EQUALS, KEY_SUB},
};

static const char *const s_key_names[KEY_COUNT] = {
    [KEY_NONE] = "NONE", [KEY_SHIFT] = "SHIFT",  [KEY_ALPHA] = "ALPHA",
    [KEY_HOME] = "HOME", [KEY_PHOTO] = "PHOTO",  [KEY_SOLVE] = "SOLVE",
    [KEY_MODE] = "MODE", [KEY_SETUP] = "ON",     [KEY_SIN] = "sin",
    [KEY_COS] = "cos",   [KEY_TAN] = "tan",      [KEY_LOG] = "log",
    [KEY_LN] = "ln",     [KEY_SQRT] = "sqrt",    [KEY_SQUARE] = "x²",
    [KEY_POWER] = "x^y", [KEY_NTH_ROOT] = "y√x", [KEY_LPAREN] = "(",
    [KEY_RPAREN] = ")",  [KEY_COMMA] = ",",      [KEY_PI] = "π",
    [KEY_E] = "e",       [KEY_NUM_7] = "7",      [KEY_NUM_8] = "8",
    [KEY_NUM_9] = "9",   [KEY_DEL] = "DEL",      [KEY_AC] = "AC",
    [KEY_LEFT] = "LEFT", [KEY_RIGHT] = "RIGHT",  [KEY_NUM_4] = "4",
    [KEY_NUM_5] = "5",   [KEY_NUM_6] = "6",      [KEY_MUL] = "*",
    [KEY_DIV] = "/",     [KEY_UP] = "UP",        [KEY_DOWN] = "DOWN",
    [KEY_NUM_1] = "1",   [KEY_NUM_2] = "2",      [KEY_NUM_3] = "3",
    [KEY_ADD] = "+",     [KEY_SUB] = "-",        [KEY_ANS] = "Ans",
    [KEY_EXP] = "EXP",   [KEY_NUM_0] = "0",      [KEY_DOT] = ".",
    [KEY_NEG] = "(-)",   [KEY_FRAC] = "a/b",     [KEY_SD] = "S-D",
    [KEY_BACK] = "BACK", [KEY_EQUALS] = "=",
};

static esp_err_t mcp_write_reg(uint8_t reg, uint8_t value) {
  uint8_t buffer[2] = {reg, value};
  return i2c_master_transmit(s_mcp_dev, buffer, sizeof(buffer), 50);
}

esp_err_t keypad_init(i2c_master_bus_handle_t bus_handle) {
  if (bus_handle == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = MCP23017_I2C_ADDR,
      .scl_speed_hz = I2C_MASTER_FREQ_HZ,
  };

  esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &s_mcp_dev);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao registrar MCP23017 no I2C: %s",
             esp_err_to_name(ret));
    return ret;
  }

  // Configuração dos registradores do MCP23017:
  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_IOCON, 0x00));

  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_OLATA, 0x00));

  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_IODIRA, 0xFF));

  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_IODIRB, 0x7F));

  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_GPPUB, 0x7F));

  ESP_LOGI(
      TAG,
      "MCP23017 inicializado com sucesso (Hi-Z Tri-state scanning ativado)!");
  return ESP_OK;
}

bool keypad_poll_event(key_event_t *event) {
  if (s_mcp_dev == NULL || event == NULL) {
    return false;
  }

  for (uint8_t row = 0; row < NUM_ROWS; row++) {
    uint8_t iodira_mask = (uint8_t)(0xFF & ~(1 << row));
    mcp_write_reg(MCP_REG_IODIRA, iodira_mask);

    // Lê o estado das 7 colunas (Porta B)
    uint8_t reg = MCP_REG_GPIOB;
    uint8_t col_data = 0xFF;
    esp_err_t ret =
        i2c_master_transmit_receive(s_mcp_dev, &reg, 1, &col_data, 1, 50);

    mcp_write_reg(MCP_REG_IODIRA, 0xFF);

    if (ret != ESP_OK) {
      continue;
    }

    col_data &= 0x7F;

    for (uint8_t col = 0; col < NUM_COLS; col++) {
      uint8_t current_bit = (col_data >> col) & 0x01;
      uint8_t stable_bit = (s_debounced_state[row] >> col) & 0x01;

      if (current_bit != stable_bit) {
        s_debounce_count[row][col]++;
        if (s_debounce_count[row][col] >= DEBOUNCE_STABLE_THRESHOLD) {
          s_debounce_count[row][col] = 0;
          if (current_bit == 0) {
            // 1 -> 0: Tecla Pressionada
            s_debounced_state[row] &= (uint8_t)(~(1 << col));
            event->key = s_keymap[row][col];
            event->type = KEY_EVENT_PRESS;
            return true;
          } else {
            // 0 -> 1: Tecla Liberada (Key Release)
            s_debounced_state[row] |= (uint8_t)(1 << col);
            event->key = s_keymap[row][col];
            event->type = KEY_EVENT_RELEASE;
            return true;
          }
        }
      } else {
        s_debounce_count[row][col] = 0;
      }
    }
  }

  return false;
}

key_code_t keypad_scan_once(void) {
  key_event_t evt;
  while (keypad_poll_event(&evt)) {
    if (evt.type == KEY_EVENT_PRESS) {
      return evt.key;
    }
  }
  return KEY_NONE;
}

const char *keypad_get_name(key_code_t key) {
  if (key >= KEY_COUNT) {
    return "DESCONHECIDO";
  }
  return s_key_names[key];
}
