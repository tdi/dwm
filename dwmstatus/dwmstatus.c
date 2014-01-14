#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Xlib.h>

char *tzberlin = "Europe/Berlin";

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

void
settz(char *tzname)
{
	setenv("TZ", tzname, 1);
}


char ischarging() {
 FILE *fd;
 char *status;
 status = malloc(30);

 fd = fopen("/sys/class/power_supply/BAT0/status", "r");
 if (fd == NULL ) {
   fprintf(stderr, "Error opening status");
     return -1;
}

fscanf(fd, "%s", status);
printf("%s", status);
fclose(fd);

if (strcmp(status, "Charging") == 0) {
  return '+';
} else if (strcmp(status, "Unknown") == 0){
  return '?';
} else {
  return '-';
}

}

int getbattery() {
  FILE *fd;
  int energy_now, energy_full, voltage_now;

  fd = fopen("/sys/class/power_supply/BAT0/energy_now", "r");
  if(fd == NULL) {
    fd = fopen("/sys/class/power_supply/BAT0/charge_now", "r");
    if(fd == NULL) {
      fprintf(stderr, "Error opening energy_now.\n");
      return -1;


  }}
  fscanf(fd, "%d", &energy_now);
  fclose(fd);


  fd = fopen("/sys/class/power_supply/BAT0/energy_full", "r");
  if(fd == NULL) {
    fd = fopen("/sys/class/power_supply/BAT0/charge_full", "r");
    if(fd == NULL ) {
    fprintf(stderr, "Error opening energy_full.\n");
    return -1;

  }}
  fscanf(fd, "%d", &energy_full);
  fclose(fd);


  fd = fopen("/sys/class/power_supply/BAT0/voltage_now", "r");
  if(fd == NULL) {
    fprintf(stderr, "Error opening voltage_now.\n");
    return -1;
  }
  fscanf(fd, "%d", &voltage_now);
  fclose(fd);


  return ((float)energy_now * 1000 / (float)voltage_now) * 100 / ((float)energy_full * 1000 / (float)voltage_now);
}


void get_battery_bar(char ** retval) {
  char belka[10] = "          ";
  int bat = getbattery();
  int val = bat / 10;
  int rem = bat % 10;
  for (int i=0;i<val; i++) {
    belka[i] = '=';
  }
  if (rem > 5) {
    belka[val] = '-';
  }
  strncpy(retval[0], belka, 10);
  strncpy(retval[1], smprintf("%d", bat), sizeof(bat));
}


char *
mktimes(char *fmt, char *tzname)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	memset(buf, 0, sizeof(buf));
	settz(tzname);
	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL) {
		perror("localtime");
		exit(1);
	}

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		exit(1);
	}

	return smprintf("%s", buf);
}

void
setstatus(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char *
loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		perror("getloadavg");
		exit(1);
	}

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

int
main(void)
{
	char *status;
	char *avgs;
	char *tmbln;
  char ** retval; 

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	/* for (;;sleep(60)) { */
  retval = malloc(sizeof(char*) *2);
  retval[0] = malloc(sizeof(char)*10);
  retval[1] = malloc(sizeof(char)*2);
		avgs = loadavg();
		tmbln = mktimes("%W %a %d %b %H:%M %Z %Y", tzberlin);
    get_battery_bar(retval);
		status = smprintf("%c[%s] %s L:%s %s", ischarging(),
				retval[0], retval[1], avgs, tmbln);
		setstatus(status);
		free(avgs);
		free(tmbln);
		free(status);
    free(retval[0]);
    free(retval[1]);
    free(retval);
	/* } */

	XCloseDisplay(dpy);

	return 0;
}

