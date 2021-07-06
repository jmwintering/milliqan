bool equalDoubles(double a, double b, double epsilon = 1e-320){
    return abs(a - b) <= epsilon;
}

int checkIntervalOverlap(double min1Temp, double max1Temp, double min2Temp, double max2Temp) {
  // 0 == intervals don't overlap
  // 1 == intervals overlap but not contained within each other
  // 2 == interval 2 inside 1
  // 3 == interval 1 inside 2
  // 4 == same interval 

  double min1(min1Temp), max1(max1Temp), min2(min2Temp), max2(max2Temp);
  // if intervals not ordered, order them:
  if(max1 < min1){ min1Temp = max1; max1Temp = min1; }
  if(max2 < min2){ min2Temp = max2; max2Temp = min2; }

  int intervalStatus = 0;

  // check if either interval is a point:
  bool point1 = equalDoubles(min1, max1);
  bool point2 = equalDoubles(min2, max2);

  cout << point1 << " " << point2 << endl;

  if(point1 && !point2){
    if(min1 > min2 && min1 < max2) intervalStatus = 1;
    if(equalDoubles(min1, min2)||equalDoubles(min1, max2)) intervalStatus = 4;
  }
  else if(!point1 && point2){
    if(min2 > min1 && min2 < max1) intervalStatus = 1;
    if(equalDoubles(min2, min1)||equalDoubles(min2, max1)) intervalStatus = 4;
  }
  else if(point1 && point2){
    if(equalDoubles(min1, min2)) intervalStatus = 4;
  }
  else{
    // intervals overlap but not inside
    if(equalDoubles(max1, min2) &&  min1 < min2 && max2 > min2) intervalStatus = 1;
    else if(equalDoubles(max2, min1) && min2 < min1) intervalStatus = 1;
    else if(min1 > min2 && min1 < max2 && max1 > max2) intervalStatus = 1;
    else if(min2 > min1 && min2 < max1 && max2 > max1) intervalStatus = 1;

    // interval 2 inside 1
    if(min2 > min1 && min2 < max1 && max2 > min1 && max2 < max1) intervalStatus = 2;
    else if(equalDoubles(min1, min2) && max2 < max1) intervalStatus = 2;
    else if(equalDoubles(max1, max2) && min2 > min1) intervalStatus = 2;

    // interval 1 inside 2
    if(min1 > min2 && min1 < max2 && max1 > min2 && max1 < max2) intervalStatus = 3;
    else if(equalDoubles(min1, min2) && max1 < max2) intervalStatus = 3;
    else if(equalDoubles(max1, max2) && min1 > min2) intervalStatus = 3;

    // intervals equal
    if(equalDoubles(min1, min2) && equalDoubles(max1, max2)) intervalStatus = 4;
  }
  
  return intervalStatus;
}