/*
 * http-parser.cc
 *
 *  Created on: Jan 29, 2014
 *      Author: dimitriv
 */

#include "http-parser.h"
#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/socket.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "http-header.h"
#include "mpeg-header.h"
#include "dash-client.h"

NS_LOG_COMPONENT_DEFINE("HttpParser");

namespace ns3
{

  HttpParser::HttpParser() :
      m_bytes(0), m_app(NULL), m_lastmeasurement("0s")
  {
    NS_LOG_FUNCTION(this);
  }

  HttpParser::~HttpParser()
  {
    NS_LOG_FUNCTION(this);
  }

  void
  HttpParser::SetApp(DashClient *app)
  {
    NS_LOG_FUNCTION(this << app);
    m_app = app;
  }
  void
  HttpParser::ReadSocket(Ptr<Socket> socket)
  {
    NS_LOG_FUNCTION(this << socket);
    Address from;
    int bytes = socket->RecvFrom(&m_buffer[m_bytes], MPEG_MAX_MESSAGE - m_bytes,
        0, from);

    MPEGHeader mpeg_header;
    HTTPHeader http_header;

    uint32_t headersize = mpeg_header.GetSerializedSize()
        + http_header.GetSerializedSize();

    if (bytes > 0)
      {
        m_bytes += bytes;

        if (m_lastmeasurement > Time("0s"))
          {
            double dt = (Simulator::Now() - m_lastmeasurement).GetSeconds();
            NS_LOG_INFO(
                Simulator::Now().GetSeconds() << " bytes: " << bytes << " dt: " << dt << " bitrate: " << (8 * (bytes + headersize)/ dt));
          }
        m_lastmeasurement = Simulator::Now();
      }

    NS_LOG_INFO(
        "### Buffer space: " << m_bytes << " Queue length " << m_app->m_player.GetQueueSize());

    if (m_bytes < headersize)
      {
        return;
      }

    Packet headerPacket(m_buffer, headersize);
    headerPacket.RemoveHeader(mpeg_header);

    uint32_t message_size = headersize + mpeg_header.GetSize();

    if (m_bytes < message_size)
      {
        return;
      }
    Packet message(m_buffer, message_size);

    memmove(m_buffer, &m_buffer[message_size], m_bytes - message_size);
    m_bytes -= message_size;

    m_app->MessageReceived(message);

    ReadSocket(socket);
  }
} // namespace ns3