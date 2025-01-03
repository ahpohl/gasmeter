#include "GasmeterMqtt.h"
#include <chrono>
#include <iostream>
#include <mosquitto.h>
#include <string>
#include <thread>

GasmeterMqtt::GasmeterMqtt(void) : Mosq(nullptr), Log(false) {
  IsConnected = false;
  NotifyOnlineFlag = false;
}

GasmeterMqtt::~GasmeterMqtt(void) {
  if (IsConnected) {
    mosquitto_disconnect(Mosq);
  }
  mosquitto_loop_stop(Mosq, false);
  mosquitto_destroy(Mosq);
  mosquitto_lib_cleanup();
}

void GasmeterMqtt::SetDebug(const bool &debug) { Log = debug; }

bool GasmeterMqtt::Begin(void) {
  mosquitto_lib_init();
  bool clean_session = true;
  if (!(Mosq = mosquitto_new(nullptr, clean_session, this))) {
    ErrorMessage = std::string("Mosquitto error: Out of memory.");
    return false;
  }
  mosquitto_connect_callback_set(Mosq, OnConnectCallbackWrapper);
  mosquitto_log_callback_set(Mosq, LogCallbackWrapper);
  return true;
}

bool GasmeterMqtt::SetUserPassAuth(const std::string &user,
                                   const std::string &pass) {
  int rc = 0;
  if ((rc = mosquitto_username_pw_set(Mosq, user.c_str(), pass.c_str()))) {
    ErrorMessage =
        std::string("Mosquitto unable to enable password authentication: ") +
        mosquitto_strerror(rc);
    return false;
  }
  return true;
}

bool GasmeterMqtt::SetTlsConnection(const std::string &cafile,
                                    const std::string &capath) {
  int rc = 0;
  if (!(cafile.empty())) {
    if ((rc =
             mosquitto_tls_set(Mosq, cafile.c_str(), NULL, NULL, NULL, NULL))) {
      ErrorMessage = std::string("Mosquitto unable to enable TLS: ") +
                     mosquitto_strerror(rc);
      return false;
    }
  } else if (!(capath.empty())) {
    if ((rc =
             mosquitto_tls_set(Mosq, NULL, capath.c_str(), NULL, NULL, NULL))) {
      ErrorMessage = std::string("Mosquitto unable to enable TLS: ") +
                     mosquitto_strerror(rc);
      return false;
    }
  }
  return true;
}

bool GasmeterMqtt::Connect(const std::string &host, const int &port,
                           const int &keepalive) {
  int rc = 0;
  if ((rc = mosquitto_loop_start(Mosq))) {
    ErrorMessage =
        std::string("Mosquitto loop start failed: ") + mosquitto_strerror(rc);
    return false;
  }
  if ((rc = mosquitto_connect_async(Mosq, host.c_str(), port, keepalive))) {
    ErrorMessage =
        std::string("Mosquitto unable to connect: ") + mosquitto_strerror(rc);
    return false;
  }

  return true;
}

bool GasmeterMqtt::SetLastWillTestament(const std::string &message,
                                        const std::string &topic,
                                        const int &qos, const bool &retain) {
  int rc = 0;
  if ((rc = mosquitto_will_set(Mosq, topic.c_str(), message.size(),
                               message.c_str(), qos, retain))) {
    ErrorMessage = std::string("Mosquitto unable to set last will: ") +
                   mosquitto_strerror(rc);
    return false;
  }
  return true;
}

bool GasmeterMqtt::PublishMessage(const std::string &message,
                                  const std::string &topic, const int &qos,
                                  const bool &retain) {
  int rc = 0;
  if ((rc = mosquitto_publish(Mosq, nullptr, topic.c_str(), message.size(),
                              message.c_str(), qos, retain))) {
    ErrorMessage =
        std::string("Mosquitto publish failed: ") + mosquitto_strerror(rc);
    IsConnected = false;
    return false;
  }
  return true;
}

std::string GasmeterMqtt::GetErrorMessage(void) const { return ErrorMessage; }

bool GasmeterMqtt::GetConnectStatus(void) const { return IsConnected; }

bool GasmeterMqtt::GetNotifyOnlineFlag(void) const { return NotifyOnlineFlag; }

void GasmeterMqtt::SetNotifyOnlineFlag(const bool &flag) {
  NotifyOnlineFlag = flag;
}

void GasmeterMqtt::OnConnectCallback(struct mosquitto *mosq, void *obj,
                                     int connack_code) {
  if (!connack_code) {
    IsConnected = true;
    NotifyOnlineFlag = true;
  } else {
    ErrorMessage = mosquitto_connack_string(connack_code);
    IsConnected = false;
  }
}

void GasmeterMqtt::OnConnectCallbackWrapper(struct mosquitto *mosq, void *obj,
                                            int connack_code) {
  auto *p = reinterpret_cast<GasmeterMqtt *>(obj);
  return p->GasmeterMqtt::OnConnectCallback(mosq, obj, connack_code);
}

void GasmeterMqtt::LogCallback(struct mosquitto *mosq, void *obj, int level,
                               const char *str) {
  if (Log) {
    std::cout << str << std::endl;
  }
}

void GasmeterMqtt::LogCallbackWrapper(struct mosquitto *mosq, void *obj,
                                      int level, const char *str) {
  auto *p = reinterpret_cast<GasmeterMqtt *>(obj);
  return p->GasmeterMqtt::LogCallback(mosq, obj, level, str);
}
