#include "private.h"

#include <unistd.h>
#include <fcntl.h>

int UMS_FILENO = -1;

__attribute__((constructor))
static void ums_init(void)
{
	UMS_FILENO = open("/dev/" UMS_DEV_NAME, O_RDONLY);
}

__attribute__((destructor))
static void ums_exit(void)
{
	close(UMS_FILENO);
}
