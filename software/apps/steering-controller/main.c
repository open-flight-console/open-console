// Analog accelerometer app
//
// Reads data from the ADXL327 analog accelerometer

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_spi.h"
#include "nrf_serial.h"

#include "buckler.h"
#include "display.h"
#include "lsm9ds1.h"
#include "steeringwheel.h"
//#include "nrf52dk.h"

//from https://github.com/lab11/nrf52x-base/blob/6ca5df7892d2a26c864b52f1b5bf383e16885d25/apps/demo_nrf52dk/main.c
// Serial configuration
// configuration for uart, RX & TX pin, empty RTS and CTS pins,
//  flow control disabled, no parity bit, 115200 baud, default priority
NRF_SERIAL_DRV_UART_CONFIG_DEF(serial_uart_config, BUCKLER_UART_RX, BUCKLER_UART_TX, 0, 0,
      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED, NRF_UART_BAUDRATE_115200, UART_DEFAULT_CONFIG_IRQ_PRIORITY);
// create serial queues for commands, tx length 32, rx length 32
NRF_SERIAL_QUEUES_DEF(serial_queues1, 1, 1);
// create serial buffers for data, tx size 100 bytes, rx size 100 bytes
NRF_SERIAL_BUFFERS_DEF(serial_buffers1, 1, 1);
// create a configuration using DMA with queues for commands and buffers for data storage
// both handlers are set to NULL as we do not need to support them
NRF_SERIAL_CONFIG_DEF(serial_config1, NRF_SERIAL_MODE_DMA, &serial_queues1, &serial_buffers1, NULL, NULL);

NRF_SERIAL_UART_DEF(serial_uart_instance, 0);

NRF_TWI_MNGR_DEF(twi_mngr_instance, 5, 0);

//error handler
//uart code from https://github.com/lab11/nrf52x-base/blob/6ca5df7892d2a26c864b52f1b5bf383e16885d25/apps/uart/main.c
//UART initialization
int main(void) {
  ret_code_t error_code = NRF_SUCCESS;
  
  // init UART
  nrf_serial_init(&serial_uart_instance, &serial_uart_config, &serial_config1);
  static char tx_message[] = "u";
  error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
  APP_ERROR_CHECK(error_code);

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  printf("Log initialized!\n");
  // initialize LEDs
  nrf_gpio_pin_dir_set(23, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(24, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(25, NRF_GPIO_PIN_DIR_OUTPUT);

  // initialize display
  nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
  nrf_drv_spi_config_t spi_config = {
    .sck_pin = BUCKLER_LCD_SCLK,
    .mosi_pin = BUCKLER_LCD_MOSI,
    .miso_pin = BUCKLER_LCD_MISO,
    .ss_pin = BUCKLER_LCD_CS,
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0,
    .frequency = NRF_DRV_SPI_FREQ_4M,
    .mode = NRF_DRV_SPI_MODE_2,
    .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  };
  error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(error_code);
  display_init(&spi_instance);
  display_write("Turn me UWU1", DISPLAY_LINE_0);
  printf("Display initialized!\n");
  
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = BUCKLER_SENSORS_SCL;
  i2c_config.sda = BUCKLER_SENSORS_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  error_code = nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  APP_ERROR_CHECK(error_code);
  lsm9ds1_init(&twi_mngr_instance);
  printf("IMU initialized!\n");
  
  steering_wheel_state_t state = OFF;
  // loop forever, running state machine
  
  while (1) {
    nrf_delay_ms(1);
    state = controller(state);
    if (state == STRAIGHT) {
      static char tx_message[] = "w";
      error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
      APP_ERROR_CHECK(error_code);
      printf("STRAIGHT");
    } else if (state == LEFT) {
      static char tx_message[] = "a";
      error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
      APP_ERROR_CHECK(error_code);
      printf("LEFT");
    } else if (state == RIGHT) {
      static char tx_message[] = "d";
      error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
      APP_ERROR_CHECK(error_code);
      printf("RIGHT");
    } else if (state == PITCHUP) {
      static char tx_message[] = "+";
      error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
      APP_ERROR_CHECK(error_code);
      printf("RIGHT");
    } else if (state == PITCHDN) {
      static char tx_message[] = "-";
      error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
      APP_ERROR_CHECK(error_code);
      printf("RIGHT");
    }
  }
  nrf_serial_uninit(&serial_uart_instance);
}
