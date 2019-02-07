#include "mem.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>

#include <cstdio>
#include <iostream>

using namespace std;
using namespace sc_core;
using namespace tlm;

const sc_time MEM_REQ_ACCEPT_DELAY(5,SC_NS);
const sc_time MEM_LATENCY(10,SC_NS);

tlm_sync_enum mem_unit::nb_transport_fw(
  tlm_base_protocol_types::tlm_payload_type& trans,
  tlm_base_protocol_types::tlm_phase_type& phase,
  sc_core::sc_time& t)
{

  // for use with the PEQ
  tlm_base_protocol_types::tlm_phase_type next_phase(UNINITIALIZED_PHASE);

  // check time argument
  if (t != SC_ZERO_TIME)          // no support for temporal decoupling yet
  {
    SC_REPORT_ERROR("memory","temporal decoupling not supported");
  }

  // out of range address
  if(trans.get_address() >= 8192)
  {
    trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
    return TLM_COMPLETED;
  }

  // this is a read-only memory
  if(trans.is_write())
  {
    trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
    return TLM_COMPLETED;
  }

  // BURSTs (i.e. operations on multiple cells) are not supported, as well
  // as "fragmented" operations, i.e. operations that act only on a part of a
  // memory cell
  if (trans.get_data_length() != 4) {
  // set BURST error
    trans.set_response_status(TLM_BURST_ERROR_RESPONSE);
    return TLM_COMPLETED;
  }

  /*	use the backward path to notify END_REQ and BEG_RESP 
          push the payload and its phase into the PEQ and call the
          nb_transport_bw method at the associated simulation times
          (see the implementation of the peq callback below)
  */

  if(phase == BEGIN_REQ)
  {
    if(m_in_request)	/* still in the request phase */
    {
      trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
      return TLM_COMPLETED;
    }
    else
    {
      m_in_request = true;
      // To notify END_REQ
      next_phase = END_REQ;
      m_request_peq.notify(trans,next_phase,MEM_REQ_ACCEPT_DELAY);
      trans.set_response_status(TLM_OK_RESPONSE);
      #ifdef TRACE_ACCESSES
	cout << hex;
	cout << name() << " : received request @ " << sc_time_stamp()
	     << " for address " << trans.get_address() << endl;
      #endif // TRACE_ACCESSES
      return TLM_ACCEPTED;
    }
  }
  else if(phase == END_RESP)
  {
    m_in_response = false;
    trans.set_response_status(TLM_OK_RESPONSE);
    return TLM_COMPLETED;
  }

  // this point should never reached.
  trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
  return TLM_COMPLETED;
}

// callback of the PEQ
void mem_unit::request_queue_callback(
  tlm_base_protocol_types::tlm_payload_type& trans,
  const tlm_base_protocol_types::tlm_phase_type& stored_phase)
{

  sc_time                                        time_offset = SC_ZERO_TIME;
  int*                                           pData;
  tlm_base_protocol_types::tlm_phase_type        phase = stored_phase;


  // Read the payloads from the PEQ and send END_REQ and BEGIN_RESP accordingly
  time_offset = SC_ZERO_TIME;
  if(phase== END_REQ)
  {
    #ifdef TRACE_ACCESSES
      cout << hex;
      cout << name() << " : sending END_REQ @ " << sc_time_stamp()
           << " for address " << trans.get_address() << endl;
    #endif // TRACE_ACCESSES
    tgt_socket->nb_transport_bw(trans,phase,time_offset);
    m_in_request = false;

    // to send the response at the correct simulation time
    phase = BEGIN_RESP;
    time_offset = MEM_LATENCY - MEM_REQ_ACCEPT_DELAY;
    m_request_peq.notify(trans,phase,time_offset);

  }
  else /* BEG_RESP*/
  {
    /* wait until the response has been accepted */
    if(m_in_response)
    {
      time_offset = MEM_LATENCY - MEM_REQ_ACCEPT_DELAY;
      m_request_peq.notify(trans,phase,time_offset);
    }
    else
    {
      m_in_response = true;
      pData = reinterpret_cast< int*>(trans.get_data_ptr());
      *pData = storage[trans.get_address()];

      // use the backward path to send the response
      time_offset = SC_ZERO_TIME;
      #ifdef TRACE_ACCESSES
        cout << hex;
        cout << name() << " : sending BEGIN_RESP @ " << sc_time_stamp()
             << " for address " << trans.get_address() << endl;
      #endif // TRACE_ACCESSES
      tlm_sync_enum response = tgt_socket->nb_transport_bw(trans, phase,
        time_offset);
      if (    (response == TLM_COMPLETED)
           || (response == TLM_UPDATED && phase == END_RESP))
        m_in_response = false;
    }
  }

}

