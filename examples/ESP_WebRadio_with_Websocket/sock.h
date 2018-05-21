#include <WebSocketsClient.h>
#include <ArduinoJson.h>

WebSocketsClient webSocket;
bool isConnected = false;

typedef void (*streamtype)(char * data);

int oncount = 0;

struct ON {
  String id;
  streamtype func;
} ons[10];

void lookup (String id, char * data) {
  for (int i = 0; i < oncount; i++) {
    if (id == ons[i].id) {
      ons[i].func(data);
      return;
    }
  }
}

static void onconnect(char * data) { };
static void ondisconnect(char * data) { };
static void ondata(char * data) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);
  for (auto field : root) {
    String value = root[field.key];
    lookup(field.key, (char*)value.c_str());
  }
};
void decode(char * json) { }

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  char s1[length + 1];
  strncpy_P(s1, (char*)payload, length);
  s1[length] = 0;
  switch (type) {
    case WStype_DISCONNECTED:
      isConnected = false;
      lookup("disconnect", &s1[0]);
      break;
    case WStype_CONNECTED:
      isConnected = true;
      lookup("connect", &s1[0]);
      break;
    case WStype_TEXT:
      lookup("data", &s1[0]);
      //decode((char*)payload);
      break;
    case WStype_BIN:
      break;
  }
}

void add(String id, streamtype func) {
  for (int i = 0; i < oncount; i++) {
    if (ons[i].id == id) {
      ons[i].func = func;
      return;
    }
  }
  ons[oncount].id = id;
  ons[oncount].func = func;
  oncount++;
}

class SOCK {
  private:
  public:
    void on(String id, streamtype func) {
      add(id, func);
    }
    void begin(const char * wshost, int wsport) {
      webSocket.begin(wshost, wsport, "/");
      webSocket.onEvent(webSocketEvent);
    };
    void loop() {
      webSocket.loop();
    }
    SOCK() {
      on("connect", onconnect);
      on("disconnect", ondisconnect);
      on("data", ondata);
    }
};

