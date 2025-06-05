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
  //   if (data.containsKey("fanSchedule")) {
  //   JsonObjectConst sched = data["fanSchedule"];
  //   String start = sched["start"];  // "08:00"
  //   String end = sched["end"];      // "10:30"
  //   fanSchedule.mode = sched["mode"];

  //   fanSchedule.startHour = start.substring(0,2).toInt();
  //   fanSchedule.startMin = start.substring(3,5).toInt();
  //   fanSchedule.endHour = end.substring(0,2).toInt();
  //   fanSchedule.endMin = end.substring(3,5).toInt();
  //   fanSchedule.valid = true;

  //   Serial.println("Fan schedule updated");
  // }

  return RPC_Response("Error: No ledMode parameter", false);
}

void taskFanScheduler(void *pvParameters)
{
  while (1)
  {
    // if (fanSchedule.valid) {
    //   struct tm timeinfo;
    //   if (getLocalTime(&timeinfo)) {
    //     int nowMin = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    //     int startMin = fanSchedule.startHour * 60 + fanSchedule.startMin;
    //     int endMin = fanSchedule.endHour * 60 + fanSchedule.endMin;

    //     ControlData ctrl = {};
    //     ctrl.fanControl = true;

    //     if (nowMin >= startMin && nowMin <= endMin) {
    //       ctrl.fanState = fanSchedule.mode;
    //     } else {
    //       ctrl.fanState = false;
    //     }

    //     esp_now_send(nodeAddress, (uint8_t*)&ctrl, sizeof(ctrl));
    //   }
    // }

    vTaskDelay(pdMS_TO_TICKS(10000)); // kiểm tra mỗi 10s
  }
}
