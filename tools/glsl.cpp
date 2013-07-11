#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
	cout << "#pragma once" << endl;
	cout << endl;

	cout << "namespace gfx {" << endl;
	cout << "namespace glsl {" << endl;
	cout << endl;

	for (int iFile = 1; iFile < argc; iFile++)
	{
		string filename = argv[iFile];
		ifstream file(filename.c_str());

		if (!file.is_open())
			continue;

		size_t pos = filename.rfind('/');

		if (pos != string::npos)
			filename = filename.substr(pos + 1);

		cout << "// " << string(77, '-') << endl;
		cout << "// " << filename << endl;
		cout << "// " << string(77, '-') << endl;
		cout << endl;

		pos = filename.find('.');

		if (pos != string::npos)
			filename[pos] = '_';

		cout << "const char *" << filename << " =" << endl;

		string line;

		while (getline(file, line))
		{
			cout << "\t\"";

			for (size_t i = 0; i < line.size(); i++)
			{
				char ch = line[i];

				switch (ch)
				{
					case '\t': cout << "\\t"; break;
					case '"':  cout << "\\\""; break;
					default:   cout << ch; break;
				}
			}

			cout << "\\n\"";

			if (file.peek() == EOF || file.eof())
				cout << ";";

			cout << endl;
		}

		cout << endl;
	}

	cout << "}} // gfx::glsl" << endl;

	return 0;
}
