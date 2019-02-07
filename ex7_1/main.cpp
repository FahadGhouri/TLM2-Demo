#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <tlm.h>

#include "src.h"
#include "trans.h"
#include "dst.h"
#include "mem.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {

  // instantiate all units
  src_unit    source("source");
  trans_unit  transform("transform");
  dst_unit    destination("destination");
  mem_unit    memory("memory");

  // <-- enter code to connect the units here
  source.init_socket_mem.bind(memory.tgt_socket);
  source.init_socket_transform.bind(transform.target_socket);
  transform.init_socket.bind(destination.tgt_socket);
  // start the calculation
  sc_start();

  return 0;

} // END MAIN

