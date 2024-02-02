
#include <catch2/catch_session.hpp>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

int main(int argc, char* argv[])
{
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};

    plog::init(plog::verbose, &colorConsoleAppender);

    return Catch::Session().run(argc, argv);
}
