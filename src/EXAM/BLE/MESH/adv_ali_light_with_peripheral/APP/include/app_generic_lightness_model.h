/********************************** (C) COPYRIGHT *******************************
 * File Name          : app_generic_lightness_model.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef app_generic_lightness_model_H
#define app_generic_lightness_model_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MESH_LIB.h"

/**
 * @brief   �洢�յ�������������Ӧ�������Ľṹ������
 */
extern const struct bt_mesh_model_op gen_lightness_op[];

/**
 * @brief   ��ȡ��ǰ������
 *
 * @param   led_pin     - LED����.
 *
 * @return  ����
 */
uint16_t read_led_lightness(uint32_t led_pin);

/**
 * @brief   ���õ�ǰ������
 *
 * @param   led_pin     - LED����.
 * @param   lightness   - ����.
 */
void set_led_lightness(uint32_t led_pin, uint16_t lightness);

#ifdef __cplusplus
}
#endif

#endif
