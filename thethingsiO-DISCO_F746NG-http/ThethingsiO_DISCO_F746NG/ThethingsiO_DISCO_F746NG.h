#ifndef __ThethingsiO_DISCO_F746NG_H 
#define __ThethingsiO_DISCO_F746NG_H 

#ifdef TARGET_DISCO_F746NG


#include <iostream>
#include <string>
#include <sstream>
#include "mbed.h"
#include "EthernetInterface.h"
#include "NetworkAPI/buffer.hpp"
#include "NetworkAPI/ip/address.hpp"
#include "NetworkAPI/tcp/socket.hpp"


class ThethingsiO_DISCO_F746NG
{
public:

    ThethingsiO_DISCO_F746NG();
    
    ThethingsiO_DISCO_F746NG(string token);
    
    ~ThethingsiO_DISCO_F746NG();

    string thingReadAll(int limit);

    string thingRead(string resource, int limit);

    int thingWrite(string resource, string value);

    string thingActivate(string activationCode);

    string getToken();

    void setToken(string token);

private:

    string thingToken;
    EthernetInterface interface;
    network::tcp::Socket socket;
    network::Buffer buffer;
};

#else
#error "This class must be used with DISCO_F746NG board only."
#endif // TARGET_DISCO_F746NG

#endif
