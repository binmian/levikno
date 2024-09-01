#include <cstdio>
#include <levikno/levikno.h>

// INFO: this program demonstrates the logging components of the library


int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true; // enable logging before initializing context
	lvnCreateInfo.logging.disableCoreLogging = true;

	lvn::createContext(&lvnCreateInfo);

	LVN_INFO("logging example, more info in source code\n");

	// [Logging functions & macros]
	// log messages are seperates through log levels

	lvn::logMessageTrace(lvn::logGetClientLogger(), "this is a trace log message");
	lvn::logMessageDebug(lvn::logGetClientLogger(), "this is a debug log message");
	lvn::logMessageInfo(lvn::logGetClientLogger(), "this is a info log message");
	lvn::logMessageWarn(lvn::logGetClientLogger(), "this is a warn log message");
	lvn::logMessageError(lvn::logGetClientLogger(), "this is a error log message");
	lvn::logMessageFatal(lvn::logGetClientLogger(), "this is a fatal log message");

	printf("\n");

	// log messages can also be called through macros which has the same definition of the log calls above
	LVN_TRACE("trace message");
	LVN_DEBUG("debug message");
	LVN_INFO("info message");
	LVN_WARN("warn message");
	LVN_ERROR("error message");
	LVN_FATAL("fatal message");

	printf("\n");


	// [Enabling and disabling logging]
	// logging can be enabled and disabled after the context has been initialized:

	lvn::logEnable(false);
	LVN_TRACE("this message will not be displayed");

	lvn::logEnable(true);
	LVN_TRACE("this message will be displayed");

	printf("\n");


	// [Core and Client logger]
	// in the library, there are two loggers that are created by default, the core logger and client logger
	// - the core logger is mainly used within the library and displays library events and errors such as object creation and loading
	// - The client logger is mainly used for the application (although it does not have to be used)
	// - you can disable the core logger if you don't want the library to output text
	// - you can still make calls to the core logger in the application:

	// this function enables or diables output from the core logger
	lvn::logEnableCoreLogging(true);

	lvn::logMessageTrace(lvn::logGetCoreLogger(), "this message is from the core logger");

	LVN_CORE_TRACE("this message is also from the core logger");

	lvn::logEnableCoreLogging(false);

	printf("\n");


	// [Creating loggers]
	// a logger object controls how log messages should be displayed,
	// if the core and client logger is not enough, new logget objects can be created:

	// logger create info struct
	LvnLoggerCreateInfo loggerCreateInfo{};
	loggerCreateInfo.loggerName = "myLogger";        // name of the logger
	loggerCreateInfo.logPatternFormat = "%n: %v%$";  // log pattern, the logger stores the log pattern which tells the logger how log messages should be formatted and displayed
	loggerCreateInfo.logLevel = Lvn_LogLevel_None;   // the log level tells the logger which log levels to display and log levels to omit

	// create the logger
	LvnLogger* logger;
	lvn::createLogger(&logger, &loggerCreateInfo);

	// we can now use our own logger
	lvn::logMessageInfo(logger, "log message from our own logger");

	// note that the outputted message from our logger does not include certain information like the time or date
	// this is because they are not added in our log pattern

	printf("\n");


	// [Log patterns]
	// - log patterns are used to format the output of our message
	// - levikno also has in built log patterns for certain information such as the date and time
	// - each log pattern starts with a '%' followed by a character
	//
	// list of default log pattens:
	//  - '$' = end of log message; new line "\n"
	//  - 'n' = the name of the logger
	//  - 'l' = log level of message (eg. "info", "error")
	//  - '#' = start color range based on log level (ANSI code color)
	//  - '^' = end of color range (ANSI code reset)
	//  - 'v' = the actual message to display
	//  - '%' = display percent sign '%'; log pattern would be "%%"
	//  - 'T' = get the time in 24 hour HH:MM:SS format
	//  - 't' = get the time in 12 hour HH:MM:SS format
	//  - 'Y' = get the year in 4 digits (eg. 2025)
	//  - 'y' = get the year in 2 digits (eg. 25)
	//  - 'm' = get the month number from (1-12)
	//  - 'B' = get the name of the month (eg. April)
	//  - 'b' = get the name of the month shortened (eg. Apr)
	//  - 'd' = get the day number in month from (1-31)
	//  - 'A' = get the name of day in week (eg. Monday)
	//  - 'a' = get the name of day in week shortened (eg. Mon)
	//  - 'H' = get hour of day (0-23)
	//  - 'h' = get hour of day (1-12)
	//  - 'M' = get minute (0-59)
	//  - 'S' = get second (0-59)
	//  - 'P' = get the time meridiem (AM/PM)
	//  - 'p' = get the time meridiem in lower (am/pm)
	//
	// ex: a log pattern of: "[%T] [%l]: %v%$"
	//     could output: "[14:25:11] [info]: some log message\n"


	// we can change the log pattern after a logger has been created
	lvn::logSetPatternFormat(logger, "[%T] [%l]: %v%$");

	lvn::logMessageTrace(logger, "log message with our new log pattern");

	// we can also add ANSI color codes within our pattern which will have specific colors based on the log level of the message
	// - ANSI color codes need to have a starting range and ending range using '%#' and '%^'
	lvn::logSetPatternFormat(logger, "[%T] [%#%l%^]: %v%$");

	lvn::logMessageWarn(logger, "log message with color");


	// we can also add our own custom log patters as well, make sure that your log pattern does not have the same symbol as one of the default log patterns
	// - this log pattern used the symbol '>' which is tied to a function that returns a string '>>>'
	//   this means when ever the log pattern '%>' is used, it will be replaced with '>>>' in our log message
	// - the parameter 'func' takes in a function with LvnLogPattern* as a parameter and a return type of std::string,
	//   here we use a lambda function for convenience
	LvnLogPattern logPattern{};
	logPattern.symbol = '>';
	logPattern.func = [](LvnLogMessage* msg) -> std::string { return ">>>"; };

	// log pattern will be added to the library
	lvn::logAddPatterns(&logPattern, 1);

	// now we can use our new log pattern
	lvn::logSetPatternFormat(logger, "[%T] [%#%l%^] %> %v%$");

	lvn::logMessageDebug(logger, "log message with our own custom log pattern");

	printf("\n");


	// [Changing log level]
	// - log messages are seperated into different levels based on their purposes (eg. trace, info, warn, error)
	// - the log levels go from lowest to highest: trace (1) -> debug (2) -> info (3) -> warn (4) -> error(5) -> fatal (6)
	// - you can set a minimum log level for each logger which will omit any log messages below a certain log level

	// here we set the log level to error
	lvn::logSetLevel(logger, Lvn_LogLevel_Error);

	// log messages below the error level will not print
	lvn::logMessageTrace(logger, "this trace message will not print");
	lvn::logMessageInfo(logger, "this info message will not print");
	lvn::logMessageWarn(logger, "this warn message will not print");

	// log messages at or above the error level will print
	lvn::logMessageError(logger, "this error message will print");
	lvn::logMessageFatal(logger, "this fatal message will print");

	// set the log level to none to allow all levels
	lvn::logSetLevel(logger, Lvn_LogLevel_None);

	printf("\n");


	// [Log data formatting]
	// - logging functions can take in extra parameters to display information to output
	// - this works similar to printf statments

	lvn::logMessageWarn(logger, "time left: %d s", 12);
	lvn::logMessageInfo(logger, "name: %s, age: %d, height: %.2f m", "Alex", 19, 1.8f);

	int code = 4;
	lvn::logMessageError(logger, "failed to do thing, error code: %d", code);


	// remember to destroy the logger like any other object
	lvn::destroyLogger(logger);

	// terminate context
	lvn::terminateContext();

	return 0;
}
