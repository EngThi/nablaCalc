#include "board_pins.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_keypad.h"
#include "nvs_flash.h"
#include <stdio.h>

static const char *TAG = "NABLA_CALC";

static i2c_master_bus_handle_t s_i2c_bus_handle = NULL;

static esp_err_t init_i2c_bus(void) {
  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_NUM_0,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };

  esp_err_t ret = i2c_new_master_bus(&bus_config, &s_i2c_bus_handle);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao criar barramento I2C mestre: %s",
             esp_err_to_name(ret));
    return ret;
  }

  ESP_LOGI(TAG, "Barramento I2C mestre inicializado (SDA: GPIO%d, SCL: GPIO%d)",
           I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
  return ESP_OK;
}

static void keypad_task(void *arg) {
  ESP_LOGI(TAG, "Task de Varredura do Teclado 7x7 iniciada com sucesso.");

  while (1) {
    key_code_t key = keypad_scan_once();
    if (key != KEY_NONE) {
      ESP_LOGI(TAG, "[TECLA ACIONADA] Código: %2d | Identificador: %s", key,
               keypad_get_name(key));
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void app_main(void) {
  ESP_LOGI(TAG, "=================================================");
  ESP_LOGI(TAG, "  Iniciando ∇Calc Firmware (ESP-IDF / FreeRTOS)  ");
  ESP_LOGI(TAG, "  Alvo: ESP32-S3-WROOM-1-N16R8                   ");
  ESP_LOGI(TAG, "=================================================");

  // 1. Inicializar memória não volátil (NVS)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // 2. Inicializar barramento I2C Mestre
  ESP_ERROR_CHECK(init_i2c_bus());

  // 3. Inicializar expansor de teclado MCP23017
  ret = keypad_init(s_i2c_bus_handle);
  if (ret != ESP_OK) {
    ESP_LOGW(TAG, "Aviso: Expansor MCP23017 não respondeu no endereço 0x20 "
                  "(verifique a placa/alimentação).");
  }

  // 4. Criar Task do FreeRTOS para a varredura do teclado
  xTaskCreatePinnedToCore(keypad_task, "keypad_task", 4096, NULL, 5, NULL, 1);

  ESP_LOGI(TAG, "Sistema pronto e em execução.");
}
