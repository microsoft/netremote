
#include <catch2/catch_session.hpp>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Init.h>
#include <plog/Severity.h>

int
main(int argc, char* argv[])
{
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};

    plog::init(plog::debug, &colorConsoleAppender);

    return Catch::Session().run(argc, argv);
}
