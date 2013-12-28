
#include "JDKSAvdeccWorld.h"

#include "JDKSAvdeccWizNetIO.h"

#ifdef __AVR__
#include "utility/w5100.h"
#include "utility/socket.h"

namespace JDKSAvdecc {

WizNetIO::~WizNetIO() {
    close(0);
}


void WizNetIO::Initialize() {
    W5100.init();
    W5100.setMACAddress(m_mac_address.value);
    //W5100.setIPAddress(IPAddress(0,0,0,0).raw_address());
    socket(0,SnMR::MACRAW,0x22f0,0);
}


uint16_t WizNetIO::ReceiveRawNet(uint8_t *data,
                                   uint16_t max_len ) {
    uint8_t head[2];
    uint16_t data_len=0;
    uint16_t ptr=0;

    if( (W5100.readSnIR(0)&SnIR::RECV) !=0 ) {
        ptr = W5100.readSnRX_RD(0);
        W5100.read_data(0,(uint8_t*)ptr,head,2);
        ptr+=2;
        data_len = head[0];
        data_len = (data_len<<8) + head[1] - 2;
        if( data_len<=max_len ) {
            W5100.read_data(0,(uint8_t*) ptr,data,data_len);
        }
        ptr += data_len;
        W5100.writeSnRX_RD(0, ptr);
        W5100.execCmdSn(0, Sock_RECV);
    }
    return data_len <= max_len ? data_len : 0;
}

bool WizNetIO::SendRawNet(uint8_t const *data,
                          uint16_t len ) {

    W5100.writeSnDHAR(0, (uint8_t*)data );

    W5100.send_data_processing(0, (uint8_t *)data, len);
    W5100.execCmdSn(0, Sock_SEND_MAC);

    while ( (W5100.readSnIR(0) & SnIR::SEND_OK) != SnIR::SEND_OK )
    {
        if (W5100.readSnIR(0) & SnIR::TIMEOUT)
        {
            W5100.writeSnIR(0, (SnIR::SEND_OK | SnIR::TIMEOUT));
            return 0;
        }
    }

    W5100.writeSnIR(0, SnIR::SEND_OK);
}


}

#endif