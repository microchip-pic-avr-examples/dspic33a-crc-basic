/**
 * CRC Generated Driver Interface Header File
 * 
 * @file      crc_interface.h
 *            
 * @defgroup  crcdriver CRC Driver
 *            
 * @brief     32-bit Programmable Cyclic Redundancy Check generator using dsPIC MCUs
 *
 * @skipline @version   PLIB Version 1.0.1
 *            
 * @skipline  Device : dsPIC33AK128MC106
*/

/*disclaimer*/

#ifndef CRC_INTERFACE_H
#define CRC_INTERFACE_H

// Section: Included Files

#include <stdbool.h>
#include "crc_types.h"

// Section: Data Type Definitions
        
/**
 @ingroup  crcdriver
 @struct   CRC_INTERFACE
 @brief    Structure containing the function pointers of CRC driver
*/

struct CRC_INTERFACE
{
    void (*Initialize)(void);
    ///< Pointer to \ref CRC_Initialize
    
    void (*Deinitialize)(void);
    ///< Pointer to \ref CRC_Deinitialize
    
    void (*SeedSet)(uint32_t seed, enum CRC_SEED_METHOD seedMethod, enum CRC_SEED_DIRECTION seedDirection);
    ///< Pointer to \ref CRC_SeedSet
    
    void (*CalculateBufferStart)(void *buffer, uint32_t sizeBytes);
    ///< Pointer to \ref CRC_CalculateBufferStart
    
    void (*CalculateProgramStart)(uint32_t startAddr, uint32_t sizeBytes);
    ///< Pointer to \ref CRC_CalculateProgramStart
    
    bool (*CalculationIsDone)(void);
    ///< Pointer to \ref CRC_CalculationIsDone
    
    uint32_t (*CalculationResultGet)(bool reverse, uint32_t xorValue);
    ///< Pointer to \ref CRC_CalculationResultGet
    
    uint32_t (*CalculationResultRawGet)(void);
    ///< Pointer to \ref CRC_CalculationResultRawGet
    
    uint32_t (*CalculationResultReverseGet)(void);
    ///< Pointer to \ref CRC_CalculationResultReverseGet   
    
    uint32_t (*CalculationResultXORGet)(uint32_t xorValue);
    ///< Pointer to \ref CRC_CalculationResultXORGet
    
    void (*EventCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to \ref CRC_EventCallbackRegister
    
    void (*Tasks)(void);
    ///< Pointer to \ref CRC_Tasks (Supported only in polling mode)
};

#endif  //CRC_INTERFACE_H
/**
 End of File
*/
