#pragma once

namespace hlp
{
	class ConfigFile
	{
	public:
		ConfigFile(const char *filename);
		~ConfigFile();

		const char *readString(const char *key, const char *def = 0);
		int readInt(const char *key, int def = 0);
		float readFloat(const char *key, float def = 0.0f);
		double readDouble(const char *key, double def = 0.0);
		bool readBoolean(const char *key, bool def = false);

		void setString(const char *key, const char *value);
		void setInt(const char *key, int value);

		void save(const char *filename);

	private:
		void *dict_;
	};
}
