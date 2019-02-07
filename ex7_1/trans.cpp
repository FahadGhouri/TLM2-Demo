#include "trans.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>

using namespace sc_core;
using namespace tlm;

void trans_unit::b_transport(tlm::tlm_generic_payload &trans, sc_core::sc_time &time) {
  int sum;
  int value;
  int *value_ptr;

  unsigned char* pData;
  tlm_generic_payload out_trans;
  sc_time out_time = SC_ZERO_TIME;;

  if (time != SC_ZERO_TIME) {
    SC_REPORT_ERROR("transform", "temporal decoupling not supported");
    trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
    return;
  }

  if (!trans.is_write()) {
    SC_REPORT_ERROR("transform", "only WRITE transactions supported");
    trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
    return;
  }

  if (trans.get_data_length() != 4) {
    SC_REPORT_ERROR("transform", "requiring sizeof(int) chars per transaction!");
    trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
    return;
  }

  // extract integer value
  value_ptr = reinterpret_cast<int *>(trans.get_data_ptr());
  value = *value_ptr;
  if (items_in_buffer < 3) {
    buffer[items_in_buffer] = value;
    items_in_buffer++;
  }
  else {
    sum = value;
    for (int i = 0; i <= 2; i++)
      sum += buffer[i];
    sum = sum/4;

    wait(8, SC_NS);
    
    // init payload and time argument
    pData = reinterpret_cast<unsigned char*>(&sum);
    out_trans.set_data_ptr(pData);
    out_trans.set_data_length(4);
    out_trans.set_write();
    out_trans.set_response_status(TLM_INCOMPLETE_RESPONSE);
    out_time = SC_ZERO_TIME;
    init_socket->b_transport(out_trans, out_time);

    // rotate buffer
    for (int i = 0; i <= 1; i++)
      buffer[i] = buffer[i+1];

    buffer[2] = value;

    trans.set_response_status(TLM_OK_RESPONSE);
  }
}

