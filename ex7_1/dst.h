#ifndef _DST_H_
#define _DST_H_

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

SC_MODULE(dst_unit) {

  SC_CTOR(dst_unit) {
    // register TLM callback methods
    tgt_socket.register_b_transport(this, &dst_unit::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);

  tlm_utils::simple_target_socket<dst_unit, 32,
    tlm::tlm_base_protocol_types> tgt_socket;

}; // END SC_MODULE "dst_unit"

#endif // _DST_H_

