//
// Created by hhw on 2025/1/20.
//
#pragma once
#ifndef REMOTER_MYHAL_H
#define REMOTER_MYHAL_H

#include "hal.h"



    class MyHAL : public HAL {
    private:
        void _stm32_hal_init();
        void _sys_clock_init();
        void _gpio_init();
        void _dma_init();
        void _timer_init();
        void _spi_init();
        void _adc_init();

        void _ssd1306_init();
        void _key_init();
        void _buzzer_init();
        void _u8g2_init();

    public:
        inline void init() override {
            _stm32_hal_init();
            _sys_clock_init();
            _gpio_init();
            _dma_init();
            _timer_init();
            _spi_init();
            _adc_init();

            _ssd1306_init();
            _key_init();
            _buzzer_init();
            _u8g2_init();
        }
    };
};
#endif //REMOTER_MYHAL_H
