/**
 * @file main.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MAIN_H
#define MAIN_H

#define VERSION_CTRL_FW 001

// language libraries (C).
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// rtos libraries (FreeRTOS).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Framework Libraries (ESP-IDF).
#include "driver/rmt.h"

// Project libraries (climaQ).
// #include "Mqtt_Control/MqttCtrl.h"
// #include "Periph_Control/LedCtrl.h"
#include "Periph_Control/ButtonCtrl.h"

#endif