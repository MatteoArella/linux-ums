#include "context.h"

int ums_context_create(struct ums_context *context)
{
	return 0;
}

int ums_context_init(struct ums_context *context)
{
	context->task = current;
	return 0;
}

int ums_context_destroy(struct ums_context *context)
{
	return 0;
}