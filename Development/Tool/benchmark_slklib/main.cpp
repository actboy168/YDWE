#include <windows.h>
#include <iostream>
#include <base/path/self.h>
#include "timer.h"
#include <boost/timer/timer.hpp>

void time_it(std::function<void(void)> func)
{
	boost::timer::cpu_timer t;
	t.resume();
	func();
	t.stop();
	std::cout << t.format(6, 
		"    wall   %ws \n"
		"    user   %us \n"
		"    system %ss \n"
		"    total  %ts \n"
		"    CPU (%p%) \n"
		) << "\n\n";
}

void benchmark_1();
void benchmark_2();

int main()
{
	boost::filesystem::current_path(base::path::self().remove_filename());

	time_it(benchmark_1);
	time_it(benchmark_2);

	system("pause");
	return 0;
}
