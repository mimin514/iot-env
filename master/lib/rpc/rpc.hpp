#ifndef rpc_HPP
#define rpc_HPP


void onAttributesReceived(const JsonObjectConst &data) ;
void requestSharedAttributes() ;

RPC_Response setLedModeCallback(const RPC_Data &data) ;
void taskFanScheduler(void *pvParameters) ;
void updateLedModeAttribute(); // Thêm dòng này ở đầu file

#endif 