
#include <Wire.h>
#include <stdlib.h>

#define I2C_ADDR    13
#define I2C_RX_BUFF_SIZE   (16)
#define I2C_TX_BUFF_SIZE   (16)

#define REG_ARRAY_LEN 20

typedef enum 
{
  MASTER_TO_SLAVE = 0,
  SLAVE_TO_MASTER 
} master_slave_direction_et;

typedef enum
{
  REG_ADDR_SET_WDT_TIMEOUT  = 0x00,
  REG_ADDR_SET_SLEEP_TIME   = 0x04,
  REG_ADDR_CLEAR_WATCHDOG   = 0x08,
  REG_ADDR_SWITCH_OFF       = 0x09,
  REG_ADDR_EEPROM_ADDR      = 0x0A,
  REG_ADDR_EEPROM_LOAD      = 0x0C,
  REG_ADDR_EEPROM_SAVE      = 0x0D,
  REG_ADDR_EEPROM_READ      = 0x10,
  REG_ADDR_EEPROM_WRITE     = 0x20,
  REG_ADDR_3 = 0x03,
  REG_ADDR_4 = 0x04,
  REG_ADDR_5 = 0x05,
} reg_addr_et;

typedef enum
{
  EEPROM_FUNC_READ        = 0x01,
  EEPROM_FUNC_WRITE       = 0x02,
} eeprom_func_et;

typedef enum
{
  EEPROM_STATUS_RD_READY    = 0x01,
  EEPROM_STATUS_WR_READY    = 0x02,
} eeprom_status_et;

typedef struct
{
  char      mode;
  uint8_t   addr;
  uint8_t   reg_addr;
  uint32_t  reg_data;
  uint8_t   reg_m2s;
  uint8_t   reg_s2m;
  uint8_t   tx_buff[I2C_TX_BUFF_SIZE];
  uint8_t   rx_buff[I2C_RX_BUFF_SIZE];

} i2c_st;

uint8_t tarr1[I2C_TX_BUFF_SIZE];
uint8_t tarr2[I2C_TX_BUFF_SIZE];

i2c_st i2c;

//uint8_t i2c_tx_buff[I2C_TX_BUFF_SIZE];

//String x = "";


void put_tx_buff_uint32( uint16_t offs, uint32_t u32)
{
    i2c.tx_buff[offs + 0 ] = (u32 >> 24) & 0x000000FF;
    i2c.tx_buff[offs + 1 ] = (u32 >> 16) & 0x000000FF;
    i2c.tx_buff[offs + 2 ] = (u32 >> 8) & 0x000000FF;
    i2c.tx_buff[offs + 3 ] = (u32 >> 0) & 0x000000FF;
}

void put_tx_buff_uint16( uint16_t offs, uint16_t u16)
{
    i2c.tx_buff[offs + 0 ] = (u16 >> 8) & 0x000000FF;
    i2c.tx_buff[offs + 1 ] = (u16 >> 0) & 0x000000FF;
}
void put_tx_buff_uint8( uint16_t offs, uint8_t u8)
{
    i2c.tx_buff[offs + 0 ] = (u8 >> 0) & 0x000000FF;
}






void edog_send_receive(void)
{
  Serial.print("Send data.."); Serial.flush();

  Wire.beginTransmission(i2c.addr); 
  Wire.write( i2c.tx_buff, i2c.reg_m2s + 1)  ;      
  Wire.endTransmission();

  Serial.println("Data sent to ATtiny Device: ");
  //Serial.println(buffer);
  if(i2c.reg_s2m > 0)
  {
    // delay(1);
    Wire.requestFrom(i2c.addr, i2c.reg_s2m);   
    uint8_t i = 0; 
    while(Wire.available())    
    { 
      int c = Wire.read();
      i2c.rx_buff[i] = (uint8_t) i;
      // Serial.print("Read: ");  Serial.println(c,HEX); 
    }
  }

}

void print_hex_array(uint8_t *arr, uint8_t nbr)
{
  for (uint8_t i = 0; i< nbr; i++)
  {
    if (arr[i] < 0x10) Serial.print(0,HEX);
    Serial.print(arr[i],HEX); Serial.print(" ");
  }
  Serial.println("");
}

void edog_build_uint_msg(uint8_t raddr, uint32_t value, uint8_t m2s, uint8_t s2m)
{
  uint8_t aindx = 0;    //[0] is reserved for register address
  i2c.reg_addr = raddr;
  i2c.reg_data = value;
  i2c.reg_m2s = m2s;
  i2c.reg_s2m = s2m;

  i2c.tx_buff[aindx++] = i2c.reg_addr;
  if (i2c.reg_m2s == 4)
  {
    i2c.tx_buff[aindx++] = (i2c.reg_data >> 24) & 0x000000FF;
    i2c.tx_buff[aindx++] = (i2c.reg_data >> 16) & 0x000000FF;
  }
  if (i2c.reg_m2s > 1)
  {
    i2c.tx_buff[aindx++] = (i2c.reg_data >> 8) & 0x000000FF;
  }
  i2c.tx_buff[aindx] = (i2c.reg_data >> 0) & 0x000000FF;
}

void edog_build_array_msg(uint8_t raddr, uint8_t *arr, uint8_t m2s, uint8_t s2m)
{
  uint8_t aindx = 0;    //[0] is reserved for register address
  i2c.reg_addr = raddr;
  i2c.reg_m2s = m2s;
  i2c.reg_s2m = s2m;

  i2c.tx_buff[aindx++] = i2c.reg_addr;
  for (uint8_t i = 0; i < m2s; i++)
  {
    i2c.tx_buff[aindx++] = arr[i];
  }
}


void edog_set_wd_timeout(uint32_t wd_timeout)
{
  Serial.print("Watchdog timeout = "); Serial.println(wd_timeout);
  edog_build_uint_msg(REG_ADDR_SET_WDT_TIMEOUT, wd_timeout, 4, 4);
  edog_send_receive();
}

void edog_set_sleep_time(uint32_t sleep_time)
{
  Serial.print("Sleep time = "); Serial.println(sleep_time);
  edog_build_uint_msg(REG_ADDR_SET_SLEEP_TIME, sleep_time, 4, 4);
  edog_send_receive();
}

void edog_clear_watchdog(void)
{
  Serial.println("Clear watchdogSleep time = ");
  edog_build_uint_msg(REG_ADDR_CLEAR_WATCHDOG, 1, 1, 0);
  edog_send_receive();
 }

void edog_load_eeprom(void)
{
  Serial.println("Load EEPROM Data");
  edog_build_uint_msg(REG_ADDR_EEPROM_LOAD, 1, 1, 0);
  edog_send_receive();
 }

void edog_save_eeprom(void)
{
  Serial.println("Save EEPROM Data");
  edog_build_uint_msg(REG_ADDR_EEPROM_SAVE, 1, 1, 0);
  edog_send_receive();
 }


void edog_read_eeprom(uint16_t addr)
{
  Serial.println("Read EEPROM @ "); Serial.println(addr,HEX);

  edog_build_uint_msg(REG_ADDR_EEPROM_ADDR, (uint32_t) addr, 2, 2);
  edog_send_receive();
  delay(1);

  edog_build_uint_msg(REG_ADDR_EEPROM_LOAD, 0, 0, 0);
  edog_send_receive();

  edog_build_uint_msg(REG_ADDR_EEPROM_READ, 0, 0, 8);
  edog_send_receive();
  print_hex_array(i2c.rx_buff,8);
 }

void edog_write_eeprom(uint16_t addr, uint8_t *arr)
{
  Serial.println("Write EEPROM @ "); Serial.println(addr,HEX);

  edog_build_uint_msg(REG_ADDR_EEPROM_ADDR, (uint32_t) addr, 2, 2);
  edog_send_receive();
  
  delay(1);
  edog_build_array_msg(REG_ADDR_EEPROM_WRITE, arr, 8, 0);
  edog_send_receive();

  edog_build_uint_msg(REG_ADDR_EEPROM_SAVE, 0, 0, 0);
  edog_send_receive();

  print_hex_array(i2c.tx_buff,8);
 }


void setup() {
  Wire.begin();
  Wire.setClock(10000);
  Serial.begin(9600); 
  delay(3000);
  Serial.println("\n\nMain program is starting...");

  for( uint8_t i = 0; i < 8; i++)
  {
    tarr1[i] = i*2;
    tarr2[i] = i*3;
  }

  i2c.addr = I2C_ADDR;
}


void loop()
{
  //edog_set_wd_timeout(2000);
  delay(1);
  edog_write_eeprom(0x80, tarr1);
  //edog_save_eeprom();
  edog_read_eeprom(0x80);
  //edog_load_eeprom();

  //edog_set_sleep_time(10000);
  delay(10000);
  while (true)
  {
    edog_clear_watchdog();
    delay(10000);
  }
}



