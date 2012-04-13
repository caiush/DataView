#ifndef _SLICE_H_
#define _SLICE_H_
class slice
{
  //
  // A very light weight object used to store slices 
  // start, stop, step , desc
  // 0      0     1      all
  // 0      M    N       [o, M) in steps of N
  // M      0    N       [M, End) in steps of N
  // M      0    0       just M
  //
  // You can also select on various conponents, aka a "pick"
public:

  slice(int st, int sp, int j=1)// select semi open range
    : start(st),
      stop(sp),
      step(j)
  {
  }
  
  slice(int st) // select just one
    : start(st),
      stop(0),
      step(0)
  {
  }
  
  slice(): //select all
    start(0),
    stop(0),
    step(1)
  {
    
  }
  
  slice(const vector<int>& p)
    :
    start(0),
    stop(0),
    step(0),
    picks(p)
  {
  }
  
  
  int start;
  int stop;
  int step;
  vector<int> picks;  
};


#endif /* _SLICE_H_ */
