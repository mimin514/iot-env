void onSharedAttributeUpdate(const Shared_Attribute &attr) {
  if (attr.isKey("fw_url")) {
    fw_url = attr.value.as<String>();
    ota_update_requested = true;
  }
}

void otaUpdate() {
  if (fw_url == "") return;
  HTTPClient http;
  http.begin(fw_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int len = http.getSize();
    WiFiClient *stream = http.getStreamPtr();
    if (!Update.begin(len)) return;
    size_t written = Update.writeStream(*stream);
    if (Update.end()) {
      if (Update.isFinished()) ESP.restart();
    }
  }
  http.end();
}