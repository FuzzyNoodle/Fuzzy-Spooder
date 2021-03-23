/*!
 *  @file Weight_Estimator.h
 *
 *  
 *
 *  
 */

#ifndef WEIGHT_ESTIMATOR_H
#define WEIGHT_ESTIMATOR_H

#include "Arduino.h"

/* Uncomment to enable printing out nice debug messages. */
#define DHT_DEBUG

#define DEBUG_PRINTER                                                          \
  Serial /**< Define where debug output will be printed.                       \
          */

/* Setup debug printing macros. */
#ifdef DHT_DEBUG
#define DEBUG_PRINT(...)                                                       \
  { DEBUG_PRINTER.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...)                                                     \
  { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...)                                                       \
  {} /**< Debug Print Placeholder if Debug is disabled */
#define DEBUG_PRINTLN(...)                                                     \
  {} /**< Debug Print Line Placeholder if Debug is disabled */
#endif


/*!
 *  @brief  Class that stores state and functions for WEIGHT_ESTIMATOR
 */
class WEIGHT_ESTIMATOR {
public:
  WEIGHT_ESTIMATOR(void);
  void begin(void);
  
private:
  
};

#endif  //#ifndef WEIGHT_ESTIMATOR_H