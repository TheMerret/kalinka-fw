#include <Math.h>

#include "Scanner.h"

#define sensor1 A0
#define sensor2 A1

const bool LOGGING = false;

const byte SENSOR1_ID = 1;
const byte SENSOR2_ID = 2;

const byte START = 1;
const byte STOP = 0;
const byte SCANNING = 2;
const byte PAUSE = 3;
const byte RESUME = 4;
const byte PENDING = 5;

const float MAX_SCENE_ROTATION_STEP = 30.0;  // degree
const float MAX_SCENE_ROTATION = 180.0;
const float SENSOR_HEIGHT_STEP = 1.0;
const float SCENE_ROTATION_STEP = 1.0;
const float SENSOR_HORIZONTAL_ROTATION_STEP = 1.0;
const float SENSOR_VERTICAL_ROTATION_STEP = 1.0;
const float SENSOR_MAX_HEIGHT = 3.0;
const byte SCANNING_HORIZONTALLY = 1;
const byte SCANNING_VERTICALLY = 2;
const byte SCANNING_BOTH_DIRECTIONS =  3;

byte state = PENDING;
byte scannig_direction = SCANNING_HORIZONTALLY;

void setup() {
  Serial.begin(9600);
  while (!Serial); 
  Serial.write(255);  // TODO: normal handshake
}

struct SensorState {
  float height = 0.0;
  float horizontal_degree = 0.0;
  float vertical_degree = 0.0;
};

struct SensorData {
  float distance = 0;
};

struct SensorsData {
  SensorData sensor1;
  SensorData sensor2;
};

struct ScanningState {
  float scene_angle = 0.0;
  SensorState sensor1;
  SensorState sensor2;
};

ScanningState scanning_state;

void reset_sensor2_height() {
  if (LOGGING) {
    Serial.println("sensor 2 reset height");
  }
  scanning_state.sensor2.height = SENSOR_MAX_HEIGHT;
}

void reset_sensor1_height() {
  if (LOGGING) {
    Serial.println("sensor 1 reset height");
  }
  scanning_state.sensor1.height = 0;
}

void reset_sensor_height(byte sensor_id) {
  switch (sensor_id) {
    case SENSOR1_ID:
      reset_sensor1_height();
      break;
    case SENSOR2_ID:
      reset_sensor2_height();
      break;
  }
}

void reset_sensor2_vertical_angle() {
  if (LOGGING) {
    Serial.println("sensor 2 reset vertical rotation");
  }
  float degree;
  if (scannig_direction & SCANNING_VERTICALLY) {
    degree = -MAX_SENSOR_VERTICAL_ROTATION;
  } else {
    degree = 0.0;
  }
  scanning_state.sensor2.vertical_degree = degree;
}

void reset_sensor2_horizontal_angle() {
  if (LOGGING) {
    Serial.println("sensor 2 reset horizontal rotation");
  }
  scanning_state.sensor2.horizontal_degree = 0;
}

void reset_sensor2_angles() {
  reset_sensor2_horizontal_angle();
  reset_sensor2_vertical_angle();
}

void reset_sensor1_vertical_angle() {
  if (LOGGING) {
    Serial.println("reset 1 reset vertical rotation");
  }
  float degree;
  if (scannig_direction & SCANNING_VERTICALLY) {
    degree = -MAX_SENSOR_VERTICAL_ROTATION;
  } else {
    degree = 0.0;
  }
}

void reset_sensor1_horizontal_angle() {
  if (LOGGING) {
    Serial.println("sensor 1 reset horizontal rotation");
  }
  scanning_state.sensor1.horizontal_degree = 0;
}

void reset_sensor1_angles() {
  reset_sensor1_horizontal_angle();
  reset_sensor1_vertical_angle();
}

void reset_sensor_angles(byte sensor_id) {
  switch (sensor_id) {
    case SENSOR1_ID:
      reset_sensor1_angles();
      break;
    case SENSOR2_ID:
      reset_sensor2_angles();
      break;
  }
}

void reset_sensor(byte sensor_id) {
  reset_sensor_angles(sensor_id);
  reset_sensor_height(sensor_id);
}

void reset_sensors() {
  byte sensor_ids[] = {SENSOR1_ID, SENSOR2_ID};
  for (byte sensor_id : sensor_ids) {
    reset_sensor(sensor_id);
  }
}

void reset_scene_origin() {
  if (LOGGING) {
    Serial.println("scene reset rotation");
  }
  scanning_state.scene_angle = 0;
}

void reset_scan() {
  reset_sensors();
  reset_scene_origin();
}

byte parse_command(byte command, byte prev_state) {
  byte new_state;
  switch (command) {
    case START:
      if (prev_state == STOP or prev_state == PENDING) {
          new_state = START;
      };
      break;
    case STOP:
      if (prev_state == SCANNING) {
          new_state = STOP;
      };
      break;
    case PAUSE:
      if (prev_state == SCANNING) {
          new_state = PAUSE;
      };
      break;
    case RESUME:
      if (prev_state == PAUSE) {
          new_state = RESUME;
      };
      break;
    default:
      Serial.println("error");
  };
  return new_state;
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
    x = -CUBE_SIZE/2;
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
    y = -CUBE_SIZE/2;
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
    z = -CUBE_SIZE/2;
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
    x = CUBE_SIZE/2;
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
    y = CUBE_SIZE/2;
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
    z = CUBE_SIZE/2;
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

struct SensorsData capture_sensors_test() {
  SensorsData sd;
  sd.sensor1.distance = get_distance_to_cube(scanning_state.scene_angle, scanning_state.sensor1.horizontal_degree, scanning_state.sensor1.height, scanning_state.sensor1.vertical_degree + 90); 
  sd.sensor2.distance = get_distance_to_cube(scanning_state.scene_angle + 180.0, scanning_state.sensor2.horizontal_degree, scanning_state.sensor2.height, scanning_state.sensor2.vertical_degree + 90);
  return sd;
}

struct SensorsData capture_sensors() {
  SensorsData sd;
  float volts = analogRead(sensor1) * 0.0048828125;
  sd.sensor1.distance = 13 * pow(volts, -1);
  return sd;
}

void send_scanning_data(SensorsData sensors_data) {
  if (LOGGING) {
    Serial.println(scanning_state.scene_angle);
    Serial.print(scanning_state.sensor1.height);
    Serial.print(",");
    Serial.print(scanning_state.sensor1.horizontal_degree);
    Serial.print(",");
    Serial.print(scanning_state.sensor1.vertical_degree);
    Serial.print(",");
    Serial.println(sensors_data.sensor1.distance);
    Serial.print(scanning_state.sensor2.height);
    Serial.print(",");
    Serial.print(scanning_state.sensor2.horizontal_degree);
    Serial.print(",");
    Serial.print(scanning_state.sensor2.vertical_degree);
    Serial.print(",");
    Serial.println(sensors_data.sensor2.distance);
  } else {
    Serial.write((uint8_t*)&scanning_state.scene_angle, 4);
    Serial.write((uint8_t*)&scanning_state.sensor1.height, 4);
    Serial.write((uint8_t*)&scanning_state.sensor1.horizontal_degree, 4);
    Serial.write((uint8_t*)&scanning_state.sensor1.vertical_degree, 4);
    Serial.write((uint8_t*)&sensors_data.sensor1.distance, 4);
    Serial.write((uint8_t*)&scanning_state.sensor2.height, 4);
    Serial.write((uint8_t*)&scanning_state.sensor2.horizontal_degree, 4);
    Serial.write((uint8_t*)&scanning_state.sensor2.vertical_degree, 4);
    Serial.write((uint8_t*)&sensors_data.sensor2.distance, 4);
  }
}

void process_sensors() {
  SensorsData sensors_data = capture_sensors();    
  send_scanning_data(sensors_data);
}

struct ScanningState get_new_scanning_state() {
  struct ScanningState s;
  return s;
}

void rotate_sensor2_horizontal(float degree) {
  if (LOGGING) {
    Serial.print("sensor 2 rotate horizontally on ");
    Serial.println(degree);
  }
  scanning_state.sensor2.horizontal_degree += degree;
}

void rotate_sensor2_vertical(float degree) {
  if (LOGGING) {
    Serial.print("sensor 2 rotate vertically on ");
    Serial.println(degree);
  }
  scanning_state.sensor2.vertical_degree += degree;
}

void raise_sensor2_height(float height) {
  if (LOGGING) {
    Serial.print("sensor 2 raise on ");
    Serial.println(height);
  }
  scanning_state.sensor2.height += height;
}

void rotate_sensor1_horizontal(float degree) {
  if (LOGGING) {
    Serial.print("sensor 1 rotate horizontally on ");
    Serial.println(degree);
  }
  scanning_state.sensor1.horizontal_degree += degree;
}

void rotate_sensor1_vertical(float degree) {
  if (LOGGING) {
    Serial.print("sensor 1 rotate vertically on ");
    Serial.println(degree);
  }
  scanning_state.sensor1.vertical_degree += degree;
}

void raise_sensor1_height(float height) {
  if (LOGGING) {
    Serial.print("sensor 1 raise on ");
    Serial.println(height);
  }
  scanning_state.sensor1.height += height;
}

void rotate_scene(float degree) {
  if (LOGGING) {
    Serial.print("rotate scene on ");
    Serial.println(degree);
  }
  scanning_state.scene_angle += degree;
}

byte scan_next_step() {
  if (((scanning_state).scene_angle + SCENE_ROTATION_STEP == MAX_SCENE_ROTATION) and
  ((scanning_state).sensor1.height + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT)) {
    return STOP;
  };
  if (scannig_direction & SCANNING_VERTICALLY) {
    if (scanning_state.sensor1.vertical_degree != MAX_SENSOR_VERTICAL_ROTATION) {
      rotate_sensor1_vertical(SENSOR_VERTICAL_ROTATION_STEP);
      rotate_sensor2_vertical(SENSOR_VERTICAL_ROTATION_STEP);
      return SCANNING;
    } else {
      reset_sensor1_vertical_angle();
      reset_sensor2_vertical_angle();
    }
  }
  if ((int)(scanning_state.scene_angle + SCENE_ROTATION_STEP) % (int)MAX_SCENE_ROTATION_STEP == 0) {
    if (scannig_direction & SCANNING_HORIZONTALLY) {
      reset_sensor1_horizontal_angle();
      reset_sensor2_horizontal_angle();
    };
    if ((scanning_state).sensor1.height + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT) {
      reset_sensor1_height();
      reset_sensor2_height();
      rotate_scene(+SCENE_ROTATION_STEP);  // по часовой
    } else {
      rotate_scene(-MAX_SCENE_ROTATION_STEP + SCENE_ROTATION_STEP);
      raise_sensor1_height(SENSOR_HEIGHT_STEP);
      raise_sensor2_height(-SENSOR_HEIGHT_STEP);
    }
    return SCANNING;  
  };
  rotate_scene(+SCENE_ROTATION_STEP);  // по часовой
  rotate_sensor1_horizontal(-SENSOR_HORIZONTAL_ROTATION_STEP); // в другую от стола
  rotate_sensor2_horizontal(-SENSOR_HORIZONTAL_ROTATION_STEP);
  return SCANNING; 
}

void stop_scanning() {
  // Serial.println("stop scanning");
}

void loop() {
  Scanner s;
  s.reset();
  if (Serial.available() > 0) {
    byte command = Serial.parseInt();
    // Serial.println(command);
    state = parse_command(command, state);
  }
  switch (state) {
    case START:
      state = SCANNING;
      scanning_state = get_new_scanning_state();
      reset_scan();
      break;
    case SCANNING:
      process_sensors();
      state = scan_next_step();
      break;
    case STOP:
      stop_scanning();
      state = PENDING;
      break;
    case PAUSE:
      {};
    case RESUME:
      state = SCANNING;
      break;
    case PENDING:
      {};
  }
}
