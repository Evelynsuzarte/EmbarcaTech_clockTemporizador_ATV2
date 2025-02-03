#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define LED_VM 11
#define LED_AZ 12
#define LED_VD 13
#define BOTAO 5

volatile bool botao_pressionado = false;
volatile int estado = 0;

void callback_botao(uint gpio, uint32_t events);
int64_t apaga_leds(alarm_id_t id, void *user_data);

int main()
{
    stdio_init_all();

    gpio_init(LED_VM);
    gpio_set_dir(LED_VM, GPIO_OUT);
    gpio_init(LED_AZ);
    gpio_set_dir(LED_AZ, GPIO_OUT);
    gpio_init(LED_VD);
    gpio_set_dir(LED_VD, GPIO_OUT);
    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO);  

    struct repeating_timer timer; 

    gpio_set_irq_enabled_with_callback(BOTAO, GPIO_IRQ_EDGE_RISE, true, &callback_botao);

    while (true) {
        if (botao_pressionado && estado == 0) {
            gpio_put(LED_VM, 1);
            gpio_put(LED_AZ, 1);
            gpio_put(LED_VD, 1);
            estado = 3;
            printf("todas leds\n");

            add_alarm_in_ms(3000, apaga_leds, NULL, false);

            botao_pressionado = false;  
        }
    }

}
    


// controle de bouncing do botao
void callback_botao(uint gpio, uint32_t events){
    if (gpio == BOTAO) {
        sleep_ms(50);
        }if (gpio == BOTAO) {
            botao_pressionado = true;
            printf("botao apertado\n");
    }
}

//apaga as leds e espera o tempo para a próxima
int64_t apaga_leds(alarm_id_t id, void *user_data) {
    if (estado == 3) {
        gpio_put(LED_VM, 0);
        estado = 2;
        printf("LED azul apagado\n");
        add_alarm_in_ms(3000, apaga_leds, NULL, false);
    } else if (estado == 2) {
        gpio_put(LED_AZ, 0);
        estado = 1;
        add_alarm_in_ms(3000, apaga_leds, NULL, false);
        printf("LED vermelho apagado\n");
    } else if (estado == 1) {
        gpio_put(LED_VD, 0);
        estado = 0;
        printf("todos apagados\n");
    }
    return 0;
}
