#include <levikno/levikno.h>
#include <cstdio>

// INFO: this program demonstrates the logging components of the library


int main(int argc, char** argv)
{
    // create context
    LvnContext* ctx;
    lvn::createContext(&ctx);


    // [Creating loggers]
    // a logger controls how log messages should be logged
    // a sink is required first before a logger can be created in order for it to know where to send log messages
    // here we just use levikno's internally provided print output function
    LvnSink sink{};
    sink.logFunc = lvn::logOutputMessage;

    // logger create info struct
    LvnLoggerCreateInfo loggerCreateInfo{};
    loggerCreateInfo.loggerName = "myLogger";     // name of the logger
    loggerCreateInfo.format = "%n: %v%$";         // log pattern, the logger stores the log pattern which tells the logger how log messages should be formatted and displayed
    loggerCreateInfo.level = Lvn_LogLevel_None;   // the log level tells the logger which log levels to display and log levels to omit
    loggerCreateInfo.pSinks = &sink;              // add the sinks as a pointer array
    loggerCreateInfo.sinkCount = 1;               // include the sink count, the number of sinks in the pSinks array

    // create the logger
    LvnLogger* logger;
    lvn::createLogger(ctx, &logger, &loggerCreateInfo);

    // we can now use our own logger
    lvn::logMessageInfo(logger, "log message from our own logger");

    // note that the outputted message from our logger does not include certain information like the time or date
    // this is because they are not included in our log pattern

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
    lvn::loggerSetPatternFormat(logger, "[%T] [%l]: %v%$");

    lvn::logMessageTrace(logger, "log message with our new log pattern");

    // ANSI color codes can also be added in our pattern giving specific colors based on the log level of the message
    // - ANSI color codes need to have a starting and ending range using '%#' and '%^'
    lvn::loggerSetPatternFormat(logger, "[%T] [%#%l%^]: %v%$");

    lvn::logMessageWarn(logger, "log message with color");


    // we can also add our own custom log patters as well, make sure that your log pattern does not have the same symbol as one of the default log patterns
    // - this log pattern used the symbol '>' which is tied to a function that returns a string '>>>'
    //   this means when ever the log pattern '%>' is used, it will be replaced with '>>>' in our log message
    // - the parameter 'func' takes in a function with LvnLogPattern* as a parameter and a return type of std::string,
    //   here we use a lambda function for convenience
    LvnLogPattern logPattern{};
    logPattern.symbol = '>';
    logPattern.func = [](LvnLogMessage* msg) -> LvnString { return ">>>"; };

    // add a log pattern to the context, all loggers created from this context will check for this pattern
    lvn::logAddPatterns(ctx, &logPattern, 1);

    // now we can use our new log pattern
    lvn::loggerSetPatternFormat(logger, "[%T] [%#%l%^] %> %v%$");

    lvn::logMessageDebug(logger, "log message with our own custom log pattern");

    printf("\n");


    // [Changing log level]
    // - log messages are seperated into different levels based on their purposes (eg. trace, info, warn, error)
    // - the log levels go from lowest to highest: trace (1) -> debug (2) -> info (3) -> warn (4) -> error(5) -> fatal (6)
    // - you can set a minimum log level for each logger which will omit any log messages below a certain log level

    // here we set the minimum log level to error
    lvn::loggerSetLevel(logger, Lvn_LogLevel_Error);

    // log messages below the error level will not be displayed
    lvn::logMessageTrace(logger, "this trace message is displayed");
    lvn::logMessageInfo(logger, "this info message is displayed");
    lvn::logMessageWarn(logger, "this warn message is displayed");

    // log messages at or above the error level will be displayed
    lvn::logMessageError(logger, "this error message is displayed");
    lvn::logMessageFatal(logger, "this fatal message is displayed");

    // set the log level to none to allow all levels
    lvn::loggerSetLevel(logger, Lvn_LogLevel_None);

    printf("\n");


    // [Log data formatting]
    // - logging functions can take in extra parameters to display information to output
    // - this works similar to printf statments

    lvn::logMessageWarn(logger, "time left: %d s", 12);
    lvn::logMessageInfo(logger, "name: %s, age: %d, height: %.2f m", "Alex", 19, 1.8f);

    int code = 4;
    lvn::logMessageError(logger, "failed to do thing, error code: %d", code);


    printf("\n");

    // [Core Logger]
    // Levikno has its own internal core logger used to log core events such as object creation and error messages

    // the core logger can be retrieved like this:
    LvnLogger* coreLogger = lvn::logGetCoreLogger(ctx);

    lvn::logMessageInfo(coreLogger, "log message using the core logger");


    printf("\n");

    // [Enable/Disable Logging]

    // if you dont want the library to log core messages you can disable core logging for each context
    lvn::logEnableCoreLogging(ctx, false);

    // this core log would print
    lvn::logMessageDebug(coreLogger, "this core log wont print");

    // enable core logging again
    lvn::logEnableCoreLogging(ctx, true);
    lvn::logMessageDebug(coreLogger, "this core log will print");

    // you can also tell the library to not log messages at all if you need
    // this will prevent logging from all loggers created within the context
    lvn::logEnable(ctx, false);

    lvn::logMessageInfo(logger, "our logger wont print");
    lvn::logMessageInfo(coreLogger, "the core logger wont print");

    lvn::logEnable(ctx, true);
    lvn::logMessageInfo(logger, "our logger can print");


    // remember to destroy the logger like any other object
    lvn::destroyLogger(logger);

    // destroy context
    lvn::destroyContext(ctx);

    return 0;
}
