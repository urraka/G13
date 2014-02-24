#include <g13/vars.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static const char *var_name(const char *name)
{
	static char str[32];
	strcpy(str, "vars:");
	size_t i = strlen(str);
	str[i++] = tolower(*(name++));

	while (*name != 0)
	{
		const char ch = tolower(*name);

		if (ch != *name)
			str[i++] = '_';

		str[i++] = ch;
		++name;
	}

	assert(i < sizeof(str));
	str[i] = 0;

	return str;
}

#define G13_VARS_SOURCE
#include <g13/vars.h>
#undef G13_VARS_SOURCE

#define G13_VARS_LOAD
#include <g13/vars.h>
#undef G13_VARS_LOAD

#define G13_VARS_GET
#include <g13/vars.h>
#undef G13_VARS_GET

#define G13_VARS_SET
#include <g13/vars.h>
#undef G13_VARS_SET
