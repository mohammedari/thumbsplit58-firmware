#define COMPILE_RIGHT
//#define COMPILE_LEFT

#include "thumbsplit58.h"
#include "nrf_drv_config.h"
#include "nrf_gzll.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"

/*****************************************************************************/
/** Configuration */
/*****************************************************************************/
#define ADDRESS_PIPE0 0x836b5fea         //< Device unique address for data pipe 0
#define ADDRESS_PIPE1 0x0f2f6886         //< Device unique address for data pipe 1
#define MAX_TX_ATTEMPTS 20               //< Number of maximum attempts to send a packet
#define TX_POWER NRF_GZLL_TX_POWER_4_DBM //< Transmission power (0db is the default)
#define MAINTENANCE_TX_INTERVAL_MS 125   //< Resending interval after a transmission fail [ms]
#define TIMEOUT_TO_SLEEP_MS 500          //< Sleeps when no activity exists for this timeout [ms]
#define SYNC_LIFETIME_MS 0               //< Timeout for tracking host frequency hopping [ms]

// Nordic SoC uses (2400 + channel) MHz to communicate between host and device.
// Note you have to use eligible frequencies in your country, though SoC itself accepts 0-125.
static uint8_t channel_table[] = { 82, 94 };

/*****************************************************************************/
/** Main Implementations */
/*****************************************************************************/

const nrf_drv_rtc_t rtc_debounce = NRF_DRV_RTC_INSTANCE(0);

// Setup switch pins with pullups
static void gpio_config(void)
{
    nrf_gpio_cfg_sense_input(S01, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S02, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S03, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S04, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S05, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S06, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S07, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S08, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S09, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S10, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S11, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S12, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S13, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S14, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S15, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S16, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S17, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S18, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S19, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S20, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S21, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S22, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S23, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S24, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S25, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S26, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S27, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S28, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S29, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(S30, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    // We have LED on the board, but do not use it.
    nrf_gpio_cfg_output(LED);
    nrf_gpio_pin_clear(LED);       
}

// Return the key states, masked with valid key pins
static uint32_t read_keys(void)
{
    return ~NRF_GPIO->IN & INPUT_MASK;
}

// Assemble packet and send to receiver
static void send(uint32_t keys)
{
    uint8_t data_payload[4]; 

    data_payload[0] = ((keys & 1<<S01) ? 1:0) << 0 | \
                      ((keys & 1<<S02) ? 1:0) << 1 | \
                      ((keys & 1<<S03) ? 1:0) << 2 | \
                      ((keys & 1<<S04) ? 1:0) << 3 | \
                      ((keys & 1<<S05) ? 1:0) << 4 | \
                      ((keys & 1<<S06) ? 1:0) << 5 | \
                      ((keys & 1<<S07) ? 1:0) << 6 | \
                      ((keys & 1<<S08) ? 1:0) << 7;

    data_payload[1] = ((keys & 1<<S09) ? 1:0) << 0 | \
                      ((keys & 1<<S10) ? 1:0) << 1 | \
                      ((keys & 1<<S11) ? 1:0) << 2 | \
                      ((keys & 1<<S12) ? 1:0) << 3 | \
                      ((keys & 1<<S13) ? 1:0) << 4 | \
                      ((keys & 1<<S14) ? 1:0) << 5 | \
                      ((keys & 1<<S15) ? 1:0) << 6 | \
                      ((keys & 1<<S16) ? 1:0) << 7;

    data_payload[2] = ((keys & 1<<S17) ? 1:0) << 0 | \
                      ((keys & 1<<S18) ? 1:0) << 1 | \
                      ((keys & 1<<S19) ? 1:0) << 2 | \
                      ((keys & 1<<S20) ? 1:0) << 3 | \
                      ((keys & 1<<S21) ? 1:0) << 4 | \
                      ((keys & 1<<S22) ? 1:0) << 5 | \
                      ((keys & 1<<S23) ? 1:0) << 6 | \
                      ((keys & 1<<S24) ? 1:0) << 7;

    data_payload[3] = ((keys & 1<<S25) ? 1:0) << 0 | \
                      ((keys & 1<<S26) ? 1:0) << 1 | \
                      ((keys & 1<<S27) ? 1:0) << 2 | \
                      ((keys & 1<<S28) ? 1:0) << 3 | \
                      ((keys & 1<<S29) ? 1:0) << 4 | \
                      ((keys & 1<<S30) ? 1:0) << 5 | \
                                            0 << 6 | \
                                            0 << 7;

    nrf_gzll_add_packet_to_tx_fifo(PIPE_NUMBER, data_payload, 4);
}

extern uint32_t debounce(uint32_t previous_state, uint32_t snapshot);

// Tick counters for handler
uint32_t ticks = 0; 
uint32_t remote_ticks = 0;
uint32_t remote_state = 0;
uint32_t last_activity_ticks = 0;
bool need_maintenance = false;
static void handler_debounce(nrf_drv_rtc_int_type_t int_type)
{
    uint32_t snapshot = read_keys();
    uint32_t state = debounce(remote_state, snapshot);

    ++ticks;
    if (snapshot != 0)
        last_activity_ticks = ticks;

    // Timeout to sleep
    if (ticks - last_activity_ticks > TIMEOUT_TO_SLEEP_MS)
    {
        nrf_drv_rtc_disable(&rtc_debounce);
        return;
    }

    // Send if key state changes 
    if (state != remote_state)
    {
        send(state);
        remote_state = state;
        remote_ticks = ticks;
        return;
    }

    // If transmission failed before, send a packet again
    if (need_maintenance && ticks - remote_ticks > MAINTENANCE_TX_INTERVAL_MS)
    {
        send(state);
        remote_state = state;
        remote_ticks = ticks;
        return;
    }
}

int main(void)
{
    // Configure all keys as inputs with pullups
    gpio_config();

    // Initialize Gazell
    nrf_gzll_init(NRF_GZLL_MODE_DEVICE);

    // Set max attempts of sending packets
    _Static_assert(MAX_TX_ATTEMPTS >= sizeof(channel_table) * NRF_GZLL_DEFAULT_TIMESLOTS_PER_CHANNEL,
        "Retry count is too small to establish synchronization.");
    nrf_gzll_set_max_tx_attempts(MAX_TX_ATTEMPTS);

    // Set TX power
    nrf_gzll_set_tx_power(TX_POWER);

    // Modify frequency hopping synchronization lifetime to reduce retry count
    nrf_gzll_set_sync_lifetime(SYNC_LIFETIME_MS * 1000 / NRF_GZLL_DEFAULT_TIMESLOT_PERIOD);
    nrf_gzll_set_timeslots_per_channel_when_device_out_of_sync(sizeof(channel_table) * NRF_GZLL_DEFAULT_TIMESLOTS_PER_CHANNEL);

    // Set channel table 
    nrf_gzll_set_channel_table(channel_table, sizeof(channel_table));

    // Addressing
    nrf_gzll_set_base_address_0(ADDRESS_PIPE0);
    nrf_gzll_set_base_address_1(ADDRESS_PIPE1);

    // Enable Gazell to start sending over the air
    nrf_gzll_enable();

    // Configure 32kHz xtal oscillator
    nrf_drv_clock_init();
    nrf_drv_clock_lfclk_request(NULL);

    // Configure RTC peripherals with ticks
    nrf_drv_rtc_init(&rtc_debounce, NULL, handler_debounce);
    nrf_drv_rtc_tick_enable(&rtc_debounce, true);

    // Set the GPIOTE PORT event as interrupt source, and enable interrupts for GPIOTE
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    NVIC_EnableIRQ(GPIOTE_IRQn);

    while (true)
        __WFI();
}

/*****************************************************************************/
/** IRQ callback function definitions  */
/*****************************************************************************/

void GPIOTE_IRQHandler(void)
{
    if (NRF_GPIOTE->EVENTS_PORT)
    {
        NRF_GPIOTE->EVENTS_PORT = 0;

        // Wakeup from sleep when a key is pressed
        nrf_drv_rtc_enable(&rtc_debounce);
    }
}

/*****************************************************************************/
/** Gazell callback function definitions  */
/*****************************************************************************/

void  nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    // We do not use ack payload
    nrf_gzll_flush_rx_fifo(pipe);

    // Clear maintenance flag if transmission succeeded
    need_maintenance = false;
}

void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    // If data transmission failed, set maitenance flag
    need_maintenance = true;
}

void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
}

void nrf_gzll_disabled()
{
}
