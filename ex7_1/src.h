#ifndef _SRC_H_
#define _SRC_H_

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_core;
using namespace tlm;
using namespace tlm_utils;

SC_MODULE(src_unit) {


  // <-- enter your implementation here
    SC_CTOR(src_unit) : data_buffer(8192) {
    init_socket_mem.register_nb_transport_bw(this, &src_unit::nb_transport_bw);

    SC_THREAD(sendDataToT); 
    SC_THREAD(sendDataToM);
  }

  tlm_sync_enum nb_transport_bw(
    tlm_base_protocol_types::tlm_payload_type& trans,
    tlm_base_protocol_types::tlm_phase_type& phase,
    sc_core::sc_time& t);
  // --> enter your implementation here

  tlm_utils::simple_initiator_socket<src_unit, 32,
    tlm::tlm_base_protocol_types> init_socket_mem;
  tlm_utils::simple_initiator_socket<src_unit, 32,
    tlm::tlm_base_protocol_types> init_socket_transform;

  void sendDataToT();
  void sendDataToM();
  
  // big enough fifo that no data will be lost
  sc_core::sc_fifo<unsigned int> data_buffer; 
  unsigned int m_data_size=350; 
  unsigned int m_sent_data_count;
}; // END SC_MODULE "src_unit"

#endif

