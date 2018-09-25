#ifndef __IDENTIFICATION_H
#define __IDENTIFICATION_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup _ID_Macros
 * @brief    Library defines
 *
 * Defines for register locations inside STM32F103 devices
 * @{
 */

/**
 * @brief Unique ID register address location
 */
#define ID_UNIQUE_ADDRESS		0x1FFFF7E8

/**
 * @brief Flash size register address
 */
#define ID_FLASH_ADDRESS		0x1FFFF7E0

/**
 * @brief  Get STM32F103 device's flash size in kilo bytes
 * @note   Defined as macro to get maximal response time
 * @param  None
 * @retval Flash size in kilo bytes
 */
#define ID_GetFlashSize()	(*(uint16_t *) (ID_FLASH_ADDRESS))

/**
 * @brief  Get unique ID number in 8-bit format
 * @note   STM32F103 has 96bits long unique ID, so 12 bytes are available for read in 8-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 8 bits you want to read
 *               - Values between 0 and 11 are allowed
 * @retval Unique ID address
 */
#define ID_GetUnique8(x)		((x >= 0 && x < 12) ? (*(uint8_t *) (ID_UNIQUE_ADDRESS + (x))) : 0)

/**
 * @brief  Get unique ID number in 16-bit format
 * @note   STM32F103 has 96bits long unique ID, so 6 2-bytes values are available for read in 16-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 16 bits you want to read
 *               - Values between 0 and 5 are allowed
 * @retval Unique ID address
 */
#define ID_GetUnique16(x)	((x >= 0 && x < 6) ? (*(uint16_t *) (ID_UNIQUE_ADDRESS + 2 * (x))) : 0)

/**
 * @brief  Get unique ID number in 32-bit format
 * @note   STM32F103 has 96bits long unique ID, so 3 4-bytes values are available for read in 32-bit format
 * @note   Defined as macro to get maximal response time
 * @param  x: Byte number: specify which part of 16 bits you want to read
 *               - Values between 0 and 2 are allowed
 * @retval Unique ID address
 */
#define ID_GetUnique32(x)	((x >= 0 && x < 3) ? (*(uint32_t *) (ID_UNIQUE_ADDRESS + 4 * (x))) : 0)

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif	/* __IDENTIFICATION_H */
