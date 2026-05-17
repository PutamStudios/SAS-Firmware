#ifndef SAS_BLE_SERVICE_H
#define SAS_BLE_SERVICE_H
#if !defined(LITE_VERSION)
#include <NimBLEServer.h>

class SASBLEService {
protected:
    NimBLEService *pService = nullptr;
    uint16_t mtu = 23; // default MTU size
public:
    virtual ~SASBLEService() = default;
    virtual void setup(NimBLEServer *pServer) = 0;
    virtual void end() = 0;
    void setMTU(int new_mtu) { mtu = new_mtu; }
};
#endif
#endif // SAS_BLE_SERVICE_H
