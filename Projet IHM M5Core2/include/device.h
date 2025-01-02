#ifndef DEVICE_H
#define DEVICE_H

#include <M5Core2.h>
#include <string>
#include <vector>

// Structure représentant un appareil
struct Device {
    int idDevice;
    int idTypeDevice;
    std::string uid;
    std::string name;
    unsigned long timestamp;

    void reset() {
        idDevice = 0;
        idTypeDevice = 0;
        uid.clear();
        name.clear();
        timestamp = 0;
    }

    bool isReset() const {
        return idDevice == 0 &&
               idTypeDevice == 0 &&
               uid.empty() &&
               name.empty() &&
               timestamp == 0;
    }
};

// Variables globales
extern int currentDeviceIndex;
extern int currentNewDeviceIndex;
extern std::vector<Device> devices;
extern std::vector<Device> new_devices;
extern String buffer;

extern QueueHandle_t queueDevice;

// Prototypes de tâches

void taskValidate(void* parameter);

// Prototypes des fonctions

bool parseDATA(const char *json);
bool parseDevice(const char *deviceData, Device &device);
bool extractJsonValue(const char *deviceData, const char *key, const char *type, void *data);

void handleAddCommand(const char * extractData);

#endif // DEVICE_H
