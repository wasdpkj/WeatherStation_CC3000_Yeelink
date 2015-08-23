String buf;

void yeelink_add_head()
{
  buf="";
  /*
  for(int a=0;a<buf.length();a++)
    buf[a]=NULL;
  */
}

void yeelink_add_body()
{
  buf.setCharAt(0,'[');
  buf += ']';
}

void yeelink_add_NOR(char* _id,float _value_normal)
{
  char c_sensor[12];
  dtostrf(_value_normal,4,1,c_sensor);  //将获取的数值转换为字符型数组       

  char buf_cache[64];
  sprintf(buf_cache, "{\"sensor_id\":%s,\"value\":%s}",_id,c_sensor);

  buf += ',';
  buf += buf_cache;
}

void yeelink_add_GPS(char* _id,float _value_gps_lat,float _value_gps_lng,float _value_gps_spd)
{
  char c_latitude[20],c_longitude[20],c_speed[10];
  dtostrf(_value_gps_lat,4,6,c_latitude);   //将获取的数值转换为字符型数组
  dtostrf(_value_gps_lng,4,6,c_longitude);  //将获取的数值转换为字符型数组
  dtostrf(_value_gps_spd,4,1,c_speed);      //将获取的数值转换为字符型数组          

  char buf_cache[128];
  sprintf(buf_cache, "{\"sensor_id\":%s,\"value\":{\"lat\":%s,\"lng\":%s,\"speed\":%s}}",_id,c_latitude,c_longitude,c_speed);

  buf += ',';
  buf += buf_cache;
}

void yeelink_begin()
{
  //BUF----------------------
  yeelink_add_head();

  Serial.println(buf);
  yeelink_add_NOR(net_sensor[0],sensor_tem);
  Serial.println(buf);
  yeelink_add_NOR(net_sensor[1],sensor_hum);
  Serial.println(buf);
  yeelink_add_NOR(net_sensor[2],sensor_pre);
  Serial.println(buf);
  yeelink_add_NOR(net_sensor[3],sensor_lux);
  Serial.println(buf);
  yeelink_add_GPS(net_sensor[5],f_latitude,f_longitude,f_speed);
  Serial.println(buf);

  yeelink_add_body();
#ifdef DEBUG
  Serial.println(buf);
#endif
}
