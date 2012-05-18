#define PORT_ADDRESS_BUFFER_LENGTH		255
#define MAX_PROTOCOL_UPDATE_DELAY		500 //ms
#define START_ACK_TIMEOUT_MS			2000
#define DATA_TIMEOUT_MS					200
#define DATA_TIMEOUT_MS_FIRST_LOOP		1000
#define DATA_STOP_TIMEOUT				5000

#define ONES_IN_BYTE(x)	(x&(1<<7)) + (x&(1<<6)) + (x&(1<<5)) + (x&(1<<4)) + (x&(1<<3)) + (x&(1<<2)) + (x&(1<<1)) + (x&1)

#define NUM_RETRIES_START				3