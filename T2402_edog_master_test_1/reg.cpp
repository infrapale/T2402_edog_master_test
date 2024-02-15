
/*

uint8_t get_msg_len(reg_addr_et reg_addr, master_slave_direction_et master_slave)
{
  uint8_t m2s_bytes = 0;
  uint8_t s2m_bytes = 0;
  switch(reg_addr)
  {
    case REG_ADDR_SET_WDT_TIMEOUT:
      m2s_bytes = 4;
      s2m_bytes = 4;
      break;
    case REG_ADDR_SET_OFF_TIME:
      m2s_bytes = 4;
      s2m_bytes = 4;
      break;
    case REG_ADDR_CLEAR_WATCHDOG:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_SWITCH_OFF:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_ADDR:
      m2s_bytes = 2;
      s2m_bytes = 2;
      break;
    case REG_ADDR_EEPROM_FUNC:
      m2s_bytes = 1;
      s2m_bytes = 0;
      break;
    case REG_ADDR_EEPROM_STATUS:
      m2s_bytes = 0;
      s2m_bytes = 1;
      break;
    case REG_ADDR_EEPROM_READ:
      m2s_bytes = 0;
      s2m_bytes = 8;
      break;
    case REG_ADDR_EEPROM_WRITE:
      m2s_bytes = 8;
      s2m_bytes = 0;
      break;
    default:
      m2s_bytes = 0;
      s2m_bytes = 0;
      break;
  }
  if (master_slave == MASTER_TO_SLAVE) return m2s_bytes;
  else return s2m_bytes;
}
*/