#ifndef __MAT2_H__
#define __MAT2_H__

#include <cmath>


template <class T>
class mat2 {
 public:
  T x1, x2, y1, y2;
  
 mat2() :x1(0), x2(0), y1(0), y2(0) {}
 mat2(T x1, T x2, T y1, T y2) : x1(x1), x2(x2), y1(y1), y2(y2) {}
 mat2(const mat2& m) : x1(m.x1), x2(m.x2), y1(m.y1), y2(m.y2) {}
  
  mat2& operator=(const mat2& m) {
    x1 = m.x1;
    x2 = m.x2;
    y1 = m.y1;
    y2 = m.y2;

    return *this;
  }
  
  mat2 operator+(mat2& m) {
    return mat2(x1 + m.x1, x2 + m.x2, y1 + m.y1, y2 + m.y2);
  }
    
  mat2 operator-(mat2& m) {
    return mat2(x1 - m.x1, x2 - m.x2, y1 - m.y1, y2 - m.y2);
  }
  
  mat2& operator+=(mat2& m) {
    x1 += m.x1;
    x2 += m.x2;
    y1 += m.y1;
    y2 += m.y2;

    return *this;
  }
    
  mat2& operator-=(mat2& m) {
    x1 += m.x1;
    x2 += m.x2;
    y1 += m.y1;
    y2 += m.y2;
      
    return *this;
  }
  
  mat2 operator+(double s) {
    return mat2(x1 + s, x2 + s, y1 + s, y2 + s);
  }
    
  mat2 operator-(double s) {
      return mat2(x1 - s, x2 - s, y1 - s, y2 - s);
  }
    
  mat2 operator*(double s) {
    return mat2(x1 * s, x2 * s, y1 * s, y2 * s);
  }
    
  mat2 operator/(double s) {
    return mat2(x1 / s, x2 / s, y1 / s, y2 / s);
  }
  
  mat2& operator+=(double s) {
      x1 += s;
      x2 += s;
      y1 += s;
      y2 += s;
      
      return *this;
  }
  mat2& operator-=(double s) {
      x1 -= s;
      x2 -= s;
      y1 -= s;
      y2 -= s;
      
      return *this;
  }
  mat2& operator*=(double s) {
      x1 *= s;
      x2 *= s;
      y1 *= s;
      y2 *= s;
      
      return *this;
  }
  mat2& operator/=(double s) {
      x1 /= s;
      x2 /= s;
      y1 /= s;
      y2 /= s;
      
      return *this;
  }
  
  void set(T x1, T x2, T y1, T y2) {
    this->x1 = x1;
    this->x2 = x2;
    this->y1 = y1;
    this->y2 = y2;
  }

};

typedef mat2<float> mat2f;
typedef mat2<double> mat2f;


#endif
