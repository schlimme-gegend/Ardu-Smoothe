//smoothe.h
#ifndef SMOOTHE_H
#define SMOOTHE_H
#include <exception>

using namespace std;

template<typename T, int items>
class Smoothe
{
  private:
  T raw_data[items];
  long previous;
  unsigned char index, counter, mask, right_shift;
	
  public:
  bool dirty;
	Smoothe(bool initialize_raw_data = true)
  {
    index = 0;
    // The bitmask that will be used to extract the index for writing
    // to raw_data is created
    mask = items - 1;
    // right_shift will be used to divide the sum of all items in 
    // raw_data by the number of items. This is done by right shifting
    // until only one significant bit remains
    right_shift = 0;
    int count_these_bits = items;
    while(count_these_bits > 1){
      count_these_bits = count_these_bits >> 1;
      right_shift ++;
    }
    // raw_data is initiallized with zeroes. This is arbitrary but at 
    // least predictable
    if(initialize_raw_data){
			for(int i = 0; i < items; i++){
				raw_data[i] = 0;
			}
    }
  }
  
  void fill_with(T value){
		for(int i = 0; i < items; i++){
			raw_data[i] = value;
		}
	}
  
  void append(T value){
    raw_data[index] = value;
    // counter is an eight bit number that is continously incremented
    // and will flow over to zero after 256 additions to the raw_data
    counter++;
    // The mask is used to extract a number between 0 and intems-1 to
    // use as index to store raw_data
    index = counter & mask;
    // The dirty bit is set to indicate, that the raw_data has likely 
    // changed since the last fetch
    dirty = true;
  }

  T fetch(){
		// to_return is used to calculate the sum of raw_data
    long to_return = 0;
    for(int i = 0; i < items; i++){
      to_return += raw_data[i];
    }
    // it is then divided by items and the dirty bit is reset
    to_return = to_return >> right_shift;
    dirty = false;
    return T(to_return);
  }
  
  char current_index(){
		return index;
	}
};
#endif
