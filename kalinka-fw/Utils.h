#ifndef KALINKA_UTILS_H
#define KALINKA_UTILS_H

void writeBuffer(SensorPacket* buffer) {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.write((uint8_t*)(buffer + i), 4);
  }
}

const float R = 4;
const float CUBE_SIZE = 3;

struct Point {
  float x;
  float y;
  float z;
};

struct Intersections {
  Point arr[2];
};

float get_x(float t, float scene_radian, float horizontal_radian) {
  return R * cos(scene_radian) - cos(scene_radian + horizontal_radian) *  t;
}

float get_t_by_x(float x, float scene_radian, float horizontal_radian) {
  return (R * cos(scene_radian) - x) / cos(scene_radian + horizontal_radian);
}

float get_y(float t, float scene_radian, float horizontal_radian) {
  return R * sin(scene_radian) - sin(scene_radian + horizontal_radian) *  t;
}

float get_t_by_y(float y, float scene_radian, float horizontal_radian) {
  return (R * sin(scene_radian) - y) / sin(scene_radian + horizontal_radian);
}

float get_z(float t, float height, float vertical_degree) {
  return height - cos(vertical_degree) *  t;
}

float get_t_by_z(float z, float height, float vertical_degree) {
  return (height - z) / cos(vertical_degree);
}

struct Intersections get_line_cube_intersection(float scene_angle, float horizontal_degree, float height, float vertical_degree) {
  float scene_radian = scene_angle * (3.14159265359 / 180);
  float horizontal_radian = horizontal_degree * (3.14159265359 / 180);
  float vertical_radian = vertical_degree * (3.14159265359 / 180);
  struct Intersections intersections;
  int ind = 0;
  float x, y, z, t;
  x = -CUBE_SIZE / 2;
  t = get_t_by_x(x, scene_radian, horizontal_radian);
  y = get_y(t, scene_radian, horizontal_radian);
  z = get_z(t, height, vertical_radian);
  if ((-CUBE_SIZE / 2 <= y) and (y <= CUBE_SIZE / 2) and (0 <= z) and (z <= CUBE_SIZE)) {
    struct Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    intersections.arr[ind] = p;
    ind += 1;
  }
  y = -CUBE_SIZE / 2;
  t = get_t_by_y(y, scene_radian, horizontal_radian);
  x = get_x(t, scene_radian, horizontal_radian);
  z = get_z(t, height, vertical_radian);
  //std::cout << x << " " << y << " " << z << " " << t << '\t';
  if ((-CUBE_SIZE / 2 < x) and (x < CUBE_SIZE / 2) and (0 <= z) and (z <= CUBE_SIZE)) {
    struct Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    intersections.arr[ind] = p;
    ind += 1;
  }
  z = -CUBE_SIZE / 2;
  t = get_t_by_z(y, height, vertical_radian);
  x = get_x(t, scene_radian, horizontal_radian);
  y = get_y(t, scene_radian, horizontal_radian);
  //std::cout << x << " " << y << " " << z << " " << t << '\t';
  if ((-CUBE_SIZE / 2 < x) and (x < CUBE_SIZE / 2) and (-CUBE_SIZE / 2 < y) and (y < CUBE_SIZE / 2)) {
    struct Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    intersections.arr[ind] = p;
    ind += 1;
  }
  x = CUBE_SIZE / 2;
  t = get_t_by_x(x, scene_radian, horizontal_radian);
  y = get_y(t, scene_radian, horizontal_radian);
  z = get_z(t, height, vertical_radian);
  //std::cout << x << " " << y << " " << z << " " << t << '\t';
  if ((-CUBE_SIZE / 2 <= y) and (y <= CUBE_SIZE / 2) and (0 <= z) and (z <= CUBE_SIZE)) {
    struct Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    intersections.arr[ind] = p;
    ind += 1;
  }
  y = CUBE_SIZE / 2;
  t = get_t_by_y(y, scene_radian, horizontal_radian);
  x = get_x(t, scene_radian, horizontal_radian);
  z = get_z(t, height, vertical_radian);
  //std::cout << x << " " << y << " " << z << " " << t << '\t';
  if ((-CUBE_SIZE / 2 < x) and (x < CUBE_SIZE / 2) and (0 <= z) and (z <= CUBE_SIZE)) {
    struct Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    intersections.arr[ind] = p;
    ind += 1;
  }
  z = CUBE_SIZE / 2;
  t = get_t_by_z(y, height, vertical_radian);
  x = get_x(t, scene_radian, horizontal_radian);
  y = get_y(t, scene_radian, horizontal_radian);
  //std::cout << x << " " << y << " " << z << " " << t << '\t';
  if ((-CUBE_SIZE / 2 < x) and (x < CUBE_SIZE / 2) and (-CUBE_SIZE / 2 < y) and (y < CUBE_SIZE / 2)) {
    struct Point p;
    p.x = x;
    p.y = y;
    p.z = z;
    intersections.arr[ind] = p;
    ind += 1;
  }
  if (ind != 2) {
    struct Point p1;
    p1.x = INFINITY;
    p1.y = INFINITY;
    p1.z = INFINITY;
    intersections.arr[0] = p1;
    struct Point p2;
    p2.x = INFINITY;
    p2.y = INFINITY;
    p2.z = INFINITY;
    intersections.arr[1] = p2;
  }
  return intersections;
};

struct Point get_laser_point_on_cube(float scene_angle, float horizontal_degree, float height, float vertical_degree) {
  struct Intersections points = get_line_cube_intersection(scene_angle, horizontal_degree, height, vertical_degree);
  float scene_radian = scene_angle * (3.14159265359 / 180);
  struct Point p1 = points.arr[0];
  struct Point p2 = points.arr[1];
  struct Point p;
  p.x = R * cos(scene_radian);
  p.y = R * sin(scene_radian);
  p.z = height;
  //std::cout << std::hypot(p1[0] - p[0], p1[1] - p[1]) << " " << std::hypot(p2[0] - p[0], p2[1] - p[1]) << '\t';
  if (sqrt(pow(p1.x - p.x, 2) +
           pow(p1.y - p.y, 2) +
           pow(p1.z - p.z, 2)) < sqrt(pow(p2.x - p.x, 2) +
                                      pow(p2.y - p.y, 2) +
                                      pow(p2.z - p.z, 2))) {
    return p1;
  } else {
    return p2;
  }
}

float get_distance_to_cube(float scene_angle, float horizontal_degree, float height, float vertical_degree) {
  struct Point p = get_laser_point_on_cube(scene_angle, horizontal_degree, height, vertical_degree);
  //std::cout << p.x << " " << p.y << '\t';
  float scene_radian = scene_angle * (3.14159265359 / 180);
  struct Point origin;
  origin.x = R * cos(scene_radian);
  origin.y = R * sin(scene_radian);
  origin.z = height;
  float distance = sqrt(pow(p.x - origin.x, 2) + pow(p.y - origin.y, 2) + pow(p.z - origin.z, 2));
  return distance;
}

#endif
