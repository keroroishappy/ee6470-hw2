#ifndef INITIATOR_H_
#define INITIATOR_H_
#include <systemc>
using namespace sc_core;

#include "tlm_utils/tlm_quantumkeeper.h"
#include <cstdint>
#include "tlm"
#include "tlm_utils/simple_initiator_socket.h"

class Initiator : public sc_module {
public:
  tlm_utils::simple_initiator_socket<Initiator> i_skt;
  tlm_utils::tlm_quantumkeeper m_qk;
  SC_HAS_PROCESS(Initiator);
  Initiator(sc_module_name n);
  int w=0;
  int r=0;
  int read_from_socket(unsigned long int addr, unsigned char mask[],unsigned char rdata[], int dataLen);
  int write_to_socket(unsigned long int addr, unsigned char mask[],unsigned char wdata[], int dataLen);
  void do_trans(tlm::tlm_generic_payload &trans);

  tlm::tlm_generic_payload trans;
  sc_time delay = sc_time(10, SC_NS);
};
#endif
