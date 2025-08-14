/*
 * M_USART.h
 *
 *  Created on: May 19, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_M_USART_H_
#define INC_M_USART_H_

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart, string &pData, uint32_t Timeout) {
	uint8_t *pdata8bits;
	uint16_t *pdata16bits;
	uint16_t uhMask;
	uint32_t tickstart;
	UNUSED(pdata16bits);
	/* Check that a Rx process is not already ongoing */
	if (huart->RxState == HAL_UART_STATE_READY) {
//		if (&pData == NULL) {
//			return HAL_ERROR;
//		}

		__HAL_LOCK(huart);

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;
		huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

		/* Init tickstart for timeout management */
		tickstart = HAL_GetTick();

		huart->RxXferSize = UINT16_MAX - 1;
		huart->RxXferCount = UINT16_MAX - 1;

		/* Computation of UART mask to apply to RDR register */
		UART_MASK_COMPUTATION(huart);
		uhMask = huart->Mask;

		/* In case of 9bits/No Parity transfer, pRxData needs to be handled as a uint16_t pointer */
		if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE)) {
			pdata8bits = NULL;
//			pdata16bits = (uint16_t*) pData->c_str();
			pdata16bits = (uint16_t*) pData.c_str();
		} else {
			pdata8bits = (uint8_t*) pData.c_str();
//			pdata8bits = (uint8_t*) pData->c_str();
			pdata16bits = NULL;
		}

		__HAL_UNLOCK(huart);

		/* Initialize output number of received elements */
//		uint16_t times_idle = 3;
		/* as long as data have to be received */
		while (huart->RxXferCount > 0U) {
			/* Check if IDLE flag is set */
			if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
				/* Clear IDLE flag in ISR */
				__HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);

				/* If Set, but no data ever received, clear flag without exiting loop */
				/* If Set, and data has already been received, this means Idle Event is valid : End reception */
				if (pData.size() > 0U) {
					huart->RxState = HAL_UART_STATE_READY;
//					times_idle--;
//					if (times_idle == 0) {
					return HAL_OK;
//					}
				}
			}

			/* Check if RXNE flag is set */
			if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE)) {
				if (pdata8bits == NULL) {
					char ch = (huart->Instance->RDR & uhMask);
//					pData.push_back(ch);

					pData += ch;
				} else {
					char ch = (huart->Instance->RDR & (uint8_t) uhMask);
					pData += ch;
//					pData.push_back(ch);
				}
				huart->RxXferCount--;
			}

			/* Check for the Timeout */
			if (Timeout != HAL_MAX_DELAY) {
				if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U)) {
					huart->RxState = HAL_UART_STATE_READY;

					return HAL_TIMEOUT;
				}
			}
		}

		/* At end of Rx process, restore huart->RxState to Ready */
		huart->RxState = HAL_UART_STATE_READY;

		return HAL_OK;
	} else {
		return HAL_BUSY;
	}
}

void UART_Init(UART_HandleTypeDef *uartHandle, USART_TypeDef *usart_define, uint32_t manual_baud) {

	uartHandle->Instance = usart_define;
	uartHandle->Init.BaudRate = manual_baud;
	uartHandle->Init.WordLength = UART_WORDLENGTH_8B;
	uartHandle->Init.StopBits = UART_STOPBITS_1;
	uartHandle->Init.Parity = UART_PARITY_NONE;
	uartHandle->Init.Mode = UART_MODE_TX_RX;
	uartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(uartHandle) != HAL_OK) {
		Error_Handler();
	}

}

#if !defined(MOHIT)
void ADC1_CHANNEL_INIT(uint32_t channel) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	sConfig.Channel = channel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}
#endif

#endif /* INC_M_USART_H_ */
