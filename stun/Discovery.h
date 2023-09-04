//
//  Discovery.h
//  stun
//
//  Created by Maoxu Li on 2/11/12.
//  Copyright (c) 2012 LIM Labs. All rights reserved.
//

#ifndef STUN_DISCOVERY_H
#define STUN_DISCOVERY_H

#include "Config.h"
#include "Message.h"
#include "Network.h"
#include <memory>

enum NAT_TYPE
{
    NAT_Open,
    NAT_FullCone,
    NAT_IpRestrictedCone,
    NAT_PortRestrictedCone,
    NAT_Symmetric,
    NAT_Blockled,
    NAT_Unknown
};

struct NatInfo
{
    NAT_TYPE NatType;
    std::string NatIp;
    int NatPort;
    std::string NatName;
};

class IvcpP2PMgr;

STUN_BEGIN

/*
 RFC 3489 (March 2003)
 10.1  Discovery Process

 In this scenario, a user is running a multimedia application which
 needs to determine which of the following scenarios applies to it:

 o  On the open Internet
 o  Firewall that blocks UDP
 o  Firewall that allows UDP out, and responses have to come back to
 the source of the request (like a symmetric NAT, but no
 translation.  We call this a symmetric UDP Firewall)
 o  Full-cone NAT
 o  Symmetric NAT
 o  Restricted cone or restricted port cone NAT
 */



    class Discovery
{
public:
    // Initiate with STUN server
    Discovery(std::shared_ptr<network::UdpSocket>, const std::string& host, unsigned short port, int timeout = 2000); // ms
    ~Discovery();

    void discover(IvcpP2PMgr* mgr);

private:

    bool isLocalAddress(const struct sockaddr_in& sin);
    bool isSameAddress(const struct sockaddr_in& a, const struct sockaddr_in& b);

    void sendMessage(Message* msg);
    Message* receiveMessage(int timeout);

    BindingResponse* binding(bool portChange = false, bool ipChange = false);

private:
    std::string _host;
    unsigned short _port;
    int _timeout;
    // Transaction ID of last request, discard unmatched response
    network::UUID _tid;
    std::shared_ptr<network::UdpSocket> _socket;
};

STUN_END

#endif
