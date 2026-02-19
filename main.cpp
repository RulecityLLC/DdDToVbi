#include <iostream>
#include "Worker.h"

using std::cout;
using std::endl;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cout << "Usage: " << argv[0] << " [path to DdD json file] [path to new .vbi file to create]" << endl;
		return 1;
	}

	Worker worker;

	cout << "Loading JSON..." << endl;
	string json = worker.loadJson(argv[1]);

	cout << "Parsing JSON (this can take a few minutes) " << endl;
	MpoJsonGenericSPtr root = worker.parseJson(json);

	cout << "Writing destination file" << endl;
	worker.writeVbiFile(root, argv[2]);

	return 0;
}
