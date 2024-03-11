#include "assert.h"
#include "os.h"
#include "types.h"
#include "utils.h"

#define STATUS_BUFSZ 1024
#define STATUS_OUTPUT_BUFSZ 128
#define STATUS_PERIOD_SECOND 1
static char buf[STATUS_BUFSZ];
static char outbuf[STATUS_OUTPUT_BUFSZ];

int main(int argc, char *argv[]) {
  char *output;

loop:
  output = outbuf;
#define OUTPUT(msg, len)                                                       \
  {                                                                            \
    memcpy(output, msg, len);                                                  \
    output += len;                                                             \
  }
#define OUTPUT_STR(msg) OUTPUT(msg, sizeof(msg) - 1)

  /* loadavg */
  OUTPUT_STR("c: ");
  {
    i64 fd_loadavg = open("/proc/loadavg", O_RDONLY);
    if (fd_loadavg < 0) {
      return 1;
    }

    /* maxium bytes required for at least
     * number of digits(NR_CPUS) * 8 + 1
     * ex: NR_CPUS=32
     *    at max it will be 32.32 32.32 32.32 => 16 + 1 bytes
     *    at low it will be    0.01 0.01 0.01 => 13 + 1 bytes
     */
    read(fd_loadavg, buf, 32);

    char *p = buf + 14;
    while (*p != ' ') {
      p++;
    }
    u64 len = (u64)(p - buf);
    OUTPUT(buf, len);

    close(fd_loadavg);
  }

  /* cpu tempature */
  OUTPUT_STR(" ");
  {
    i64 fd_temp = open("/sys/class/hwmon/hwmon4/temp2_input", O_RDONLY);
    if (fd_temp < 0) {
      return 2;
    }

    /* values in C
     *   min 9000
     *   max 999000
     */
    read(fd_temp, buf, 8);

    char *p = buf;
    while (*p != '\n') {
      p++;
    }
    u64 len = (u64)((p - buf) - 3);
    OUTPUT(buf, len);

    close(fd_temp);
  }

  /* gpu */
  OUTPUT_STR(" g: ");
  {
    i64 fd_gpu_percent =
        open("/sys/class/drm/card0/device/gpu_busy_percent", O_RDONLY);
    if (fd_gpu_percent < 0) {
      return 3;
    }

    /* values in percent
     *   min 0
     *   max 100
     */
    read(fd_gpu_percent, buf, 3);

    char *p = buf;
    while (*p != '\n') {
      p++;
    }
    u64 len = (u64)(p - buf);
    OUTPUT(buf, len);

    close(fd_gpu_percent);
  }

  /* gpu tempature */
  OUTPUT_STR(" ");
  {
    i64 fd_temp = open("/sys/class/hwmon/hwmon0/temp2_input", O_RDONLY);
    if (fd_temp < 0) {
      return 4;
    }

    /* values in C
     *   min 9000
     *   max 999000
     */
    read(fd_temp, buf, 8);

    char *p = buf;
    while (*p != '\n') {
      p++;
    }
    u64 len = (u64)((p - buf) - 3);
    OUTPUT(buf, len);

    close(fd_temp);
  }

  /* memory */
  OUTPUT_STR(" m: ");
  {
    i64 fd_meminfo = open("/proc/meminfo", O_RDONLY);
    if (fd_meminfo < 0) {
      return 5;
    }

    u64 mem_total = 0;
    u64 mem_free = 0;
    u64 mem_buffers = 0;
    u64 mem_cached = 0;
    u64 mem_shared = 0;
    u64 mem_sreclaimable = 0;
    u64 mem_used;

    read(fd_meminfo, buf, 1024);

    /* units are kb */
    char *p = buf;
    p = strtou64(p, sizeof(buf) - (u64)(p - buf), &mem_total);
    p = strtou64(p, sizeof(buf) - (u64)(p - buf), &mem_free);
    p = strchr(p, 'u'); /* ignore available */
    p = strtou64(p, sizeof(buf) - (u64)(p - buf), &mem_buffers);
    p = strtou64(p, sizeof(buf) - (u64)(p - buf), &mem_cached);
    p = strstrn(p, sizeof(buf) - (u64)(p - buf), "Sh", 2); /* Shmem */
    p = strtou64(p, sizeof(buf) - (u64)(p - buf), &mem_shared);
    p = strstrn(p, sizeof(buf) - (u64)(p - buf), "SR", 2); /* SReclaimable */
    p = strtou64(p, sizeof(buf) - (u64)(p - buf), &mem_sreclaimable);

    /* calculation from htop
     * src: htop/linux/LinuxMachine.c
     * fnc: LinuxMachine_scanMemoryInfo()
     */
    mem_used = mem_total - (mem_free + mem_buffers + mem_cached + mem_shared +
                            mem_sreclaimable);

    u64 mem_used_gb = kbtogb(mem_used);
    u64 mem_used_mb = kbtomb(mem_used - gbtokb(mem_used_gb)) / 102;
    u64 mem_total_gb = kbtogb(mem_total);
    u64 mem_total_mb = kbtomb(mem_total - gbtokb(mem_total_gb)) / 102;

    struct string str;

#undef OUTPUT_U64
#define OUTPUT_U64(x)                                                          \
  {                                                                            \
    str = u64tostr(x, buf, sizeof(buf));                                       \
    OUTPUT(str.buf, str.len);                                                  \
  }

    OUTPUT_U64(mem_used_gb);
    OUTPUT_STR(".");
    OUTPUT_U64(mem_used_mb);
    OUTPUT_STR("/");
    OUTPUT_U64(mem_total_gb);
    OUTPUT_STR(".");
    OUTPUT_U64(mem_total_mb);

    close(fd_meminfo);
  }

  /* network */
  OUTPUT_STR(" n: ");
  {
    static u64 rx_bytes = 0;
    static u64 tx_bytes = 0;
    u64 rx_bytes_new = 0;
    u64 tx_bytes_new = 0;

    i64 fd_rx = open("/sys/class/net/wlan0/statistics/rx_bytes", O_RDONLY);
    if (fd_rx < 0) {
      return 5;
    }
    i64 fd_tx = open("/sys/class/net/wlan0/statistics/tx_bytes", O_RDONLY);
    if (fd_tx < 0) {
      return 5;
    }

    /* read current value */
    read(fd_rx, buf, 16);
    strtou64(buf, sizeof(buf), &rx_bytes_new);

    read(fd_tx, buf, 16);
    strtou64(buf, sizeof(buf), &tx_bytes_new);

    /* delta */
    rx_bytes = rx_bytes_new - rx_bytes;
    tx_bytes = tx_bytes_new - tx_bytes;

    if (rx_bytes == rx_bytes_new) {
      OUTPUT_STR("0.0/0.0");
      goto cleanup_network;
    }

    /* output */
    struct string str;

#undef OUTPUT_U64
#define OUTPUT_U64(x)                                                          \
  {                                                                            \
    str = u64tostr(x, buf, sizeof(buf));                                       \
    OUTPUT(str.buf, str.len);                                                  \
  }

    u64 rx_bytes_mb = btomb(rx_bytes);
    u64 rx_bytes_kb = btokb(rx_bytes - mbtob(rx_bytes_mb)) / 10;
    u64 tx_bytes_mb = btomb(tx_bytes);
    u64 tx_bytes_kb = btokb(tx_bytes - mbtob(tx_bytes_mb)) / 10;

    OUTPUT_U64(tx_bytes_mb);
    OUTPUT_STR(".");
    OUTPUT_U64(tx_bytes_kb);
    OUTPUT_STR("/");
    OUTPUT_U64(rx_bytes_mb);
    OUTPUT_STR(".");
    OUTPUT_U64(rx_bytes_kb);

  cleanup_network:
    /* cleanup */
    close(fd_rx);
    close(fd_tx);

    /* swap */
    rx_bytes = rx_bytes_new;
    tx_bytes = tx_bytes_new;
  }

#if 0

#if PIPEWIRE
  OUTPUT_STR(" v: ");
  {
    OUTPUT_STR("100%");
    OUTPUT_STR(" pipewire ---- ");
  }
#elif PULSEAUDIO
  OUTPUT_STR(" v: ");
  {
    OUTPUT_STR("100%");
    OUTPUT_STR(" pulseaudio ---- ");
  }
#elif ALSA
  OUTPUT_STR(" v: ");
  {
    OUTPUT_STR("100%");
    OUTPUT_STR(" alsa ---- ");
  }
#endif

#endif

  OUTPUT_STR("\n");
  write(SYSOUT, outbuf, (u64)(output - outbuf));

  /* sleep 1 seconds */
  nanosleep(
      &(struct timespec){
          .tv_sec = STATUS_PERIOD_SECOND,
          .tv_nsec = 0,
      },
      0);
  goto loop;
  return 0;
}
