/**
 * CRC Generated Driver Source File
 * 
 * @file      crc.h
 *            
 * @ingroup   crcdriver
 *            
 * @brief     This is the generated driver source file for the CRC driver
 *
 * @skipline @version   PLIB Version 1.0.1
 *            
 * @skipline  Device : dsPIC33AK128MC106
*/

/*disclaimer*/

// Section: Included Files

#include <stddef.h>
#include "../crc.h"

// Section: File specific functions

static void (*CRC_EventHandler)(void) = NULL;
// Section: Driver Interface

const struct CRC_INTERFACE CRC = {
    .Initialize = &CRC_Initialize,
    .Deinitialize = &CRC_Deinitialize,
    .SeedSet = &CRC_SeedSet,
    .CalculateBufferStart = &CRC_CalculateBufferStart,
    .CalculateProgramStart = &CRC_CalculateProgramStart,
    .CalculationIsDone = &CRC_CalculationIsDone,
    .CalculationResultGet = &CRC_CalculationResultGet,
    .CalculationResultRawGet = &CRC_CalculationResultRawGet,
    .CalculationResultReverseGet = &CRC_CalculationResultReverseGet,
    .CalculationResultXORGet = &CRC_CalculationResultXORGet,
    .EventCallbackRegister = &CRC_EventCallbackRegister,
    .Tasks = NULL
};
// Section: Structure Declarations

struct CRC_OBJ
{
    uint8_t dataWidth;                              /**< Set the CRC data width */
    uint8_t polyWidth;                              /**< Set the CRC polynomial width */
    enum CRC_STATE state;       /**< Defines the CRC calculation state */
    bool program;                                   /**< Defines the Program space CRC start status */
    uint32_t remainingSize;                         /**< Defines the CRC calculation buffer remaining size */
    union
    {
        uint8_t *data8bit;                          /**< Set the CRC data 8bit pointer */
        uint16_t *data16bit;                        /**< Set the CRC data 16bit pointer */
        uint32_t *data32bit;                        /**< Set the CRC data 32bit pointer */
        void *data;                                 /**< Set the CRC data void pointer */
        uint32_t program;                           /**< Set the CRC program start address */
    } ptr;
};

// Section: Private Data
 
static struct CRC_OBJ crcObj;

void CRC_Initialize(void)
{
    // reset the module
    CRCCON = 0;
    CRCXOR = 0;
    CRCWDAT = 0;

    // initials the module - it will not be enabled until the end of the function
    // MOD Legacy; LENDIAN Start with MSb; CRCGO CRC is turned off; CRCISEL Interrupt on shift complete and results ready; SIDL disabled; ON disabled; PLEN 16; DWIDTH 8; 
    CRCCON = 0x70F0040UL;
    
    CRC_EventCallbackRegister(&CRC_EventCallback);
    
    // enable module
    CRCCONbits.ON = 1;

    // some polynomial
    CRCXOR = 0x1021UL;
    
    // some seed
    CRCWDAT = 0x84CFUL;

    // set module state
    crcObj.remainingSize = 0;
    crcObj.state = CRC_STATE_CALCULATE;
	
	crcObj.polyWidth = CRCCONbits.PLEN + 1;
}

void CRC_Deinitialize(void)
{
    IFS4bits.CRCIF = 0;
    IEC4bits.CRCIE = 0;
    
    CRCCON = 0x40;
    CRCXOR = 0x0;
    CRCWDAT = 0x0;
}

// Section: Private CRC Driver Functions
 
static bool CRC_ProgramTask(void)
{
    bool status = false;
	uint32_t *programFlashAddress = NULL;
	uint32_t *flash_data_value = NULL;

    CRCCONbits.CRCGO = false;
    IFS4bits.CRCIF = false;

    while((!CRCCONbits.CRCFUL) && (crcObj.remainingSize))
    {
		programFlashAddress = (uint32_t*) crcObj.ptr.program;
		flash_data_value = (uint32_t*) *(programFlashAddress);
		CRCDAT = (uint32_t) flash_data_value;

        crcObj.ptr.program += 4U;
        crcObj.remainingSize -= (uint32_t)4U;
    }

    CRCCONbits.CRCGO = true;
    
    if(crcObj.remainingSize == (uint32_t)0) 
    {
        status = true;
    }

    return status;
}

static bool CRC_BufferTask(void)
{
    bool status = false;

    if(CRCCONbits.CRCFUL != 0U)
    {
        status = false;
    }
    else
    {
        IFS4bits.CRCIF = false;

        if(crcObj.dataWidth <= (uint8_t)8)
        {
            while((!CRCCONbits.CRCFUL) && (crcObj.remainingSize))
            {
                *((uint8_t *)&CRCDAT) = *crcObj.ptr.data8bit;
                crcObj.ptr.data8bit++;
                crcObj.remainingSize--;
            }
        } 
        else if(crcObj.dataWidth <= (uint8_t)16)
        {
            while((!CRCCONbits.CRCFUL) && (crcObj.remainingSize))
            {
                *((uint16_t *)&CRCDAT) = *crcObj.ptr.data16bit;
                crcObj.ptr.data16bit++;
                crcObj.remainingSize -= (uint32_t)2;
            }
        } 
        else
        {
            while((!CRCCONbits.CRCFUL) && (crcObj.remainingSize))
            {
                CRCDAT = *crcObj.ptr.data32bit;
                crcObj.ptr.data32bit++;
                crcObj.remainingSize -= (uint32_t)4;
            }
        }

        CRCCONbits.CRCGO = true;

        if(crcObj.remainingSize == (uint32_t)0)
        {
            status = true;
        }
    }

    return status;
}

static bool CRC_FlushTask(void)
{
    bool status = false;

    if(IFS4bits.CRCIF != 0U)
    {

        CRCCONbits.CRCGO = false;
        IFS4bits.CRCIF = false;

        CRCCONbits.DWIDTH = crcObj.polyWidth - (uint8_t)1;
        
        if(crcObj.polyWidth <= (uint8_t)8)
        {
            *((uint8_t *)&CRCDAT) = 0;
        }
        else if(crcObj.polyWidth <= (uint8_t)16)
        {
            *((uint16_t *)&CRCDAT) = 0;
        }
        else
        {
            CRCDAT = 0;
        }

        CRCCONbits.CRCGO = true;
        
        status = true;
    }

    return status;
}

static bool CRC_CleanUpTask(void)
{
    bool status = false;

    if(IFS4bits.CRCIF != 0U)
    {

        CRCCONbits.DWIDTH = crcObj.dataWidth - (uint8_t)1;
        CRCCONbits.CRCGO = false;
        IFS4bits.CRCIF = false;

        status = true;
    }
    
    return status;
}

static uint32_t CRC_ReverseValue(uint32_t crc)
{
    uint32_t mask;
    uint32_t reverse;
    uint32_t crctemp = crc;

    mask = 1;
    mask <<= (crcObj.polyWidth - (uint8_t)1);
    reverse = 0;

    while(crctemp  != 0U)
    {
        if((crctemp & (uint32_t)0x01) != 0U)
        {
            reverse |= mask;

        }
            
        mask >>= 1;
        crctemp >>= 1;
    }

    return reverse;
}

static uint32_t CRC_PolynomialMask(void)
{
    uint32_t mask = 0;
    uint32_t idx;

    for(idx = 0; idx < crcObj.polyWidth; idx++)
    {
        mask <<= 1;
        mask |= 1;
    }

    return mask;
}

// Section: CRC Module APIs

void CRC_SeedSet(uint32_t seed, enum CRC_SEED_METHOD seedMethod, enum CRC_SEED_DIRECTION seedDirection)
{
    uint8_t direction = CRCCONbits.LENDIAN;
    uint8_t dataWidth = CRCCONbits.DWIDTH;
    bool enable = IEC4bits.CRCIE;

    IEC4bits.CRCIE = false;

    if(seedMethod == CRC_SEED_METHOD_INDIRECT)
    {
        CRCWDAT = seed;
    }
    else
    {

        CRCCONbits.DWIDTH = CRCCONbits.PLEN; 
        CRCCONbits.LENDIAN = seedDirection;

        if(crcObj.polyWidth <= (uint8_t)8)
        {
            *((uint8_t *)&CRCDAT) = (uint8_t)seed;
        }
        else if(crcObj.polyWidth <= (uint8_t)16)
        {
             *((uint16_t *)&CRCDAT) = (uint16_t)seed;
        }
        else
        {
            CRCDAT = (uint32_t)seed;
        }

        // Run the seed through the shift register
        IFS4bits.CRCIF = false;

        CRCCONbits.CRCGO = true;
        while(1){
            if(IFS4bits.CRCIF == true){
                break;
            }

        }
        
        CRCCONbits.CRCGO = false;

        CRCCONbits.LENDIAN = direction;
        IFS4bits.CRCIF = false;
        CRCCONbits.DWIDTH = dataWidth; 
    }
    
    IEC4bits.CRCIE = enable;
}

void CRC_CalculateBufferStart(void *buffer, uint32_t sizeBytes)
{
    crcObj.dataWidth = CRCCONbits.DWIDTH + 1;
    crcObj.polyWidth = CRCCONbits.PLEN + 1;
    crcObj.remainingSize = sizeBytes;
    crcObj.ptr.data = buffer;
    crcObj.state = CRC_STATE_CALCULATE;
    crcObj.program = false;

    CRCCONbits.CRCGO = true;
    IFS4bits.CRCIF = true;
    IEC4bits.CRCIE = true;
}

void CRC_CalculateProgramStart(uint32_t startAddr, uint32_t sizeBytes)
{
    crcObj.dataWidth = CRCCONbits.DWIDTH + 1;
    crcObj.polyWidth = CRCCONbits.PLEN + 1;
    crcObj.remainingSize = sizeBytes;
	
	uint32_t alignedAddrOffset = startAddr % 4U;  
	if(alignedAddrOffset == 0U) 
	{
		crcObj.ptr.program = startAddr; 
	}
	else
	{
		crcObj.ptr.program = startAddr - alignedAddrOffset;
	}
	
    crcObj.state = CRC_STATE_CALCULATE;
    crcObj.program = true;
    
    CRCCONbits.DWIDTH = 32 - 1;

    CRCCONbits.CRCGO = true;
    IFS4bits.CRCIF = true;
    IEC4bits.CRCIE = true;
}

void CRC_EventCallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        CRC_EventHandler = handler;
    }
}

void __attribute__ ((weak)) CRC_EventCallback ( void )
{ 

} 


bool CRC_CalculationIsDone(void)
{
    return crcObj.state == CRC_STATE_DONE;
}

uint32_t CRC_CalculationResultRawGet(void)
{
    uint32_t result = 0;

    if(crcObj.polyWidth <= (uint8_t)8)
    {
        result = (uint32_t)CRCWDAT & (uint32_t)0xFFUL;
    }
    else if(crcObj.polyWidth <= (uint8_t)16)
    {
        result = (uint32_t)CRCWDAT & (uint32_t)0xFFFFUL;
    }
    else
    {
        result |= (uint32_t)CRCWDAT;
    }
    
    return result;
}

uint32_t CRC_CalculationResultReverseGet(void)
{
    uint32_t result;

    result = CRC_CalculationResultRawGet();

    return CRC_ReverseValue(result);
}

uint32_t CRC_CalculationResultXORGet(uint32_t xorValue)
{
    uint32_t result;

    result = CRC_CalculationResultRawGet();

    result ^= xorValue;

    return result & CRC_PolynomialMask();
}

uint32_t CRC_CalculationResultGet(bool reverse, uint32_t xorValue)
{
    uint32_t result;

    if(reverse) 
    {
        result = CRC_CalculationResultReverseGet();
    }
    else
    {
        result = CRC_CalculationResultRawGet();
    }

    result ^= xorValue;

    return result & CRC_PolynomialMask();
}

void __attribute__ ((interrupt, no_auto_psv)) _CRCInterrupt(void)
{
    switch(crcObj.state)
    {
        case CRC_STATE_CALCULATE:
            if(crcObj.program)
            {
                if(CRC_ProgramTask()) 
                {
                    crcObj.state = CRC_STATE_FLUSH;
                }
            }
            else
            {
                if(CRC_BufferTask())
                {
                    crcObj.state = CRC_STATE_FLUSH;
                }
            }    
            break;

        case CRC_STATE_FLUSH:
            if(CRC_FlushTask())
            {
                crcObj.state = CRC_STATE_CLEANUP;
            }
            break;

        case CRC_STATE_CLEANUP:
            if(CRC_CleanUpTask())
            {
                crcObj.state = CRC_STATE_DONE;
            }
            break;

        case CRC_STATE_DONE:
            IEC4bits.CRCIE = false;
            break;
        
        default:
            CRCCON = 0;
            break;
    }

    if(IFS4bits.CRCIF != 0U)
    {
        if(NULL != CRC_EventHandler)
        {
            (*CRC_EventHandler)();
        }
    }
}

/**
 End of File
*/