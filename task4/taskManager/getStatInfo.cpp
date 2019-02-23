#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	cout << "name=" << argv[2] << endl;
	cout << "pid=" << argv[1] << endl;	
	cout << "ppid=" << argv[4] << endl;
	cout << "priority=" << argv[18] << endl;
	cout << "rss=" << argv[24] << endl;
	return 0;
}
