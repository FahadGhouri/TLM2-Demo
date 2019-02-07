#ifndef _TRANS_H_
#define _TRANS_H_

#include <iostream>

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

SC_MODULE(trans_unit) {
  tlm_utils::simple_target_socket<trans_unit, 32, tlm::tlm_base_protocol_types> target_socket;
  tlm_utils::simple_initiator_socket<trans_unit, 32, tlm::tlm_base_protocol_types> init_socket;
  
  SC_CTOR(trans_unit) {
    items_in_buffer = 0;
    target_socket.register_b_transport(this, &trans_unit::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload &trans, sc_core::sc_time &t);
  
private:
  int items_in_buffer;
  int buffer[3];

}; // END SC_MODULE "trans_unit"

#endif // _TRANS_H_

