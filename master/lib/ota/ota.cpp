#include "global.hpp"

void sendOTACommand()
{
    StaticJsonDocument<512> doc;
    doc["fw_title"] = "OTA";
    doc["fw_version"] = "3.3";
    doc["fw_checksum"] = "abc123...";
    doc["fw_checksum_algorithm"] = "MD5";
    doc["fw_size"] = 150000;
    doc["fw_url"] = "http://yourserver.com/firmware.bin";

    // tb_led.sendTelemetryJson(doc);
}
