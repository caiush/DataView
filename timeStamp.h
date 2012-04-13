#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

struct TimeStamp{
  unsigned date;
  unsigned time;
  TimeStamp(unsigned date, unsigned time=0);
  bool operator==(const TimeStamp& rhs)const{
    return (this->date == rhs.date && time==rhs.time);
  }
  bool operator<(const TimeStamp& rhs)const{
    return (date<rhs.date || (date==rhs.date && time<rhs.time));
  };
  bool operator<=(const TimeStamp& rhs)const{
    return (date<rhs.date || (date==rhs.date && time<=rhs.time));
  };
  bool operator>(const TimeStamp& rhs)const{
    return (date>rhs.date || (date==rhs.date && time>rhs.time));
  };
  bool operator>=(const TimeStamp& rhs)const{
    return (date>rhs.date || (date==rhs.date && time>=rhs.time));
  };
};

#endif /* _TIMESTAMP_H_ */
