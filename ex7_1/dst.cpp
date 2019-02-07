
#include "dst.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <cstdio>
#include <systemc>
#include <tlm.h>

using namespace std;
using namespace sc_core;
using namespace tlm;

void dst_unit::b_transport(tlm_generic_payload& trans, sc_time& t) {

  unsigned char* pData;
  int*           pValue;
  int            value;


  // check time argument: must be "zero", as this unit doesn't support
  // temporal decoupling yet
  if (t != SC_ZERO_TIME)          // no support for temporal decoupling yet
  {
    SC_REPORT_ERROR("destination","temporal decoupling not supported");
  }

  // destination supports only WRITE requests
  if (!(trans.is_write())) {
    SC_REPORT_ERROR("destination","only WRITE transactions supported");
  }

  // extract the value from the payload structure
  pData = trans.get_data_ptr();
  pValue = reinterpret_cast<int*>(pData);
  value = *pValue;

  // print the value as ASCII
  putchar(value);
  wait(1, SC_NS);
} // END METHOD "b_transport"

