#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

const int SOBEL_FILTER_R_ADDR = 0x00000000;
const int SOBEL_FILTER_RESULT_ADDR = 0x00000010;
const int SOBEL_FILTER_CHECK_ADDR = 0x00000020;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
