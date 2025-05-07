class RunningMean{
  private:
    int interval_ms;
    int last_val = 0;
    int current_val = 0;
    int current_measurements = 0;
    unsigned long current_period_ms = 0;
  public:
    int last_measurements = 0;
    RunningMean(int interval_ms)
      : interval_ms(interval_ms)
    {}
    RunningMean(int interval_ms, int init_val)
      : interval_ms(interval_ms)
      , last_val(init_val)
    {}
    bool update(){
      auto now = millis();
      if(now > current_period_ms + interval_ms){
        if (current_measurements > 0)
          last_val = current_val / current_measurements;
        else
          last_val = -1;
        last_measurements = current_measurements;
        current_measurements = 0;
        current_val = 0;
        current_period_ms = now;
        return true;
      }
      return false;
    }
    bool add(int val){
      bool had_update = update();
      current_measurements++;
      current_val += val;
      return had_update;
    }
    int get(){
      update();
      return last_val;
    }
};