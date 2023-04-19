#include <cmath>
#include <iomanip>

#include "SobelFilter.h"

SobelFilter::SobelFilter(sc_module_name n): sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_filter);
  SC_THREAD(do_output);
  t_skt.register_b_transport(this, &SobelFilter::blocking_transport);
}

SobelFilter::~SobelFilter(){
  
}

int window[9];
int row=1;
int col=1;
int i=0;
int grey_temp[9];
int buffer_9[3][3];
int temp_median[512][512];
int ext_temp_median[513][513]={0};

void insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


int find_median(int array[][3]){
      //neighbor pixel values are stored in window including this pixel
      int median_value=0;
      window[0] = array[row-1][col-1];
      window[1] = array[row-1][col];
      window[2] = array[row-1][col+1];
      window[3] = array[row][col-1];
      window[4] = array[row][col];
      window[5] = array[row][col+1];
      window[6] = array[row+1][col-1];
      window[7] = array[row+1][col];
      window[8] = array[row+1][col+1];
      insertionSort(window,9);
      median_value=window[4];  
      return median_value;
}



void ext_arr(void){
  for(int row=1; row<512; row++){
    for(int col=1; col<512; col++){
      ext_temp_median[row][col]=temp_median[row-1][col-1];  
    }
  }
}

void do_mean(void){
  ext_arr();
  for(int col=1; col<512; col++){
    for(int row=1; row<512; row++){
      window[0] = ext_temp_median[row-1][col-1];
      window[1] = ext_temp_median[row-1][col];
      window[2] = ext_temp_median[row-1][col+1];
      window[3] = ext_temp_median[row][col-1];
      window[4] = 2*(ext_temp_median[row][col]);
      window[5] = ext_temp_median[row][col+1];
      window[6] = ext_temp_median[row+1][col-1];
      window[7] = ext_temp_median[row+1][col];
      window[8] = ext_temp_median[row+1][col+1];
      int mean=window[0]+window[1]+window[2]+window[3]+window[4]+window[5]+window[6]+window[7]+window[8];
      mean=mean/10;
      //std::cout<<"mean= "<<mean<<"  "<<std::endl;
      temp_median[row-1][col-1]=mean;

    }
  }
}


void SobelFilter::do_output(void){
  int val2;
  //wait(event1);
  while(true){
    //if(trigger_cnt==262144){
    do_mean();
    std::cout<<"execute do output"<<std::endl;
    for(int row=0; row<512; row++){
      for(int col=0; col<512; col++){
      val2=temp_median[col][row]; 
      int result=(int)(val2);
      //std::cout<<"result="<<result<<std::endl;
      o_result.write(result);
      }
    }
    //}
    //wait(1,SC_NS);
  }
}



void SobelFilter::do_filter() {
int grey;
  int x=0;
  int y=0;
  int k=0;



  while (true) {
    //median filter
    int median_value=0;
    for ( int v = 0; v < 3; ++v) {
      for ( int u = 0; u < 3; ++u) {
         //convert each pixel to greyscale
         if(x==0){
          grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
          buffer_9[v][u]=grey;
         }
      }
    }
    if(x!=0){
      buffer_9[0][0]=buffer_9[0][1];
      buffer_9[1][0]=buffer_9[1][1];
      buffer_9[2][0]=buffer_9[2][1];
      buffer_9[0][1]=buffer_9[0][2];
      buffer_9[1][1]=buffer_9[1][2];
      buffer_9[2][1]=buffer_9[2][2];
      for(int g=0; g<3; g++){
        grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        buffer_9[g][2]=grey;
      }
    }

    median_value=find_median(buffer_9);
    //std::cout<<"medain value  "<<median_value<<"  "<<std::endl;

    if(x==512){
      y=y+1;
      x=0;
    }
    trigger_cnt++;
    std::cout<<"trigger_cnt "<<trigger_cnt<<std::endl;
    temp_median[x][y]=median_value;
    x=x+1;
    k=k+1;
    std::cout<<"x= "<<x<<std::endl;
    std::cout<<"y= "<<y<<std::endl;
    // if(k==262144){
    //   event1.notify();
    //   std::cout<<"notify_is_trigger"<<std::endl;
    //  }
    //wait(1,SC_NS);
  }
}



void SobelFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr = addr - base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;

  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_RESULT_ADDR:
      buffer.uint = o_result.read();
      break;
    case SOBEL_FILTER_CHECK_ADDR:
      buffer.uint = o_result.num_available();
      break;
    default:
      std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      break;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    delay=sc_time(5, SC_NS);
    break;

  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case SOBEL_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_r.write(data_ptr[0]);
        delay=sc_time(10, SC_NS);
      }
      if (mask_ptr[1] == 0xff) {
        i_g.write(data_ptr[1]);
        delay=sc_time(10, SC_NS);
      }
      if (mask_ptr[2] == 0xff) {
        i_b.write(data_ptr[2]);
        delay=sc_time(10, SC_NS);
      }
      break;
    default:
      std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      break;
    }
    break;

  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  default:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  }
  payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}
