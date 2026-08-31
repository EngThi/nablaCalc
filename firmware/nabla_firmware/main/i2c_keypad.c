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
#define MCP_REG_GPPUB 0x0D // Pull-up das Colunas (Porta B)
#define MCP_REG_GPIOB 0x13 // Leitura das Colunas
#define MCP_REG_OLATA 0x14 // Saída das Linhas

#define NUM_ROWS 7
#define NUM_COLS 7

static i2c_master_dev_handle_t s_mcp_dev = NULL;
static uint8_t s_last_state[NUM_ROWS] = {0x7F, 0x7F, 0x7F, 0x7F,
                                         0x7F, 0x7F, 0x7F};

static const key_code_t s_keymap[NUM_ROWS][NUM_COLS] = {
    {KEY_SHIFT, KEY_ALPHA, KEY_HOME, KEY_PHOTO, KEY_SOLVE, KEY_MODE, KEY_SETUP},
    {KEY_SIN, KEY_COS, KEY_TAN, KEY_LOG, KEY_LN, KEY_SQRT, KEY_SQUARE},
    {KEY_POWER, KEY_NTH_ROOT, KEY_LPAREN, KEY_RPAREN, KEY_COMMA, KEY_PI, KEY_E},
    {KEY_NUM_7, KEY_NUM_8, KEY_NUM_9, KEY_DEL, KEY_AC, KEY_LEFT, KEY_RIGHT},
    {KEY_NUM_4, KEY_NUM_5, KEY_NUM_6, KEY_MUL, KEY_DIV, KEY_UP, KEY_DOWN},
    {KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, KEY_ADD, KEY_SUB, KEY_ANS, KEY_EXP},
    {KEY_NUM_0, KEY_DOT, KEY_NEG, KEY_FRAC, KEY_SD, KEY_BACK, KEY_EQUALS},
};

static const char *const s_key_names[KEY_COUNT] = {
    [KEY_NONE] = "NONE", [KEY_SHIFT] = "SHIFT",  [KEY_ALPHA] = "ALPHA",
    [KEY_HOME] = "HOME", [KEY_PHOTO] = "PHOTO",  [KEY_SOLVE] = "SOLVE",
    [KEY_MODE] = "MODE", [KEY_SETUP] = "SETUP",  [KEY_SIN] = "sin",
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

  // Configura os registradores
  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_IOCON, 0x00));
  ESP_ERROR_CHECK(mcp_write_reg(MCP_REG_IODIRA, 0x80)); // 7 Linhas como Saída
  ESP_ERROR_CHECK(
      mcp_write_reg(MCP_REG_IODIRB, 0x7F)); // 7 Colunas como Entrada
  ESP_ERROR_CHECK(
      mcp_write_reg(MCP_REG_GPPUB, 0x7F)); // Ativa Pull-ups nas Colunas
  ESP_ERROR_CHECK(
      mcp_write_reg(MCP_REG_OLATA, 0x7F)); // Linhas em repouso (HIGH)

  ESP_LOGI(TAG, "MCP23017 inicializado com sucesso no endereço 0x%02X!",
           MCP23017_I2C_ADDR);
  return ESP_OK;
}

key_code_t keypad_scan_once(void) {
  if (s_mcp_dev == NULL) {
    return KEY_NONE;
  }

  for (uint8_t row = 0; row < NUM_ROWS; row++) {
    // Coloca apenas a linha atual em LOW (0), outras em HIGH (1)
    uint8_t row_mask = (uint8_t)(~(1 << row) & 0x7F);
    mcp_write_reg(MCP_REG_OLATA, row_mask);

    // Lê o estado das 7 colunas (Porta B)
    uint8_t reg = MCP_REG_GPIOB;
    uint8_t col_data = 0xFF;
    esp_err_t ret =
        i2c_master_transmit_receive(s_mcp_dev, &reg, 1, &col_data, 1, 50);

    if (ret == ESP_OK) {
      col_data &= 0x7F;

      // Detecta se houve transição de Solto (1) para Pressionado (0)
      uint8_t pressed = (s_last_state[row] & (uint8_t)(~col_data));
      s_last_state[row] = col_data;

      if (pressed != 0) {
        for (uint8_t col = 0; col < NUM_COLS; col++) {
          if (pressed & (1 << col)) {
            return s_keymap[row][col];
          }
        }
      }
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
