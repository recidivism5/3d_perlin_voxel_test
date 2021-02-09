#include <quaternion.h>
#include <math.h>

void hamilton_product(const float* q, const float* r, float *output){
  output[0] = (q[0] * r[0]) - (q[1] * r[1]) - (q[2] * r[2]) - (q[3] * r[3]);
  output[1] = (q[0] * r[1]) + (q[1] * r[0]) + (q[2] * r[3]) - (q[3] * r[2]);
  output[2] = (q[0] * r[2]) - (q[1] * r[3]) + (q[2] * r[0]) + (q[3] * r[1]);
  output[3] = (q[0] * r[3]) + (q[1] * r[2]) - (q[2] * r[1]) + (q[3] * r[0]);
}

void make_quaternion(const float* axis, const float theta, float *output){
  float sin_theta = sin(theta);

  output[0] = cos(theta);
  output[1] = sin_theta * axis[0];
  output[2] = sin_theta * axis[1];
  output[3] = sin_theta * axis[2];
}

void rotate(const float* point, const float* axis, const float theta, float *output){
  static float q[4];
  static float q_conj[4];
  static float o4d[4];
  static float p[4];
  static float inter[4];
  make_quaternion(axis, theta/2, q);

  memcpy(q_conj, q, sizeof(q_conj));
  q_conj[1] = -q[1];
  q_conj[2] = -q[2];
  q_conj[3] = -q[3];
  
  p[0] = 0.0F;
  memcpy(&p[1], point, 3*sizeof(float));
  
  hamilton_product(q, p, o4d);

  memcpy(inter, o4d, sizeof(o4d));
  
  hamilton_product(inter, q_conj, o4d);

  memcpy(output, &o4d[1], 3*sizeof(float));
}