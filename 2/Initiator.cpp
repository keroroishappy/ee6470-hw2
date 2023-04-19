#include "Initiator.h"

Initiator::Initiator(sc_module_name n) : sc_module(n), i_skt("i_skt") {
  m_qk.set_global_quantum( sc_time(20, SC_NS) );
  m_qk.reset();
}

int Initiator::read_from_socket(unsigned long int addr, unsigned char mask[],
                                unsigned char rdata[], int dataLen) {
  // Set up the payload fields. Assume everything is 4 bytes.
  trans.set_read();
  trans.set_address((sc_dt::uint64)addr);
  static int R=0;
  trans.set_byte_enable_length((const unsigned int)dataLen);
  trans.set_byte_enable_ptr((unsigned char *)mask);
  R++;
  std::cout<<"r time"<<"R"<<std::endl;
  trans.set_data_length((const unsigned int)dataLen);
  trans.set_data_ptr((unsigned char *)rdata);
  delay = m_qk.get_local_time();
  // Transport.
  do_trans(trans);
  //std::cout << "trans = " <<"R"<< " time " << R << std::endl;
  
  m_qk.inc( delay );
  if (m_qk.need_sync()) m_qk.sync();
  /* For now just simple non-zero return code on error */
  return trans.is_response_ok() ? 0 : -1;

} // read_from_socket()

int Initiator::write_to_socket(unsigned long int addr, unsigned char mask[],
                               unsigned char wdata[], int dataLen) {
  // Set up the payload fields. Assume everything is 4 bytes.
  trans.set_write();
  trans.set_address((sc_dt::uint64)addr);
  static int W=0;
  trans.set_byte_enable_length((const unsigned int)dataLen);
  trans.set_byte_enable_ptr((unsigned char *)mask);
  W++;
  std::cout<<"w times"<<"W"<<std::endl;
  trans.set_data_length((const unsigned int)dataLen);
  trans.set_data_ptr((unsigned char *)wdata);
  delay = m_qk.get_local_time();
  // Transport.
  do_trans(trans);
  //std::cout << "trans = " <<"W"<< "  time " << W << std::endl;
  
  m_qk.inc( delay );
  if (m_qk.need_sync()) m_qk.sync();
  /* For now just simple non-zero return code on error */
  return trans.is_response_ok() ? 0 : -1;
} // writeUpcall()

void Initiator::do_trans(tlm::tlm_generic_payload &trans) {
  //sc_core::sc_time dummyDelay = sc_core::SC_ZERO_TIME;

  // Call the transport and wait for no time, which allows the thread to yield
  // and others to get a look in!

  i_skt->b_transport(trans, delay);
  //wait(sc_core::SC_ZERO_TIME);
  //wait(delay);

} // do_trans()
