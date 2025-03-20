#pragma once

#include <exception>
#include <future>
#include <memory>
#include <string>

//#include <ppltasks.h>

#include "RfAttenuatorExceptionImpl.hxx"
#include "RfAttenuatorProtocol.hxx"

/**
 * @brief Base class for all exceptions that can result from errors using a
 * transport.
 */
struct RfAttenuatorTransportException : public RfAttenuatorExceptionImpl
{
    RfAttenuatorTransportException() = default;
    RfAttenuatorTransportException(std::string what);
};

/**
 * @brief Attenuator transport.
 *
 * This is responsible for exchanging requests and their responses to/from the
 * attenuator. The input (request) and output (response) data types are variable
 * and should be specified by implementations. In most cases, it's expected that
 * the input and output data types are the same.
 *
 * @tparam MessageType The data type the transport accepts.
 */
template <typename MessageType>
struct RfAttenuatorTransport
{
    using MessageDataType = MessageType;

    // The base class must define the virutal destructor.
    // Or the child class destructor will not be called when delete the base class pointer.
    // https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors.
    // Because of this, the child class RfAttenuatorTransportStreamSocketWin32 resouse will not be released
    // when the unique_ptr<RfAttenuatorTransportStreamSocketWin32>/shared_ptr<RfAttenuatorTransportStreamSocketWin32>
    // is moved to unique_ptr<RfAttenuatorTransport>/shared_ptr<RfAttenuatorTransport>, then call unique_ptr<RfAttenuatorTransport>.reset().
    virtual ~RfAttenuatorTransport() = default;

    /**
     * @brief Send a synchronous request to the attenuator.
     *
     * @param request The request for the attenuator to execute.
     * @return MessageType The response from the attenuator.
     */
    virtual MessageType
    SendRequest(MessageType request) =0;

    // /**
    //  * @brief Send an asynchronous request to the attenuator.
    //  *
    //  * @param request The request for the attenuator to execute.
    //  * @return concurrency::task<MessageType> A task which returns the response
    //  * from the attenuator upon completion.
    //  */
    // virtual concurrency::task<MessageType>
    // SendRequestAsync(MessageType request)
    // {
    //     throw std::runtime_error("implementation for SendRequestAsync not implemented");
    // }

    /**
     * @brief Protocol message adapter.
     *
     * This is responsible for adapting transport-agnostic request
     * (IRfAttenuatorRequest) and response (IRfAttenuatorResponse) message
     * types. This allows differentiation of protocols over the transport.
     */
    struct ProtocolAdapter
    {
        /**
         * @brief Convert a transport-agnostic attenuator request
         * (IRfAttenuatorRequest) to a transport-specific (MessageType)
         * encoding.
         *
         * @param request Transport-agnostic request to adapt.
         * @return MessageType A transport-specific encoding of the specified
         * request.
         */
        virtual MessageType
        AdaptRequest(IRfAttenuatorRequest* /* request */)
        {
            throw std::runtime_error("protocol adapter interface function 'AdaptRequest' not implemented");
        }

        virtual ~ProtocolAdapter() = default;

        /**
         * @brief Convert a transport-specific (MessageType) response
         * encoding to a transport-agnostic attenuator response.
         *
         * @param request The request associated with the response.
         * @param response Transport-specific response to adapt.
         * @return std::unique_ptr<IRfAttenuatorResponse> A transport agnostic
         * attenuator response.
         */
        virtual std::unique_ptr<IRfAttenuatorResponse>
        AdaptResponse(const IRfAttenuatorRequest* /* request */, MessageType& /* response */)
        {
            throw std::runtime_error("protocol adapter interface function 'AdaptResponse' not implemented");
        }
    };
};
