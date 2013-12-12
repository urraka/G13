#include "ConfigFile.h"
#include "ini/iniparser.h"

#include <assert.h>

namespace hlp {

ConfigFile::ConfigFile(const char *filename) : dict_(0)
{
	if (filename == 0 || (dict_ = iniparser_load(filename)) == 0)
		dict_ = dictionary_new(0);
}

ConfigFile::~ConfigFile()
{
	if (dict_ != 0)
		iniparser_freedict((dictionary*)dict_);
}

const char *ConfigFile::readString(const char *key, const char *def)
{
	return iniparser_getstring((dictionary*)dict_, key, def);
}

int ConfigFile::readInt(const char *key, int def)
{
	return iniparser_getint((dictionary*)dict_, key, def);
}

float ConfigFile::readFloat(const char *key, float def)
{
	return (float)iniparser_getdouble((dictionary*)dict_, key, def);
}

double ConfigFile::readDouble(const char *key, double def)
{
	return iniparser_getdouble((dictionary*)dict_, key, def);
}

bool ConfigFile::readBoolean(const char *key, bool def)
{
	return (bool)iniparser_getboolean((dictionary*)dict_, key, (int)def);
}

void ConfigFile::setString(const char *key, const char *value)
{
	const char *colon = strchr(key, ':');

	if (colon == 0)
		return;

	char section[33] = {0};

	assert(colon - key < (int)sizeof(section) - 1);

	strncpy(section, key, colon - key);

	if (!iniparser_find_entry((dictionary*)dict_, section))
		iniparser_set((dictionary*)dict_, section, NULL);

	iniparser_set((dictionary*)dict_, key, value);
}

void ConfigFile::save(const char *filename)
{
	FILE *file = fopen(filename, "w");

	if (file != 0)
	{
		iniparser_dump_ini((dictionary*)dict_, file);
		fclose(file);
	}
}

} // hlp
