#ifndef KALINKA_UTILS_H
#define KALINKA_UTILS_H

#include <Arduino.h>

void writeBuffer(byte*, unsigned int);

struct Point {
  float x;
  float y;
  float z;
};

struct Intersections {
  Point arr[2];
};

float get_x(float, float, float);

float get_t_by_x(float, float, float);

float get_y(float, float, float);

float get_t_by_y(float, float, float);

float get_z(float, float, float);

float get_t_by_z(float, float, float);

struct Intersections get_line_cube_intersection(float, float, float, float);

struct Point get_laser_point_on_cube(float, float, float, float);

float get_distance_to_cube(float, float, float, float);

#endif
