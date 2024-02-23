
#include <memory>

#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliData.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteCliHandlerOperations.hxx>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Init.h>
#include <plog/Severity.h>

using Microsoft::Net::Remote::NetRemoteCli;
using Microsoft::Net::Remote::NetRemoteCliData;
using Microsoft::Net::Remote::NetRemoteCliHandler;
using Microsoft::Net::Remote::NetRemoteCliHandlerOperationsFactory;

int
main(int argc, char *argv[])
{
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    plog::init(plog::verbose, &colorConsoleAppender);

    auto cliData = std::make_shared<NetRemoteCliData>();
    auto cliHandler = std::make_shared<NetRemoteCliHandler>(std::make_unique<NetRemoteCliHandlerOperationsFactory>());
    auto cli{ NetRemoteCli::Create(cliData, cliHandler) };

    const int ret = cli->Parse(argc, argv);
    if (ret != 0) {
        return ret;
    }

    return 0;
}
