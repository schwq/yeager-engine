#include "NetworkSocket.h"
using namespace Yeager;

NetworkSocket::NetworkSocket() {}

NetworkSocket::NetworkSocket(const String& ip, int port)
{
  Initialize(ip, port);
}

NetworkSocket::~NetworkSocket()
{
  close(mClient);
}

void NetworkSocket::Initialize(const String& ip, int port)
{
  mClient = socket(AF_INET, SOCK_STREAM, 0);
  if (mClient < 0) {
    Yeager::Log(ERROR, "[CLIENT] -> Error establishing socket");
  }

  int attemps = 5;

  while ((!bConnected) && (attemps > 0)) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (connect(mClient, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0) {
      bConnected = true;
      Yeager::Log(INFO, "[CLIENT] -> Socket connected!");
    } else {
      port += 1;
      attemps -= 1;
      Yeager::Log(WARNING, "[CLIENT] -> Error connecting to port {}, attemping to connect to port {}", port - 1, port);
    }
  }
}

void NetworkSocket::Send(const String& message)
{
  if (!bConnected)
    return;

  int lenght = message.length();
  String lenght_str = std::to_string(lenght);
  String message_lenght = String(mMessageSizeLenght - lenght_str.length(), '0') + lenght_str;
  send(mClient, message_lenght.c_str(), mMessageSizeLenght, 0);
  send(mClient, message.c_str(), lenght, 0);
}

String NetworkSocket::Receive()
{
  if (!bConnected)
    return String();

  char message_lenght[mMessageSizeLenght] = {0};
  int n = recv(mClient, message_lenght, mMessageSizeLenght, 0);
  String message_lenght_str(message_lenght);
  int lenght = std::stoi(message_lenght_str);
  char message[lenght] = {0};
  n = recv(mClient, message, lenght, 0);
  return message;
}

void NetworkSocket::Send(const std::vector<unsigned int>& vec)
{
  if (!bConnected)
    return;

  size_t data_size = sizeof(unsigned int) / sizeof(unsigned char);
  size_t lenght = vec.size() * data_size;
  String lenght_str = std::to_string(lenght);
  String message_lenght = String(mMessageSizeLenght - lenght_str.length(), '0') + lenght_str;
  send(mClient, message_lenght.c_str(), mMessageSizeLenght, 0);
  send(mClient, (u_char*)vec.data(), lenght, 0);
}