#ifndef DEVICE_H
#define DEVICE_H

bool parseDATA(const char *json);
bool extractJsonValue(const char *deviceData, const char *key, const char *type, void *data);

#endif // DEVICE_H