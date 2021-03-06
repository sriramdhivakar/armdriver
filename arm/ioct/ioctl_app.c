#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio_commands.h"

int main(void)
{
	int fd,exp = 1,dir = 1,high = 1,low = 0;
	int gpio = 105,io;
	sleep(1);

	fd = open("/dev/ioctl",O_RDWR);
	if(fd < 0)
	{
		perror("open");
		_exit(1);
	}
	sleep(1);
	ioctl(fd,set_gpio_assign,&gpio);
	sleep(1);
	ioctl(fd,set_gpio_export,&exp);
	sleep(1);
	ioctl(fd,set_gpio_direction,&dir);
	sleep(1);
	while(1)
	{
		ioctl(fd,set_gpio_value,&high);
		sleep(1);
		ioctl(fd,set_gpio_value,&low);
		sleep(1);
	}

	close(fd);

	_exit(0);
}
