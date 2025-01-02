#ifndef SERIAL_H
#define SERIAL_H

#include "device.h"
#include "project.h"

#define TRAME_SIZE 256

void SerialSetup();
void taskSerialRead(void *pvParameters);

#endif // SERIAL_H
