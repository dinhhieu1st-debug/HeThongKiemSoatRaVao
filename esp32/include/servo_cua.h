#ifndef SERVO_CUA_H
#define SERVO_CUA_H

namespace ServoCua
{

// Khoi tao servo dieu khien cua
void begin();

// Quay servo sang vi tri mo cua
void moCua();

// Quay servo sang vi tri dong cua
void dongCua();

// Tra ve true neu cua dang o trang thai mo
bool dangMo();

}

#endif
