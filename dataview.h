#ifndef _DATAVIEW_H_
#define _DATAVIEW_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
using std::vector;

#include "timeStamp.h"

//
// TODO: strip out colMajor stuff again.
//




class DataContainer{
  //
  // This class just takes care of the polymorphism, at some point
  // the memory management will get promoted to here, but not today!
 public:
  DataContainer(){ return; }
  virtual ~DataContainer(){return;}
  virtual const std::string& GetName()const=0;
  virtual  const TimeStamp& GetTimeStamp()const=0;
  virtual void SetName(const std::string& name)=0;
  virtual void SetTimeStamp(const TimeStamp& ts)=0;
  
 private:
  
};

template<typename T>
class DataView: public DataContainer
{
protected:
  struct header
  {
    T*           data;
    void*        owner;
    long         size;
    long         magic;
    unsigned     nref;
    char         memmode;
    std::string  name;
    TimeStamp    time;
  };

  // disable public use of the derfault ctor
  //
 DataView():
   _header(NULL), _data(NULL), _colMajor(false), _offset(0)
    {
    }
  
  // create a non-owned view on some data
  //
  DataView(header* hdr,const vector<int>& dims, const vector<int>& strides, bool colMajor=false, int offset=0)
    :_header(hdr),
    _data(hdr->data),
    _colMajor(colMajor),
    _offset(offset)
  {
    _header->nref++;
    const unsigned ndims = dims.size();    
    for (std::size_t k=0; k<dims.size(); k++)
      {
        _dims.push_back(dims[k]);
        _strides.push_back(strides[k]);                        
      }
    _nentries = 1;
    for (unsigned i=0; i<_dims.size() ; ++i) _nentries *=dims[i] ;        
  }
  
  // creates an owned dataview from precomputed values, use owr own strides, this should go away soon.
  DataView(T* data, const vector<int>& dims, const vector<int>& strides, bool colMajor=false, int offset=0)
    : _data (data),      
      _colMajor(colMajor),
      _offset(offset)
  {
    std::cout << "making " << this << std::endl;
    _header = new header;
    _header->data = _data;
    _header->owner = (void*)this;
    _header->magic = 0x8BADF00D;
    _header->nref = 1;
    _header->memmode = 0; //"0wner" controlled
    const unsigned ndims = dims.size();    
    for (std::size_t k=0; k<dims.size(); k++)
      {
        _dims.push_back(dims[k]);
        _strides.push_back(strides[k]);                        
      }
    _nentries = 1;
    for (unsigned i=0; i<_dims.size() ; ++i) _nentries *=dims[i] ;    
    _header->size = _nentries;
    std::cout << "made " << this << std::endl;
  }


  DataView(T*  data, const vector<int>& dims, bool colMajor=false, int offset=0)
    : _header ( new header), 
    _data (data),
    _colMajor(colMajor),
    _offset(offset)
  {
    std::cout << "making " << this << std::endl;
    const unsigned ndims = dims.size();
    _header->data = _data;
    _header->owner = (void*)this;
    _header->magic = 0x8BADF00D;
    _header->nref = 1;
    _header->memmode = 0; //"0wner" controlled
    
    for (std::size_t k=0; k<dims.size(); k++)
      {
        _dims.push_back(dims[k]);
        int s = 1;
        if (colMajor)
          for (int j=0; j<k-1; ++j) s*=dims[j];
        else
          for (int j=k+1; j<ndims; ++j)s*=dims[j];          
        _strides.push_back(s);                        
      }
    _nentries = 1;
    for (unsigned i=0; i<_dims.size() ; ++i) _nentries *=dims[i];    
    _header->size = _nentries;
    std::cout << "made " << this << std::endl;
  }


  //
  // Helper fuctions
  //
  //
  unsigned idx2offset(const int x, const int y, const int z)const
  {
    //FIXME, negative index? 
    return (_strides[0]*x + _strides[1]*y + _strides[2]*z );   
  }

  unsigned idx2offset(const int x)const
  {
    //FIXME, negative index?
    return (_strides[0]*x);    
  }

  unsigned idx2offset(const int x, const int y)const
  {
    return (_strides[0]*((x<0)?_dims[0]+x:x) + 
            _strides[1]*((y<0)?_dims[0]+y:y)  );   
  }
  
  unsigned idx2offset(const int* idx)const
  {
    unsigned x = 0;    
    for (std::size_t k=0; k<_dims.size(); k++)
      {
        const int i = (idx[k]<0)?_dims[k]+idx[k]:idx[k];        
        x += (_strides[k]*i);
      }
    return x;    
  }



public:

  const std::string& GetName()const{
    return (_header->name);
  }
  const TimeStamp& GetTimeStamp()const{
    return (_header->time);
  }
  void SetName(const std::string& name){
    _header->name = name;
  }
  void SetTimeStamp(const TimeStamp& ts){
    _header->time = time;
  }
  
  const T* GetDataPtr()const 
  {
    return _header->data;    
  }
  

  // ctor and dtor should be protected and friends of the data manager 
 DataView(const vector<int>& dims, const std::string& name, const TimeStamp& ts, bool colMajor=false)
    : _header(new header)
  {
    _nentries = 1;
    _colMajor = colMajor;
    const unsigned ndims = dims.size();
    for (std::size_t k=0; k<dims.size(); k++)
      {
        _dims.push_back(dims[k]);
        int s = 1;
        if (colMajor)
          for (int j=0; j<k-1; ++j) s*=dims[j];
        else
          for (int j=k+1; j<ndims; ++j)s*=dims[j];          
        _strides.push_back(s);                        
      }

    for (unsigned i=0; i<_dims.size() ; ++i) _nentries *=dims[i] ;    
    _data = (T*)malloc(sizeof(T)*_nentries);    
    _header->data = _data;
    _header->owner = (void*)this;
    _header->magic = 0x8BADF00D;
    _header->nref = 1;
    _header->memmode = 0; //"0wner" controlled
    _header->size = _nentries;
    _header->name = name;
    _header->time = ts;
  }
 DataView(const vector<int>& dims, const std::string& name, const TimeStamp& ts, T* data)
    : _header(new header)
  {
    _nentries = 1;
    const unsigned ndims = dims.size();
    for (std::size_t k=0; k<dims.size(); k++)
      {
        _dims.push_back(dims[k]);
        int s = 1;
        for (int j=k+1; j<ndims; ++j)s*=dims[j];          
        _strides.push_back(s);                        
      }

    for (unsigned i=0; i<_dims.size() ; ++i) _nentries *=dims[i] ;    
    _data = data;
    
    _header->data = _data;
    _header->owner = (void*)this;
    _header->magic = 0x8BADF00D;
    _header->nref = 1;
    _header->memmode = 0; //"0wner" controlled
    _header->size = _nentries;
    _header->name = name;
    _header->time = ts;
  }
  
  ~DataView()
  {
    std::cout << "Deleteing "<< this << std::endl;
    if ((this->owner() && this->memMode()==0) ||
	(this->memMode()==1 && _header->nref ==1 )){
      free((void*)(_header->data));
      _header->magic = 0;
      _header->data = 0;
      delete _header;
    }  
  }
  
  //
  // memory mamangement functions
  //
  bool owner()const
  {
    return ((void*)this) == _header->owner;
  }
  bool valid()const{
    return ((long)(this->_header->magic) == 0x8BADF00D);
  }
  char memMode()const{
    return _header->memmode;
  }
  void setMemMode(const char mode){
    _header->memmode = mode;
  }


  const DataView<T>& operator=(const T& rhs)
  {
    // this needs generatixing to the iterator
    T* p = _data + _offset ;
    int jump = 0;        
    for (unsigned i=0; i<_nentries; ++i)
      {
        jump = 0;
        
        for (int k=_dims.size()-1; k>=0;--k)
          {
            jump += (i % _dims[k])*_strides[k];
          }
        *(p+jump) = rhs;
      }
    return (*this);
  }
  
  
  unsigned rank()const
  {
    return _dims.size();    
  }

  unsigned size(const unsigned i)const
  {
    return  _dims[i];    
  }  
          
  const T& get(const vector<int>& idx)const
  {
    return _data[_offset+idx2offset(&(idx[0]))];    
  }
  const T& get( int x, int y)const
  {    
    return _data[_offset+idx2offset(x, y)];        
  }
  const T& get( int x, int y, int z)const
  {    
    return _data[_offset+idx2offset(x, y, z)];        
  }
  const T& get( int x)const
  {    
    return _data[_offset+idx2offset(x)];        
  }

  T& get(const vector<int>& idx)
  {
    return _data[_offset+idx2offset(&(idx[0]))];    
  }
  T& get( int x, int y)
  {    
    return _data[_offset+idx2offset(x, y)];        
  }
  T& get( int x, int y, int z)
  {    
    return _data[_offset+idx2offset(x, y, z)];        
  }
  T& get( int x)
  {    
    return _data[_offset+idx2offset(x)];        
  }
  
  
  
protected:
  T* _data; // for now this is legacy/premature optimization : we should go to header to check, as that does the BADFOOD check  
  mutable header* _header; // this needs to be mutable to allow coping a copy
  std::vector<int>  _dims;
  std::vector<int>  _strides;
  bool _colMajor; //should be moved to the header
  int _offset;    //
  int _nentries; //cache
};



#endif /* _DATAVIEW_H_ */
