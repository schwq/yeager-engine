#include "Connection.h"
using namespace Yeager;

bool IpAddr::Validate()
{
  if (IsValidIPv4(mAddr)) {
    mProtocol = EIpAddrProtocol::IPv4;
    mValid = true;
    return true;
  } else if (IsValidIPv6(mAddr)) {
    mProtocol = EIpAddrProtocol::IPv6;
    mValid = true;
    return true;
  } else {
    mProtocol = EIpAddrProtocol::Unknown;
    mValid = false;
    return false;
  }
}

bool IpAddr::IsValidIPv4(const String& addr)
{
  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, addr.c_str(), &(sa.sin_addr));
  return result != 0;
}

bool IpAddr::IsValidIPv6(const String& addr)
{
  struct sockaddr_in sa;
  int result = inet_pton(AF_INET6, addr.c_str(), &(sa.sin_addr));
  return result != 0;
}

Connection::Connection(const IpAddr& address, int port) : mIpAddress(address), mPort(port)
{
  Initialize();
}

Connection::~Connection() {}

void Connection::Initialize()
{
  if (bConnected)
    return;

  mSocketAddress.sin_family = AF_INET;
  mSocketAddress.sin_port = htons(mPort);
  inet_pton(AF_INET, mIpAddress.mAddr.c_str(), &mSocketAddress.sin_addr);
}

bool Connection::Connect(const int socket)
{
  return connect(socket, (const struct sockaddr*)&mSocketAddress, sizeof(mSocketAddress)) == 0;
}

IpAddr Connection::GetIpAddress() const
{
  return mIpAddress;
}

int Connection::GetPort() const
{
  return mPort;
}

bool Connection::IsConnected() const
{
  return bConnected;
}