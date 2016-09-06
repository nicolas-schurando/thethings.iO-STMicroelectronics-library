#include "ThethingsiO_DISCO_F746NG.h"

ThethingsiO_DISCO_F746NG::ThethingsiO_DISCO_F746NG()
{
    this->interface.init();
    this->interface.connect();
    this->buffer = network::Buffer(1024);
}

ThethingsiO_DISCO_F746NG::ThethingsiO_DISCO_F746NG(string token)
{
    this->interface.init();
    this->interface.connect();
    this->thingToken = token;
    this->buffer = network::Buffer(1024);
}

ThethingsiO_DISCO_F746NG::~ThethingsiO_DISCO_F746NG()
{

}

string ThethingsiO_DISCO_F746NG::thingReadAll(int limit)
{
    std::string request("GET /v2/things/");
    request.append(thingToken);
    request.append("/all_resources?limit=");
    stringstream convert;
    convert << limit;
    request.append(convert.str());
    request.append(" HTTP/1.1\r\nHost: %s\r\nUser-Agent: curl/7.43.0\r\nAccept: application/json\r\n\r\n");
    if (socket.open() < 0) {
        printf("Failed to open TCP Socket\n\r");
        return "Failed to open TCP Socket\n\r";
    }
    if (socket.connect("api.thethings.io", 80) < 0) {
        printf("Failed to connect with thethings.iO\n\r");
        return "Failed to connect with thethings.iO\n\r";
    }
    if (socket.write((void *)request.data(), request.size()) < 0) {
        printf("Failed to write HTTP request\n\r");
        return "Failed to write HTTP request\n\r";
    }
    int result = this->socket.read(buffer);
    //printf("Received %d bytes:\n\r%s\n\r", string string string string string string string int result, (char *)buffer.data());
    string response = string((char*)buffer.data()).substr(string((char*)buffer.data()).find("\r\n\r\n"));
    buffer.flush();
    socket.close();
    return response;
}

string ThethingsiO_DISCO_F746NG::thingRead(string resource, int limit)
{
    std::string request("GET /v2/things/");
    request.append(thingToken);
    request.append("/resources/");
    request.append(resource);
    request.append("?limit=");
    stringstream convert;
    convert << limit;
    request.append(convert.str());
    request.append(" HTTP/1.1\r\nHost: %s\r\nUser-Agent: curl/7.43.0\r\nAccept: application/json\r\n\r\n");
    if (socket.open() < 0) {
        printf("Failed to open TCP Socket\n\r");
        return "Failed to open TCP Socket\n\r";
    }

    if (socket.connect("api.thethings.io", 80) < 0) {
        printf("Failed to connect with thethings.iO\n\r");
        return "Failed to connect with thethings.iO\n\r";
    }

    if (socket.write((void *)request.data(), request.size()) < 0) {
        printf("Failed to write HTTP request\n\r");
        return "Failed to write HTTP request\n\r";
    }
    int result = this->socket.read(this->buffer);
    //printf("Received %d bytes:\n\r%s\n\r", string string string string string string string int result, (char *)buffer.data());
    string response = string((char*)this->buffer.data()).substr(string((char*)this->buffer.data()).find("\r\n\r\n"));
    this->buffer.flush();
    socket.close();
    return response;
}

int ThethingsiO_DISCO_F746NG::thingWrite(string resource, string value)
{
    string body;
    body.append("{\"values\":[{\"key\":\"");
    body.append(resource);
    body.append("\",\"value\":\"");
    body.append(value);
    body.append("\"}]}");
    std::string writerequest("POST /v2/things/");
    writerequest.append(thingToken);
    writerequest.append(" HTTP/1.1\r\nHost: %s\r\nUser-Agent: curl/7.43.0\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: ");
    stringstream convert;
    convert << body.length();
    writerequest.append(convert.str());
    writerequest.append("\r\n\r\n");
    writerequest.append(body);
    writerequest.append("\r\n");
    if (socket.open() < 0) {
        printf("Failed to open TCP Socket\n\r");
        return -1;
    }
    if (socket.connect("api.thethings.io", 80) < 0) {
        printf("Failed to connect with thethings.iO\n\r");
        return -1;
    }
    if (socket.write((void *)writerequest.data(), writerequest.size()) < 0) {
        printf("Failed to connect with thethings.iO\n\r");
        return -1;
    }
    int result = socket.read(buffer);
    //printf("Received %d bytes:\n\r%s\n\r", string string string string string string string int result, (char *)buffer.data());
    string response = string((char *) buffer.data());
    //printf("%s\n", (char *) buffer.data());
    int ret = response.find("status:created");
    buffer.flush();
    socket.close();
    if (ret >= 0) return 0;
    else return -1;
}

string ThethingsiO_DISCO_F746NG::thingActivate(string activationCode)
{
    string body;
    body.append("{\"activationCode\":\"");
    body.append(activationCode);
    body.append("\"}");
    std::string writerequest("POST /v2/things/");
    writerequest.append(" HTTP/1.1\r\nHost: %s\r\nUser-Agent: curl/7.43.0\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: ");
    stringstream convert;
    convert << body.length();
    writerequest.append(convert.str());
    writerequest.append("\r\n\r\n");
    writerequest.append(body);
    writerequest.append("\r\n");
    if (socket.open() < 0) {
        printf("Failed to open TCP Socket\n\r");
        return "Failed to open TCP Socket\n\r";
    }
    if (socket.connect("api.thethings.io", 80) < 0) {
        printf("Failed to connect with thethings.iO\n\r");
        return "Failed to connect with thethings.iO\n\r";
    }
    if (socket.write((void *)writerequest.data(), writerequest.size()) < 0) {
        printf("Failed to write HTTP request\n\r");
        return "Failed to write HTTP request\n\r";
    }
    int result = socket.read(buffer);
    //printf("Received %d bytes:\n\r%s\n\r", string string string string string string string int result, (char *)buffer.data());
    string response = string((char *) buffer.data());
    response = response.substr(response.find("\r\n\r\n"));
    buffer.flush();
    socket.close();

    //string string string int result = "{\"status\":\"created\",\"message\":\"thing activated\",\"thingToken\":\"5RWZ6xFogVSJEla3b2gbToHGF1Ko-R8vTNoS3FMl8mM\"}"
    return response;
}

string ThethingsiO_DISCO_F746NG::getToken()
{
    return this->thingToken;
}

void ThethingsiO_DISCO_F746NG::setToken(string token)
{
    this->thingToken = token;
}