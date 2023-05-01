void gpsdump(TinyGPS &gps)    // function for displaying day of the week
{
  gps.get_position(&lat, &lon);
  Serial.print("Lat/Long(10^5): "); Serial.print(lat); Serial.print(", "); Serial.println(lon);           // Uncomment to print live Latitudes & Longtitudes
}

void dTime()                  // function for time covert to Local time
{
  // get time from gpsp module
  if (gpsp.time.isValid())
  {
    Minute = gpsp.time.minute();
    Second = gpsp.time.second();
    Hour   = gpsp.time.hour();
  }

  // get date drom gpsp module
  if (gpsp.date.isValid())
  {
    Day   = gpsp.date.day();
    Month = gpsp.date.month();
    Year  = gpsp.date.year();
  }

  if (last_second != gpsp.time.second()) // if time has changed
  {
    last_second = gpsp.time.second();

    // set current UTC time
    setTime(Hour, Minute, Second, Day, Month, Year);
    // add the offset to get local time
    adjustTime(time_offset);

    // update time array
    Time[12] = second() / 10 + '0';
    Time[13] = second() % 10 + '0';
    Time[9]  = minute() / 10 + '0';
    Time[10] = minute() % 10 + '0';
    Time[6]  = hour()   / 10 + '0';
    Time[7]  = hour()   % 10 + '0';

    // update date array
    Date[14] = (year()  / 10) % 10 + '0';
    Date[15] =  year()  % 10 + '0';
    Date[9]  =  month() / 10 + '0';
    Date[10] =  month() % 10 + '0';
    Date[6]  =  day()   / 10 + '0';
    Date[7]  =  day()   % 10 + '0';
  }
}

void print_wday(byte wday)          // function for displaying day of the week
{
  lcd.setCursor(4, 0);  // move cursor to column 5, row 1
  switch (wday)
  {
    case 1:  lcd.print(" SUNDAY  ");   break;
    case 2:  lcd.print(" MONDAY  ");   break;
    case 3:  lcd.print(" TUESDAY ");   break;
    case 4:  lcd.print("WEDNESDAY");   break;
    case 5:  lcd.print("THURSDAY ");   break;
    case 6:  lcd.print(" FRIDAY  ");   break;
    default: lcd.print("SATURDAY ");
  }
}

void statCheck()                     // function for Station Checking process
{
  for (int i = 1; i <= Number_of_Stations; i++)
  {
    if (((Lat[2 * i - 2] < lat) && (lat < Lat[2 * i - 1])) && ((Lon[2 * i - 2] < lon) && (lon < Lon[2 * i - 1])))
    {
      Return = i;
      if (control != i)
      {
        ReturnList[count] = Return;
        count++;
        control = Return;
      }
    }
  }

  if ((count == 1) && (control == 1))
  {
    for (int j = 0; j < Number_of_Stations; j++)
    {
      sort[j] = j;
    }
  }
  else if ((count == 1) && (control == Number_of_Stations))
  {
    for (int j = 0; j < Number_of_Stations; j++)
    {
      sort[j] = Number_of_Stations - j - 1;
    }
  }
}
