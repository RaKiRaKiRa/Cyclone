/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-06-05 15:05
 * Last modified : 2019-06-05 15:05
 * Filename      : log_test.cc
 * Description   : 
 **********************************************************/
#include <string.h>
#include "../AsyncLogging.h"
#include "../Logging.h"

AsyncLogging *logptr = NULL;

void output(const char* logline, int len)
{
  logptr -> append(logline, len);
}

int main()
{
  LOG<<"Rua!!!!!!!!!!!";
/*
  LOG<<"Rua!!!!";
  LOG<<"Rua!!!!!!!!!!!!!!!!!!!!!";
  LOG<<"Rua!!!!!!!!!!!!!!!";
  LOG<<"Rua!";
  LOG<<"Rua!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
  LOG<<"Rua!!!!!!!";*/
  logptr = new AsyncLogging("asynclog", 50*1024*1000);
  Logger::setOutput(output);
  logptr -> start();


    for(int i=0;i<10;i++){
	LOG<<"Rua!!!!!!!";
        LOG_INFO<<"ASYNC LOG";
        struct timespec ts = { 0, 500*1000*1000 };
        nanosleep(&ts, NULL);
    }
  	LOG<<"Rua!!!!!!!";
LOG_WARN<<"Rua!!!!!!!";LOG_ERROR<<"Rua!!!!!!!";LOG_FATAL<<"Rua!!!!!!!";LOG_TRACE<<"Rua!!!!!!!";LOG_DEBUG<<"Rua!!!!!!!";LOG<<"Rua!!!!!!!";LOG<<"Rua!!!!!!!";

	struct timespec ts2 = { 10,0};
        nanosleep(&ts2, NULL);

}

