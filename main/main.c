#include "legato.h"

#define N_UARTS 2
#define GPIO_START 21
#define GPIO_END 37
#define BUFFER_LEN 256

static const char* TTY_AT_PATH = "/dev/ttyAT";

static int configureGpio (int fd, char* buffer, bool* res) {
  int resIndex = 0;
  for (int i = GPIO_START; i <= GPIO_END; i++) {
    int strSize = sprintf(buffer, "AT+WIOCFG=%d,16\r", i);
    LE_INFO("Running AT command on GPIO %d (%s)", i, buffer);
    write(fd, buffer, strSize);
    int nRead = read(fd, buffer, BUFFER_LEN);
    buffer[nRead + 1] = '\0';
    LE_INFO("Got %s response for GPIO %d", buffer, i);
    // if the return string matches OK
    // we can consider this one a success
    res[resIndex++] = strcmp(buffer, "OK") == 0;
  }
  return resIndex;
}

static int configureUart (int fd, char* buffer, bool* res) {
  int resIndex = 0;
  for (int i = 1; i <= N_UARTS; i++) {
    int strSize = sprintf(buffer, "AT!MAPUART=17,%d\r", i);
    LE_INFO("Running AT command on UART %d (%s)", i, buffer);
    write(fd, buffer, strSize);
    int nRead = read(fd, buffer, BUFFER_LEN);
    buffer[nRead + 1] = '\0';
    LE_INFO("Got %s response for UART %d", buffer, i);
    // if the return string matches OK
    // we can consider this one a success
    res[resIndex++] = strcmp(buffer, "OK") == 0;
  }
  return resIndex;
}

COMPONENT_INIT {
  char buffer[BUFFER_LEN];
  bool res[BUFFER_LEN];
  int serialFd = le_tty_Open(TTY_AT_PATH, O_RDWR);
  le_tty_SetBaudRate(serialFd, LE_TTY_SPEED_9600);
  le_tty_SetCanonical(serialFd);
  int n = configureGpio(serialFd, buffer, res);
  // seek to the right location in res
  configureUart(serialFd, buffer, res + n);
  le_tty_Close(serialFd);
}
