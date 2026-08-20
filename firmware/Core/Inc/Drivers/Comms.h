#ifndef COMMS_H
#define COMMS_H

typedef enum {
  IMU, BMP
} SENSOR;

void Comms_Init(SENSOR sensor);

#endif