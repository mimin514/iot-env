#include "global.hpp"
#include "rpc.hpp"
void onAttributesReceived(const JsonObjectConst &data)
{
  if (data.containsKey("ledMode"))
  {

    ledMode = data["ledMode"];
    Serial.printf("Updated LED mode: %d\n", ledMode);
    ControlData ctrl= {};
    ctrl.ledMode = ledMode;

    esp_err_t result = esp_now_send(nodeAddress, (uint8_t *)&ctrl, sizeof(ctrl));
    if (result == ESP_OK)
    {
      Serial.println("Sent LED mode to node via ESP-NOW");
    }
    else
    {
      Serial.println("Failed to send LED mode to node");
    }

    // updateLedModeAttribute(); // <--- Thêm dòng này

    // return;
  }
}

void requestSharedAttributes()
{
  tb_led.Shared_Attributes_Request(Attribute_Request_Callback(onAttributesReceived));
}
void updateLedModeAttribute()
{

  tb_led.sendTelemetryData("ledMode", ledMode); // Nếu chỉ cần gửi lên như telemetry
}
RPC_Response setLedModeCallback(const RPC_Data &data)
{
  Serial.println("Received RPC call: setLedMode");
  Serial.print("Raw data received: ");
  Serial.println(data.as<String>());

  JsonObjectConst params = data["params"];

  if (params.containsKey("ledMode"))
  {
    ledMode = params["ledMode"];
    Serial.printf("LED mode updated: %d\n", ledMode);
    digitalWrite(LED_BUILTIN, ledMode ? HIGH : LOW);

    ControlData ctrl;
    ctrl.ledMode = ledMode;

    esp_err_t result = esp_now_send(nodeAddress, (uint8_t *)&ctrl, sizeof(ctrl));
    if (result == ESP_OK)
    {
      Serial.println("Sent LED mode to node via ESP-NOW");
    }
    else
    {
      Serial.println("Failed to send LED mode to node");
    }

    return RPC_Response("LED mode updated", true);
  }

  Serial.println("Error: No ledMode parameter found in RPC call.");
  return RPC_Response("Error: No ledMode parameter", false);
}
