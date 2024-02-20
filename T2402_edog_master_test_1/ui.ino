
bool read_hex_array(uint8_t *arr, uint8_t nbr)
{
  String StrArray;
  String StrByte;
  uint8_t arrindx = 0;
  bool do_continue = true;
  char buff[4];
  int   pos1 = 0;
  int   pos2 = 0;

  Serial.println("Enter Hex String, example AB CD 12 34 ");
  while(Serial.available() == 0) {}
  StrArray = Serial.readString();
  StrArray.trim(); StrArray.toUpperCase();
  Serial.print(StrArray); Serial.print(" len="); Serial.println(StrArray.length());

  pos1 = 0; pos2 = 0;
  
  if (StrArray.length() < (nbr * 3)-1)
  {
    Serial.print("Incorrect string length: "); Serial.println(StrArray.length());
    do_continue = false;
  }
 
  if (do_continue)
  {
    while(arrindx < nbr)
    {
      pos1 = (arrindx)*3; pos2 = pos1+2;
      StrByte = StrArray.substring(pos1,pos2);
      StrByte.toCharArray(buff, 4);
      long val =strtol(buff, NULL, 16);
      Serial.print(val);
      arr[arrindx] = (uint8_t) val;
      Serial.print("-");
      ++arrindx;
    }
    Serial.println("");
    return true;
  }
  else
  {
    return false;
  } 
}


uint32_t read_u32(void)
{
  String StrValue;
  uint32_t u32;

  while(Serial.available() == 0) {}
  StrValue = Serial.readString();
  StrValue.trim();
  u32 = (uint32_t)StrValue.toInt();
  Serial.println(u32);
  return u32;
}


uint8_t read_u16(void)
{  
  uint16_t u16;
  uint32_t u32 = read_u32();
  if (u32 > 255) u32 = 0;
  u16 = (uint16_t)u32;
  return u16;
}

uint8_t read_u8(void)
{  
  uint8_t u8;
  uint32_t u32 = read_u32();
  if (u32 > 255) u32 = 0;
  u8 = (uint8_t)u32;
  return u8;
}

char read_one_char()
{
  String StrValue;
  while(Serial.available() == 0) {}
  StrValue = Serial.readString();
  StrValue.trim(); StrValue.toUpperCase();
  return StrValue.charAt(0);
}

void loop_xx() 
{
  static uint8_t state = 0;
  static uint8_t inp_array[REG_ARRAY_LEN];

  static uint8_t array_index;
  

  static char     mode = 'X';
  static uint8_t  reg_addr = 0;
  static uint32_t reg_data = 0;
  static uint8_t  reg_m2s = 0;
  static uint8_t  reg_s2m = 0;

  //int   pos1 = 0;
  //int   pos2 = 0;
  switch(state)
  {
    case 0:
      array_index = 0;  //leave inp_array[0] for register address
      state = 5;
      break;
    case 5:
      Serial.print("\nEnter Register address: ");
      reg_addr = read_u8();
      inp_array[array_index++] = reg_addr;
      state = 10;
      break;
    case 10:
      Serial.print("Enter M2S bytes: ");
      reg_m2s = read_u8();
      state++;
      break;
    case 11:
      Serial.print("Enter S2M bytes: ");
      reg_s2m = read_u8();
      state = 20;
      break;
    case 20:
      Serial.print("\nSelect Mode: Array Input: A,  Integer I: ");
      mode = read_one_char();
      Serial.println(mode);
      switch(mode)
      {
        case 'A': state = 40; break;
        case 'I': state = 30; break;
        default: Serial.println("incorrect mode");
      }
      break;
    case 30:
      Serial.print("State..."); Serial.println(state);
      Serial.print("Enter Decimal Data: ");
      reg_data = read_u32();
      Serial.print(reg_data); Serial.print(" HEX= "); Serial.println(reg_data,HEX);
      if (reg_m2s == 4)
      {
        inp_array[array_index++] = (reg_data >> 24) & 0x000000FF;
        inp_array[array_index++] = (reg_data >> 16) & 0x000000FF;
      }
      if (reg_m2s > 1)
      {
        inp_array[array_index++] = (reg_data >> 8) & 0x000000FF;
      }
      inp_array[array_index++] = (reg_data >> 0) & 0x000000FF;
      state = 50;
      break;
     case 40:
      if (read_hex_array(&inp_array[1],reg_m2s ) ) state = 50;
      else state = 0;
    case 50: 
      Serial.print("State="); Serial.println(state);
      Serial.print("Send data..");
      Serial.flush();
      Wire.beginTransmission(I2C_ADDR); 
      Wire.write( inp_array, reg_m2s+1)  ;      
      Wire.endTransmission();

      Serial.println("Data sent to ATtiny Device: ");
      //Serial.println(buffer);
      if(reg_s2m > 0)
      {
        // delay(1);
        Wire.requestFrom((uint8_t)I2C_ADDR, reg_s2m);    
        while(Wire.available())    
        { 
          int c = Wire.read();
          Serial.print("Read: ");
          Serial.println(c,HEX); 
        }
      }
      state = 60;
      break;  
    case 60:
      //state++;
      break;
    case 70:
      state++;
      break;
    case 80:
      state++;
      break;
    case 90:
      state++;
      break;
    case 100:
      Serial.println("Transmitting...");
      state++;
      break;
    default:
      Serial.print("Incorrect state "); Serial.println(state);

  }




}

void send_receive_i2c(void)
{
   bool new_command = false;
  reg_addr_et reg;
  uint8_t     m2s_bytes;
  uint8_t     s2m_bytes;
  char buffer[8];

  if (new_command)
  {
    //x.toCharArray(buffer, 4);
    m2s_bytes = 0;
    s2m_bytes = 0;
    switch(buffer[0])
    {
      case '1':
        reg = REG_ADDR_SET_WDT_TIMEOUT;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint32(1, 0x01234567);
        put_tx_buff_uint32(5, 0x89ABCDEF);
        break;
      case '2':
        reg = REG_ADDR_SET_SLEEP_TIME;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint32(1, 10000);
        break;
      case '3':
        reg = REG_ADDR_CLEAR_WATCHDOG;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint8(1, 1);
        break;
      case '4':
        reg = REG_ADDR_SWITCH_OFF;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint8(1, 1);
        break;
      case '5':
        reg = REG_ADDR_EEPROM_ADDR;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint16(1, 0x0080);
        break;
      case '6':
        reg = REG_ADDR_EEPROM_READ;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint16(1, 0x0080);
        break;
      case '7':
        reg = REG_ADDR_EEPROM_WRITE;
        put_tx_buff_uint8(0, reg);
        put_tx_buff_uint32(1, 0x01234567);
        put_tx_buff_uint32(5, 0x89ABCDEF);
        break;


    }

    //m2s_bytes = get_msg_len(reg, MASTER_TO_SLAVE);
    //s2m_bytes = get_msg_len(reg, SLAVE_TO_MASTER);

  
    {
      Wire.beginTransmission(I2C_ADDR); 
      Wire.write(i2c.tx_buff, m2s_bytes+1)  ;      
      Wire.endTransmission();
    }

    Serial.print("Data sent to ATtiny Device: ");
    Serial.println(buffer);
    if(s2m_bytes > 0)
    {
      // delay(1);
      Wire.requestFrom((uint8_t)I2C_ADDR, s2m_bytes);    
      while(Wire.available())    
      { 
        int c = Wire.read();
        Serial.print("Read: ");
        Serial.println(c,HEX); 
      }
    }
  }
  delay(50);
}



