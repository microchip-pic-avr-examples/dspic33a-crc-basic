/**
 * CRC Generated Driver Types Header File
 * 
 * @file      crc_types.h
 *            
 * @ingroup   crcdriver
 *            
 * @brief     This is the generated driver types header file for the CRC driver
 *
 * @skipline @version   PLIB Version 1.0.1
 *            
 * @skipline  Device : dsPIC33AK128MC106
*/

/*disclaimer*/

#ifndef CRC_TYPES_H
#define CRC_TYPES_H

// Section: Enum Declarations

/**
 @ingroup  crcdriver
 @enum     CRC_STATE
 @brief    Defines the CRC calculation states
*/
enum CRC_STATE
{
    CRC_STATE_CALCULATE,   /**< CRC calculation start */
    CRC_STATE_FLUSH,       /**< CRC calculation data flush */
    CRC_STATE_CLEANUP,     /**< CRC calculation reset */
    CRC_STATE_DONE         /**< CRC calculation complete */
};

/**
 @ingroup  crcdriver
 @enum     CRC_SEED_METHOD
 @brief    Defines the CRC calculation seed method \ref CRC_SeedSet. 
           The direct method refers to the seed being placed before the shifters with 
           the result being shifted through the polynomial. The indirect method refers 
           to the seed being placed after the shifters with the result not being shifted 
           through the polynomial.
*/
enum CRC_SEED_METHOD
{
    CRC_SEED_METHOD_DIRECT,     /**< CRC seed in direct method */
    CRC_SEED_METHOD_INDIRECT    /**< CRC seed in indirect method */
};

/**
 @ingroup  crcdriver
 @enum     CRC_SEED_DIRECTION
 @brief    Defines the CRC calculation seed direction in direct method \ref CRC_SeedSet
*/
enum CRC_SEED_DIRECTION
{
    CRC_SEED_DIRECTION_Msb,     /**< CRC seed in Big Endian */
    CRC_SEED_DIRECTION_Lsb,     /**< CRC seed in Little Endian */
};

#endif  //CRC_TYPES_H
/**
 End of File
*/
