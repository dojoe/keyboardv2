#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#define OW_MAX_XFER 32

enum ow_state_e { OW_TX, OW_RX, OW_IDLE, OW_ERROR };

extern uint8_t ow_buffer[OW_MAX_XFER];
extern volatile enum ow_state_e ow_state;

void ow_init(void);
void ow_reset(void);
void ow_start(uint8_t write_size, uint8_t read_size, void *read_buf);
void ow_disconnect(void);
uint8_t ow_wait(void);

static inline uint8_t ow_done(void)
{
	return ow_state >= OW_IDLE;
}

static inline uint8_t ow_error(void)
{
	return ow_state == OW_ERROR;
}

#define ow_write(size) ow_start(size, 0, 0)
#define ow_read(write_size, read_size) ow_start(write_size, read_size, 0)
#define ow_read_buf(write_size, read_size, buf) ow_start(write_size, read_size, buf)

#endif /* ONEWIRE_H_ */
