class RunningMean {
private:
  int interval_ms;
  int last_val = 0;
  int last_min = INT_MAX;
  int last_max = INT_MIN;

  int current_val = 0;
  int current_min = INT_MAX;
  int current_max = INT_MIN;
  int current_measurements = 0;
  unsigned long current_period_ms = 0;
public:
  int last_measurements = 0;
  RunningMean(int interval_ms)
    : interval_ms(interval_ms) {}
  RunningMean(int interval_ms, int init_val)
    : interval_ms(interval_ms), last_val(init_val) {}
  bool update() {
    auto now = millis();
    if (now > current_period_ms + interval_ms) {
      if (current_measurements > 0) {
        // if no measurements, keep the last value
        last_val = current_val / current_measurements;
        last_max = current_max;
        last_min = current_min;
      }
      last_measurements = current_measurements;
      current_measurements = 0;
      current_val = 0;
      current_min = INT_MAX;
      current_max = INT_MIN;
      current_period_ms = now;
      return true;
    }
    return false;
  }
  bool add(int val) {
    bool had_update = update();
    current_measurements++;
    current_val += val;
    if (val > current_max) current_max = val;
    if (val < current_min) current_min = val;

    return had_update;
  }
  int get() {
    update();
    return last_val;
  }
  int get_min() {
    return last_min;
  }
  int get_max() {
    return last_max;
  }
};