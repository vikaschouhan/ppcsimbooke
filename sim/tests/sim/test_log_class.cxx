// This is a modified third party test for a third party logging framework.
// It appeared with an article titled "Logging in C++" by Petru Marginean in 2007 on www.drdobbs.com
// This test was there with the other headers.

#include "log.hpp"

int main(int argc, char* argv[])
{
    LOG_TO_FILE("test_log_class.log");    
    try
    {
	    const int count = 3;
        LOG("DEBUG") << "A loop with " << count << " iterations" << std::endl;
        for (int i = 0; i != count; ++i)
        {
	        LOG("DEBUG1") << "the counter i = " << i << std::endl;
        }
        return 0;
    }
    catch(const std::exception& e)
    {
        LOG("ERROR") << e.what() << std::endl;
    }
    return 0;
}
