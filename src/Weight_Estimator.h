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


class WEIGHT_ESTIMATOR 
{
public:
  WEIGHT_ESTIMATOR(void);
  void begin(void);
  void update(void);
private:
  uint32_t blink_timer;
  const uint32_t BLINK_PEROID = 200;
};

#endif  //#ifndef WEIGHT_ESTIMATOR_H