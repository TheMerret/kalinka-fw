const byte SENSOR1_ID = 1;
const byte SENSOR2_ID = 2;

const byte START = 1;
const byte STOP = 0;
const byte SCANNING = 2;
const byte PAUSE = 3;
const byte RESUME = 4;

const float MAX_SCENE_ROTATION_STEP = 30.0;  // degree
const float MAX_SCENE_ROTATION = 90.0;
const float SENSOR_HEIGHT_STEP = 1.0;
const float SCENE_ROTATION_STEP = 1.0;
const float SENSOR_ROTATION_STEP = 1.0;
const float SENSOR_MAX_HEIGHT = 3.0;

byte state = STOP;

void setup() {
  Serial.begin(9600);
}

void reset_sensor2_height() {
  Serial.println("sensor 2 reset height");
}

void reset_sensor1_height() {
  Serial.println("sensor 1 reset height");
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
  Serial.println("reset 2 reset vertical rotation");
}

void reset_sensor2_horizontal_angle() {
  Serial.println("sensor 2 reset horizontal rotation");
}

void reset_sensor2_angles() {
  reset_sensor2_horizontal_angle();
  reset_sensor2_vertical_angle();
}

void reset_sensor1_vertical_angle() {
  Serial.println("reset 1 reset vertical rotation");
}

void reset_sensor1_horizontal_angle() {
  Serial.println("sensor 1 reset horizontal rotation");
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
  Serial.println("scene reset rotation");
}

void reset_scan() {
  reset_sensors();
  reset_scene_origin();

}

byte parse_command(byte command, byte prev_state) {
  byte new_state;
  switch (command) {
    case START:
      switch (prev_state) {
        case STOP:
          new_state = START;
      };
      break;
    case STOP:
      switch (prev_state) {
        case SCANNING:
          new_state = STOP;
      };
      break;
    case PAUSE:
      switch (prev_state) {
        case SCANNING:
          new_state = PAUSE;
      };
      break;
    case RESUME:
      switch (prev_state) {
        case PAUSE:
          new_state = RESUME;
      };
      break;
    default:
      Serial.println("error");
  };
  return new_state;
}

struct SensorState {
  float height = 0.0;
  float horizontal_degree = 0.0;
  float vertical_degree = 0.0;
};

struct SensorData {
  SensorState sensor_state;
  float distance;
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

struct SensorsData capture_sensors() {
  SensorsData sd;
  sd.sensor1.sensor_state.height = scanning_state.sensor1.height;
  sd.sensor1.sensor_state.horizontal_degree = scanning_state.sensor1.horizontal_degree;
  sd.sensor1.sensor_state.vertical_degree = scanning_state.sensor1.vertical_degree;
  float volts = analogRead(0) * (5/1024);
  Serial.println(volts);
  int distance = 13 * pow(volts, -1);
  sd.sensor1.distance = distance;
  return sd;
}

void send_sensors_data(SensorsData sensors_data) {
  Serial.print(sensors_data.sensor1.sensor_state.height);
  Serial.print(",");
  Serial.print(sensors_data.sensor1.sensor_state.horizontal_degree);
  Serial.print(",");
  Serial.print(sensors_data.sensor1.sensor_state.vertical_degree);
  Serial.print(",");
  Serial.println(sensors_data.sensor1.distance);
  Serial.print(sensors_data.sensor2.sensor_state.height);
  Serial.print(",");
  Serial.print(sensors_data.sensor2.sensor_state.horizontal_degree);
  Serial.print(",");
  Serial.print(sensors_data.sensor2.sensor_state.vertical_degree);
  Serial.print(",");
  Serial.println(sensors_data.sensor2.distance);
}

void process_sensors() {
  SensorsData sensors_data = capture_sensors();
  send_sensors_data(sensors_data);
}

struct ScanningState get_new_scanning_state() {
  struct ScanningState s;
  return s;
}

void rotate_sensor1_horizontal(float degree) {
  Serial.print("sensor 1 rotate horizontally on ");
  Serial.println(degree);
}

void rotate_scene(float degree) {
  Serial.print("rotate scene on ");
  Serial.println(degree);
}

void raise_sensor1_height(float height) {
  Serial.print("sensor 1 raise on ");
  Serial.println(height);
}

byte scan_next_step() {
  if (((scanning_state).scene_angle + SCENE_ROTATION_STEP == MAX_SCENE_ROTATION) and
  ((scanning_state).sensor1.height + SENSOR_HEIGHT_STEP > SENSOR_MAX_HEIGHT)) {
    return STOP;
  };
  rotate_scene(+SCENE_ROTATION_STEP);  // по часовой
  (scanning_state).scene_angle += SCENE_ROTATION_STEP;
  rotate_sensor1_horizontal(-SENSOR_ROTATION_STEP); // в другую от стола
  (scanning_state).sensor1.horizontal_degree -= SENSOR_ROTATION_STEP;
  if ((scanning_state).sensor1.height > SENSOR_MAX_HEIGHT) {
    reset_sensor1_horizontal_angle();
    (scanning_state).sensor1.height = 0.0;
    reset_sensor1_height();
    (scanning_state).sensor1.height = 0;
    rotate_scene(+MAX_SCENE_ROTATION_STEP);
    (scanning_state).scene_angle += MAX_SCENE_ROTATION_STEP;
  } else if ((int)scanning_state.scene_angle % (int)MAX_SCENE_ROTATION_STEP == 0) {
    reset_sensor1_horizontal_angle();
    (scanning_state).sensor1.horizontal_degree = 0.0;
    rotate_scene(-MAX_SCENE_ROTATION_STEP);
    (scanning_state).scene_angle -= MAX_SCENE_ROTATION_STEP;
    raise_sensor1_height(SENSOR_HEIGHT_STEP);
    (scanning_state).sensor1.height += SENSOR_HEIGHT_STEP;
  };
  return SCANNING; 
}

void stop_scanning() {
  Serial.println("stop scanning");
}

void loop() {
  if (Serial.available() > 0) {
    byte command = Serial.parseInt();
    Serial.println(command);
    state = parse_command(command, state);

  }
  switch (state) {
    case START:
      reset_scan();
      state = SCANNING;
      scanning_state = get_new_scanning_state();
      break;
    case SCANNING:
      process_sensors();
      state = scan_next_step();
      Serial.println(scanning_state.scene_angle);
      break;
    case STOP:
      stop_scanning();
      break;
//    case PAUSE:
//      pass;
    case RESUME:
      state = SCANNING;
      break;
  }
}
