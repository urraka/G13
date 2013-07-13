#include <g13/g13.h>
#include <sys/sys.h>

int main(int argc, char *argv[])
{
	sys::callback(sys::Initialize, g13::initialize);
	sys::callback(sys::Display, g13::display);
	sys::callback(sys::Terminate, g13::terminate);

	return sys::run(argc, argv);
}
