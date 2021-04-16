#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace boost;

std::string sApiKey;
std::string sConfigFile;

int main(int argc, char *argv[])
{
    try
    {
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
        ("apikey,a", po::value<string>(&sApiKey), "show this stream")
        ("help,h", "show options")
        ;
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

        po::notify(vm);

        if (vm.count("apikey"))
        {
            cout << "apikey:" << sApiKey << endl;
        }
        if (vm.count("help"))
        {
            cerr << argv[0] << " - xtoy tool." << std::endl;
            cerr << desc << endl;
            return system::errc::success;
        }
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return system::errc::operation_not_permitted; // eq. 1 - General Catch
    }
    return system::errc::success; // eq. 0 - ok.
}
