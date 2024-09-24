/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/crc/crc.h"
#include <stdbool.h>
#include <stdint.h>
/*  cppcheck-suppress misra-c2012-21.6
 *  The use of stdio.h functions does not follow the MISRA 2012 specification.
 * 
 *  (Rule 21.6) REQUIRED: The standard library input/output functions shall not be used
 * 
 *  Reasoning: printf is required for printing to the terminal in this code example
 */
#include <stdio.h>

/*
    Main application
*/

static uint16_t calculateHardwareCRC(uint8_t data[], uint8_t dataLength)
{
    /*  
        uses the CRC-16-CCITT polynomial and its default settings in MCC Melody
        CRC Driver version 1.0.4
    */
    
    // starts the CRC calculation
    CRC_CalculateBufferStart(data, dataLength);                       
    
    // active until calculation is done
    while(CRC_CalculationIsDone() == false) {};
    
    // obtains the calculation result from the registers, do not reverse and XOR = 0
    return CRC_CalculationResultGet(false, 0); 
}

static uint16_t calculateSoftwareCRC(uint8_t const data[], uint8_t dataLength)
{
    uint16_t result = 0xFFFFu;
    uint16_t polynomial = 0x1021u;
    
    uint8_t dataWidth = 8;
    for(uint8_t word = 0; word < dataLength; word++)
    {
        result ^= (data[word] << 8);
        for(uint8_t count = 0; count < dataWidth; count++)
        {
            if(result >= 0x8000u)
            {
                result = (result << 1) ^ polynomial;
            }
            else
            {
                result = (result << 1);
            }
        }
    }
    
    return result;
}

int main(void)
{
    SYSTEM_Initialize();
    
    // data for which the CRC result will be calculated
    uint8_t buffer[2] = {0x6C, 0x93};
    uint8_t bufferLength = sizeof(buffer) / sizeof(buffer[0]);  
    
    uint16_t hardwareResult;
    uint16_t softwareResult;
    
    (void) printf("\r\nCRC Basic Code Example Results\r\n");
    
    hardwareResult = calculateHardwareCRC(buffer, bufferLength);          
    
    // prints the hardware CRC result in hexadecimal
    (void) printf("\r\nHardware CRC Result: 0x%04X\r\n", (unsigned short)hardwareResult);
    
    softwareResult = calculateSoftwareCRC(buffer, bufferLength);
    
    // prints the software CRC result in hexadecimal
    (void) printf("Software CRC Result: 0x%04X\r\n", (unsigned short)softwareResult);
    
    if (hardwareResult == softwareResult) 
    {
        (void) printf("\r\nSuccess\r\n");
    }
    else 
    { 
        (void) printf("\r\nError\r\n");
    }
    
    while(1) {};
}