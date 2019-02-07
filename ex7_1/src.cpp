#include "src.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>


using namespace sc_core;
using namespace tlm;
// <-- enter your implementation here
void src_unit::sendDataToM(){
    tlm_generic_payload trans2;
    unsigned char* pData;
    sc_core::sc_time t;
    unsigned int data; 
    unsigned int addressformem=1;


    unsigned char* pData2;
    int*           pValue;
    int            value;
    
    tlm_sync_enum return_path;

    tlm_base_protocol_types::tlm_phase_type phase;
    pData = reinterpret_cast<unsigned char*>(&data);
        trans2.set_data_ptr(pData);
        trans2.set_data_length(4);
        trans2.set_read();
        t = SC_ZERO_TIME;
    while(true) {
       
        trans2.set_response_status(TLM_INCOMPLETE_RESPONSE);
        trans2.set_address(addressformem); 
        

        phase=BEGIN_REQ;

        return_path=init_socket_mem->nb_transport_fw(trans2,phase,t);
       
         if(return_path==TLM_UPDATED){
                if(phase == BEGIN_RESP){
                     pData2 = trans2.get_data_ptr();
                     pValue = reinterpret_cast<int*>(pData2);
                     value = *pValue;
                     data_buffer.write(value);
    
                     phase=END_RESP;

                     init_socket_mem->nb_transport_fw(trans2,phase,t);

                }
         }
        addressformem++;
        
        if(addressformem == 350) break;
    }
}
// <-- enter your implementation here
tlm_sync_enum src_unit::nb_transport_bw(
  tlm_base_protocol_types::tlm_payload_type& trans,
  tlm_base_protocol_types::tlm_phase_type& phase,
  sc_core::sc_time& t){ 
      unsigned char* pData;
      int*           pValue;
      int            value;    

    if(phase == BEGIN_RESP){
    pData = trans.get_data_ptr();
    pValue = reinterpret_cast<int*>(pData);
    value = *pValue;
    data_buffer.write(value);
    
   // phase=END_RESP;

   // trans.set_response_status(TLM_OK_RESPONSE);

   // init_socket_mem->nb_transport_fw(trans,phase,t);

    return TLM_COMPLETED;

}
else //if(phase == END_REQ){
    return TLM_ACCEPTED;
//}

    

}

void src_unit::sendDataToT() {
    
    tlm_generic_payload trans;
    unsigned char* pData;
    sc_core::sc_time t;
    unsigned int data; 

    while(true) {
    
        data=data_buffer.read();
    
        pData = reinterpret_cast<unsigned char*>(&data);
        trans.set_data_ptr(pData);
        trans.set_data_length(4);
        trans.set_write();
        trans.set_response_status(TLM_INCOMPLETE_RESPONSE);
        trans.set_address(0x10000); 
        t = SC_ZERO_TIME;
        init_socket_transform->b_transport(trans,t);
        m_sent_data_count++;
        if(m_sent_data_count == m_data_size) break;
    }
    //send data to T per b_transport()
    std::cout << "end of the simulation @" << sc_time_stamp()<< std::endl;
}


