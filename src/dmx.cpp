/*
 * dmx.cpp
 *
 *  Created on: 24-06-2022
 *      Author: Sploit
 */

// *** INCLUDES ***
#include <dmx.h>

// *** Defines ***

// *** Variables & Objects ***
QueueHandle_t queue;
dmx_port_t dmxPort = 1;

// *** Prototypes ***
void dmx_init(uint16_t dmxAdress);
uint8_t dmx_update(userDataStruct *ptrUserData);

// *** Functions ***
void dmx_init(uint16_t dmxAdress)
{
    dmx_config_t dmxConfig = DMX_DEFAULT_CONFIG;
    dmx_param_config(dmxPort, &dmxConfig);
    dmx_set_pin(dmxPort, DMX_TX_PIN, DMX_RX_PIN, DMX_EN_PIN);

    uint8_t queueSize = 1;
    uint8_t interruptPriority = 1;

    dmx_driver_install(dmxPort, DMX_MAX_PACKET_SIZE, queueSize, &queue, interruptPriority);
}

uint8_t dmx_update(userDataStruct *ptrUserData)
{
    if (!ptrUserData->dmxOperation && !ptrUserData->dmxInfoTab)
        return 0;

    static bool dmxIsConnected = false;
    dmx_event_t packet;
    byte data[DMX_MAX_PACKET_SIZE];

    if (xQueueReceive(queue, &packet, 35)) // Average DMX transmit duration of 512 packages = 23ms
    {
        if (packet.status == DMX_OK)
        {
            if (!dmxIsConnected)
            {
                debugln("[DMX] Connected");
                dmxIsConnected = true;
            }

            dmx_read_packet(dmxPort, data, packet.size);

            for (int i = 0; i < CHANNEL_COUNT; i++)
            {
                if (data[ptrUserData->dmxAdress + i] != ptrUserData->dmxData[i])
                {
                    // debugln(String(i) + " DMXData Old: " + String(ptrUserData->dmxData[i]) + " | DMXData New: " + String(data[ptrUserData->dmxAdress + i]));
                    ptrUserData->dmxData[i] = data[ptrUserData->dmxAdress + i];
                    ptrUserData->dmxDataChange[i] = true;
                }
                else
                    ptrUserData->dmxDataChange[i] = false;
            }
        }
        else
        {
            debugln("[DMX] Faulty package");
        }
    }
    else if (dmxIsConnected)
    {
        debugln("[DMX] Timed out");
        dmxIsConnected = false;
    }

    return 1;
}