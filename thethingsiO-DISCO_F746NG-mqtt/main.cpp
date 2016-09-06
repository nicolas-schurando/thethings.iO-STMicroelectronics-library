#include "mbed.h"
#include "rtos.h"
#include "TS_DISCO_F746NG.h"
#include "LCD_DISCO_F746NG.h"
#include <iostream>
#include <string>
#include <sstream>
#include "MQTTEthernet.h"
#include "MQTTClient.h"

LCD_DISCO_F746NG lcd;
TS_DISCO_F746NG ts;
//EthernetInterface eth;
MQTTEthernet ipstack = MQTTEthernet();
MQTT::Client<MQTTEthernet, Countdown> client = MQTT::Client<MQTTEthernet, Countdown>(ipstack);
MQTT::Message message;
int message_count = 0;
string buff;
stringstream convert;
int rc;

#define TOKEN "YOUR THING TOKEN"

string topic;

void addValue(string key, int value)
{
    if (message_count > 0) buff.append(",");
    buff.append("{\"key\":\"");
    buff.append(key);
    buff.append("\",\"value\":\"");
    convert.str("");
    convert << value;
    buff.append(convert.str());
    buff.append("\"}");
    message_count+= 1;
}

void send()
{
    buff.append("]}");
    //convert << "]}";
    message.qos = MQTT::QOS0; // QOS0 or 1 available
    message.payload = (void*)buff.c_str();
    message.retained = false;
    message.dup = false;
    message.payloadlen = strlen(buff.c_str());
    rc = client.publish(topic.c_str(), message);
    buff = "{\"values\":[";
    //convert.str("{\"values\":[");
    //lcd.DisplayStringAt(0, LINE(5), (uint8_t *)convert.str().c_str(), CENTER_MODE);
    message_count = 0;
}

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", message.payloadlen, (char*)message.payload);
}

int main(int argc, char* argv[])
{
    TS_StateTypeDef TS_State;
    uint16_t x, y, x_ant, y_ant;
    /***** TS *****/


    topic.append("v2/things/");
    topic.append(string(TOKEN));
    /***** MQTT *****/

    char* hostname = "mqtt.thethings.io";
    int port = 1883;
    rc = ipstack.connect(hostname, port);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "disco-f746";
    rc = client.connect(data);
    rc = client.subscribe(topic.c_str(), MQTT::QOS0, messageArrived);
    buff = "{\"values\":[";
    addValue("demo", 0);
    send();
    lcd.Clear(LCD_COLOR_BLACK);

    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.DrawCircle(160, 136, 50);
    wait(0.1);
    lcd.SetTextColor(LCD_COLOR_RED);
    lcd.FillCircle(160, 136, 50);
    wait(1);
    lcd.SetTextColor(LCD_COLOR_GREEN);
    lcd.DrawCircle(320, 136, 50);
    wait(0.1);
    lcd.SetTextColor(LCD_COLOR_GREEN);
    lcd.FillCircle(320, 136, 50);
    wait(1);
    ts.Init(lcd.GetXSize(), lcd.GetYSize());
    wait(3);
    while (1) {
        rc = client.connect(data);
        ts.GetState(&TS_State);
        if (TS_State.touchDetected) {
            x = TS_State.touchX[0];
            y = TS_State.touchY[0];
            if ((x_ant != x) && (y_ant != y)) {
                if ((x >= 110 && x <= 210)&&(y >= 86 && y <= 186)) {
                    addValue("demo", 1);
                    send();
                } else if ((x >= 270 && x <= 370)&&(y >= 86 && y <= 186)) {
                    addValue("demo", 2);
                    send();
                }
            }
            x_ant = x;
            y_ant = y;
        }
    }
}
