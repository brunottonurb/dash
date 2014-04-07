/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 TEI of Western Macedonia, Greece
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Dimitrios J. Vergados <djvergad@gmail.com>
 */

#ifndef DASH_SERVER_H
#define DASH_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include <map>
#include <queue>

namespace ns3
{

  class Address;
  class Socket;
  class Packet;

  /**
   * \ingroup applications
   * \defgroup packetsink DashServer
   *
   * This application was written to complement OnOffApplication, but it
   * is more general so a DashServer name was selected.  Functionally it is
   * important to use in multicast situations, so that reception of the layer-2
   * multicast frames of interest are enabled, but it is also useful for
   * unicast as an example of how you can write something simple to receive
   * packets at the application layer.  Also, if an IP stack generates
   * ICMP Port Unreachable errors, receiving applications will be needed.
   */

  /**
   * \ingroup packetsink
   *
   * \brief Receive and consume traffic generated to an IP address and port
   *
   * This application was written to complement OnOffApplication, but it
   * is more general so a DashServer name was selected.  Functionally it is
   * important to use in multicast situations, so that reception of the layer-2
   * multicast frames of interest are enabled, but it is also useful for
   * unicast as an example of how you can write something simple to receive
   * packets at the application layer.  Also, if an IP stack generates
   * ICMP Port Unreachable errors, receiving applications will be needed.
   *
   * The constructor specifies the Address (IP address and port) and the
   * transport protocol to use.   A virtual Receive () method is installed
   * as a callback on the receiving socket.  By default, when logging is
   * enabled, it prints out the size of packets and their address, but
   * we intend to also add a tracing source to Receive() at a later date.
   */
  class DashServer : public Application
  {
  public:
    static TypeId
    GetTypeId(void);
    DashServer();

    virtual
    ~DashServer();

    /**
     * \return the total bytes received in this sink app
     */
    /* uint32_t GetTotalRx () const;*/

    /**
     * \return pointer to listening socket
     */
    Ptr<Socket>
    GetListeningSocket(void) const;

    /**
     * \return list of pointers to accepted sockets
     */
    std::list<Ptr<Socket> >
    GetAcceptedSockets(void) const;

  protected:
    virtual void
    DoDispose(void);
  private:
    // inherited from Application base class.
    virtual void
    StartApplication(void);    // Called at time specified by Start
    virtual void
    StopApplication(void);     // Called at time specified by Stop

    void
    HandleRead(Ptr<Socket>);
    void
    DataSend(Ptr<Socket>, uint32_t);
    void
    SendSegment(uint32_t video_id, uint32_t resolution, uint32_t segment_id,
        Ptr<Socket> socket);
    void
    HandleAccept(Ptr<Socket>, const Address& from);
    void
    HandlePeerClose(Ptr<Socket>);
    void
    HandlePeerError(Ptr<Socket>);

    // In the case of TCP, each socket accept returns a new socket, so the
    // listening socket is stored seperately from the accepted sockets
    Ptr<Socket> m_socket;       // Listening socket
    std::list<Ptr<Socket> > m_socketList; //the accepted sockets

    Address m_local;        // Local address to bind to
    uint32_t m_totalRx;      // Total bytes received
    TypeId m_tid;          // Protocol TypeId
    TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;
    std::map<Ptr<Socket>, std::queue<Packet> > m_queues;

  };

} // namespace ns3

#endif /* DASH_SERVER_H */

