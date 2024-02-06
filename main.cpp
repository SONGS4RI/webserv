#include "parse_config.hpp"
#include "utility.hpp"

using namespace std;

int	main(int ac, char **av) {
	if (ac != 2) {
		exitWithErrmsg(string("Input Error: ") + av[0] + " [configuration file]");
	}
	ParseConfig	conf(av[1]);
}
