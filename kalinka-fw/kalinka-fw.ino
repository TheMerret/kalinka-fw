const byte SENSOR1_ID = 1;
const byte SENSOR2_ID = 2;

const byte START = 1;
const byte STOP = 0;
const byte SCANNING = 2;
const byte PAUSE = 3;
const byte RESUME = 4;

byte state = STOP;

void setup() {
  Serial.begin(9600);
}

void reset_sensor2_height() {
  Serial.println("сенсор 2 сброс высоты");
}

void reset_sensor1_height() {
  Serial.println("сенсор 1 сброс высоты");
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
  Serial.println("сенсор 2 сброс вертикального поворота");
}

void reset_sensor2_horizontal_angle() {
  Serial.println("сенсор 2 сброс горизонтального поворота");
}

void reset_sensor2_angles() {
  reset_sensor2_horizontal_angle();
  reset_sensor2_vertical_angle();
}

void reset_sensor1_vertical_angle() {
  Serial.println("сенсор 1 сброс вертикального поворота");
}

void reset_sensor1_horizontal_angle() {
  Serial.println("сенсор 1 сброс горизонтального поворота");
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
  Serial.println("стол сброс поворота");
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

struct SensorsData capture_sensors() {
  return (SensorsData) {};
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

struct ScanningState {
  float scene_angle = 0.0;
  SensorState sensor1;
  SensorState sensor2;
};

struct ScanningState get_new_scanning_state() {
  struct ScanningState s;
  return s;
}

void scan_next_step(ScanningState scanning_state) {
  Serial.println("следующий шаг сканирования");
}

void stop_scanning() {
  Serial.println("стоп сканирование");
}

ScanningState scanning_state;

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
      scan_next_step(scanning_state);
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
