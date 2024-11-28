//    Yeager Engine, free and open source 3D/2D renderer written in OpenGL
//    In case of questions and bugs, please, refer to the issue tab on github
//    Repo : https://github.com/schwq/YeagerEngine
//    Copyright (C) 2023 - Present
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Common/Utils/Common.h"
#include "Common/Utils/LogEngine.h"
#include "Common/Utils/Utilities.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define YEAGER_DEFAULT_IP_ADDR "127.0.0.1"
#define YEAGER_DEFAULT_NET_PORT 5001

/**
 * @TODO: Finish this
 */

namespace Yeager {

struct EIpAddrProtocol {
  enum Enum { IPv4, IPv6, Unknown };
};

struct IpAddr {

  IpAddr(const String& ip) : mAddr(ip) { Validate(); }

  String mAddr = YEAGER_DEFAULT_IP_ADDR;
  EIpAddrProtocol::Enum mProtocol = EIpAddrProtocol::Unknown;
  bool mValid = false;

  bool Validate();
  static bool IsValidIPv4(const String& addr);
  static bool IsValidIPv6(const String& addr);
};

class Connection {

 public:
  Connection(const IpAddr& address, int port);
  ~Connection();

  void Initialize();
  bool Connect(const int socket);

  YEAGER_NODISCARD IpAddr GetIpAddress() const;
  YEAGER_NODISCARD int GetPort() const;
  bool IsConnected() const;

 private:
  IpAddr mIpAddress;
  int mPort = YEAGER_DEFAULT_NET_PORT;
  bool bConnected = false;
  struct sockaddr_in mSocketAddress;
};
}  // namespace Yeager