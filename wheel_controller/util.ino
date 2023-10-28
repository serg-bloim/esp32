int cap(int v, int min, int max){
  if(v < min){
    return min;
  } else if (v > max) {
    return max;
  } else{
    return v;
  }
}