#include <cstdio>
#include <levikno/levikno.h>

// INFO: this example shows how to save log messages to a file

int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true; // enable logging before initializing context
	lvnCreateInfo.logging.disableCoreLogging = true;

	lvn::createContext(&lvnCreateInfo);

	// NOTE: set the file config here to any logger to enable logging to file
	lvn::logSetFileConfig(lvn::logGetClientLogger(), true, "logToFileExample.txt", Lvn_FileMode_Write);

	LVN_INFO("logging to file example, more info in source code\n");

	lvn::logMessageTrace(lvn::logGetClientLogger(), "this is a trace log message");
	lvn::logMessageDebug(lvn::logGetClientLogger(), "this is a debug log message");
	lvn::logMessageInfo(lvn::logGetClientLogger(), "this is a info log message");
	lvn::logMessageWarn(lvn::logGetClientLogger(), "this is a warn log message");
	lvn::logMessageError(lvn::logGetClientLogger(), "this is a error log message");
	lvn::logMessageFatal(lvn::logGetClientLogger(), "this is a fatal log message");

	printf("\n"); // note that this print message will not be recorded to file

	LVN_TRACE("some trace messages...");
	LVN_DEBUG("a debug message here...");
	LVN_INFO("an info message here...");
	LVN_WARN("a warning message, code: %d", 12);
	LVN_ERROR("error message, code: %d", 2);
	LVN_FATAL("fatal message, string: %s", "oh no");

	printf("\n");

	// logging to file can be turned off again
	lvn::logSetFileConfig(lvn::logGetClientLogger(), false);

	LVN_INFO("these messages will not be logged into file");
	LVN_TRACE("trace messages again...");
	LVN_DEBUG("debug message again...");


	printf("\n");

	// logging to file from a custom logger
	LvnLoggerCreateInfo loggerCreateInfo{};
	loggerCreateInfo.loggerName = "customLogger";
	loggerCreateInfo.format = "[%T] [%#%l%^] %n: %v%$";
	loggerCreateInfo.level = Lvn_LogLevel_None;

	LvnLogger* logger;
	lvn::createLogger(&logger, &loggerCreateInfo);

	// note we are logging and appending to the same file
	// also note that if we dont specify an output file, the logger name will be used instead as the name of the file
	lvn::logSetFileConfig(logger, true, "logToFileExample.txt", Lvn_FileMode_Append);

	lvn::logMessageInfo(logger, "our custom logger output");
	lvn::logMessageWarn(logger, "more log outputs...");

	// terminate context
	lvn::terminateContext();

	return 0;
}
