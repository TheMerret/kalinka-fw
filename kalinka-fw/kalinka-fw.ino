#include <math.h>

#define sensor1 A0
#define sensor2 A1


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
const float MAX_SENSOR_VERTICAL_ROTATION = 45.0;
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
  // Serial.println("sensor 2 reset height");
  scanning_state.sensor2.height = SENSOR_MAX_HEIGHT;
}

void reset_sensor1_height() {
  // Serial.println("sensor 1 reset height");
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
  // Serial.println("reset 2 reset vertical rotation");
  float degree;
  if (scannig_direction & SCANNING_VERTICALLY) {
    degree = -MAX_SENSOR_VERTICAL_ROTATION;
  } else {
    degree = 0.0;
  }
  scanning_state.sensor2.vertical_degree = degree;
}

void reset_sensor2_horizontal_angle() {
  // Serial.println("sensor 2 reset horizontal rotation");
  scanning_state.sensor2.horizontal_degree = 0;
}

void reset_sensor2_angles() {
  reset_sensor2_horizontal_angle();
  reset_sensor2_vertical_angle();
}

void reset_sensor1_vertical_angle() {
  // Serial.println("reset 1 reset vertical rotation");
  float degree;
  if (scannig_direction & SCANNING_VERTICALLY) {
    degree = -MAX_SENSOR_VERTICAL_ROTATION;
  } else {
    degree = 0.0;
  }
}

void reset_sensor1_horizontal_angle() {
  // Serial.println("sensor 1 reset horizontal rotation");
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
  // Serial.println("scene reset rotation");
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

float polar_square_function(float degree) {
    degree = degree * (3.14159265359 / 180);
    return 4 - min(3 / 2 / abs(cos(degree)), 3 / 2 / abs(sin(degree)));
}

float get_distance_to_cube(float degrees, float height, float horizontal_degree, float vertical_degree) {
    horizontal_degree = horizontal_degree * (3.14159265359 / 180);
    vertical_degree = vertical_degree * (3.14159265359 / 180);
    float d = polar_square_function(degrees);
    float distance = d / sin(horizontal_degree) / cos(vertical_degree) / 4;
    return distance;
}

struct SensorsData capture_sensors_test() {
  SensorsData sd;
  sd.sensor1.distance = get_distance_to_cube(scanning_state.scene_angle, scanning_state.sensor1.height, scanning_state.sensor1.horizontal_degree, scanning_state.sensor1.vertical_degree);
  sd.sensor2.distance = get_distance_to_cube(scanning_state.scene_angle + 180.0, scanning_state.sensor2.height, scanning_state.sensor2.horizontal_degree, scanning_state.sensor2.vertical_degree);
  return sd;
}

struct SensorsData capture_sensors() {
  SensorsData sd;
  float volts = analogRead(sensor1) * 0.0048828125;
  sd.sensor1.distance = 13 * pow(volts, -1);
  return sd;
}

void send_scanning_data(SensorsData sensors_data) {
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

void process_sensors() {
  SensorsData sensors_data = capture_sensors_test();    
  send_scanning_data(sensors_data);
}

struct ScanningState get_new_scanning_state() {
  struct ScanningState s;
  return s;
}

void rotate_sensor2_horizontal(float degree) {
  // Serial.print("sensor 2 rotate horizontally on ");
  // Serial.println(degree);
  scanning_state.sensor2.horizontal_degree += degree;
}

void rotate_sensor2_vertical(float degree) {
  // Serial.print("sensor 2 rotate vertically on ");
  // Serial.println(degree);
  scanning_state.sensor2.vertical_degree += degree;
}

void raise_sensor2_height(float height) {
  // Serial.print("sensor 2 raise on ");
  // Serial.println(height);
  scanning_state.sensor2.height += height;
}

void rotate_sensor1_horizontal(float degree) {
  // Serial.print("sensor 1 rotate horizontally on ");
  // Serial.println(degree);
  scanning_state.sensor1.horizontal_degree += degree;
}

void rotate_sensor1_vertical(float degree) {
  // Serial.print("sensor 1 rotate vertically on ");
  // Serial.println(degree);
  scanning_state.sensor1.vertical_degree += degree;
}

void raise_sensor1_height(float height) {
  // Serial.print("sensor 1 raise on ");
  // Serial.println(height);
  scanning_state.sensor1.height += height;
}

void rotate_scene(float degree) {
  // Serial.print("rotate scene on ");
  // Serial.println(degree);
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
  rotate_scene(+SCENE_ROTATION_STEP);  // по часовой
  rotate_sensor1_horizontal(-SENSOR_HORIZONTAL_ROTATION_STEP); // в другую от стола
  rotate_sensor2_horizontal(-SENSOR_HORIZONTAL_ROTATION_STEP);
  if ((scanning_state).sensor1.height > SENSOR_MAX_HEIGHT) {
    if (scannig_direction & SCANNING_HORIZONTALLY) {
      reset_sensor1_horizontal_angle();
      reset_sensor2_horizontal_angle();
    }
    reset_sensor1_height();
    reset_sensor2_height();
    rotate_scene(+MAX_SCENE_ROTATION_STEP);
  } else if ((int)scanning_state.scene_angle % (int)MAX_SCENE_ROTATION_STEP == 0) {
    if (scannig_direction & SCANNING_HORIZONTALLY) {
      reset_sensor1_horizontal_angle();
      reset_sensor2_horizontal_angle();
    }
    reset_sensor1_horizontal_angle();
    reset_sensor2_horizontal_angle();
    rotate_scene(-MAX_SCENE_ROTATION_STEP);
    raise_sensor1_height(SENSOR_HEIGHT_STEP);
    raise_sensor2_height(-SENSOR_HEIGHT_STEP);
  };
  return SCANNING; 
}

void stop_scanning() {
  // Serial.println("stop scanning");
}

void loop() {
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
