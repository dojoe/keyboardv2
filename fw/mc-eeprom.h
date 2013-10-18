#ifndef MC_EEPROM_H_
#define MC_EEPROM_H_

typedef void (*eep_callback)(int success);

enum eep_prot_e {
	EEP_PROT_NONE          = 0x0,
	EEP_PROT_UPPER_QUARTER = 0x4,
	EEP_PROT_UPPER_HALF    = 0x8,
	EEP_PROT_ALL           = 0xC,
};

enum eep_erase_e {
	EEP_ERASE_00 = 0x6D,
	EEP_ERASE_FF = 0x67,
};

void eep_read(uint16_t addr, uint16_t size, void *buf, eep_callback cb);
void eep_write(uint16_t addr, uint16_t size, void *buf, eep_callback cb);
void eep_erase(uint8_t erase_value, eep_callback cb);
void eep_protect(uint8_t protbits, eep_callback cb);

void eep_poll(void);

#endif /* MC_EEPROM_H_ */
