/**
 * @file application.c
 * @brief Firmware pro monitorování teploty v inkubátoru (IoT uzel).
 * @author Pavel Černý, Barbora Šimková, Štěpán Hruška, Tereza Holubcová, Václav Haba
 * @version 1.0
 * * Tento kód inicializuje teploměr TMP112 a odesílá naměřená data
 * v pravidelných intervalech přes rádiový modul do brány (Gateway).
 */

#include <application.h>

/** @brief Instance LED ovladače pro vizuální signalizaci */
twr_led_t led;

/** @brief Instance ovladače teploměru TMP112 */
twr_tmp112_t tmp112;

/**
 * @brief Callback funkce, která obsluhuje události z teploměru.
 * * @param self Ukazatel na instanci teploměru, která událost vyvolala.
 * @param event Typ události (např. aktualizace dat).
 * @param event_param Volitelný parametr předaný při inicializaci.
 */
void tmp112_event_handler(twr_tmp112_t *self, twr_tmp112_event_t event, void *event_param)
{
    // Reagujeme pouze na událost aktualizace naměřených dat
    if (event == TWR_TMP112_EVENT_UPDATE)
    {
        float celsius;
        
        // Přečtení aktuální teploty v stupních Celsia
        if (twr_tmp112_get_temperature_celsius(self, &celsius))
        {
            // Výpis hodnoty do ladicí konzole (Debugger)
            twr_log_debug("APP: Teplota naměřena: %.2f °C", celsius);

            // Odeslání hodnoty rádiem na zadaném kanálu
            twr_radio_pub_temperature(TWR_RADIO_PUB_CHANNEL_R1_I2C0_ADDRESS_ALTERNATE, &celsius);
            
            // Krátké bliknutí LED pro indikaci úspěšného odeslání dat
            twr_led_pulse(&led, 50);
        }
    }
}

/**
 * @brief Hlavní inicializační funkce aplikace. Volá se jednou po startu.
 * Nastavuje hardware a plánuje první úlohy.
 */
void application_init(void)
{
    // Inicializace subsystému pro logování (výpisy do konzole)
    twr_log_init(TWR_LOG_LEVEL_DUMP, TWR_LOG_TIMESTAMP_ABS);

    // Konfigurace systémové LED diody (výchozí stav vypnuto)
    twr_led_init(&led, TWR_GPIO_LED, false, 0);
    // Úvodní bliknutí signalizující start zařízení
    twr_led_pulse(&led, 2000);

    // Inicializace senzoru TMP112 na sběrnici I2C0 s adresou 0x49
    twr_tmp112_init(&tmp112, TWR_I2C_I2C0, 0x49);
    // Registrace obslužné funkce pro události ze senzoru
    twr_tmp112_set_event_handler(&tmp112, tmp112_event_handler, NULL);
    
    // Nastavení intervalu měření a odesílání na 10 000 ms (10 sekund)
    twr_tmp112_set_update_interval(&tmp112, 10000);

    // Aktivace rádiového přenosu v energeticky úsporném režimu
    twr_radio_init(TWR_RADIO_MODE_NODE_SLEEPING);
    
    // Odeslání párovacího požadavku se jménem projektu
    twr_radio_pairing_request("inkubator-projekt", FW_VERSION);
}

/**
 * @brief Periodicky volaná funkce pro naplánované úlohy.
 * V tomto projektu se nevyužívá, protože veškerou logiku řídí události (events).
 */
void application_task(void)
{
    // Ponecháno prázdné pro budoucí rozšíření
}