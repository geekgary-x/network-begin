#include <gtest/gtest.h>
#include <LibName/Core/test.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
using namespace boost::asio::ip;
const int MAX_LENGTH = 1024;
class ServerTest : public ::testing::Test
{
protected:

    void SetUp() override
    { 
        future = boost::async(boost::launch::async, server);
    }

    void TearDown() override
    {
        // 停止服务端
        future.get();
    }

    static void server()
    {
        boost::asio::io_context io_context;
        tcp::socket             socket(io_context);
        tcp::acceptor           a(io_context, tcp::endpoint(tcp::v4(), 10086));
        a.accept(socket);
        try {
            for (;;) {
                char data[MAX_LENGTH];
                memset(data, '\0', MAX_LENGTH);
                boost::system::error_code error;
                size_t length = socket.read_some(boost::asio::buffer(data, MAX_LENGTH), error);
                if (error == boost::asio::error::eof) {
                    std::cout << "connection closed by peer" << std::endl;
                    break;
                }
                else if (error) {
                    throw boost::system::system_error(error);
                }
                std::cout << "receive from " << socket.remote_endpoint().address().to_string()
                     << std::endl;
                std::cout << "receive message is " << data << std::endl;
                // 回传信息值
                boost::asio::write(socket, boost::asio::buffer(data, length));
            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception in thread: " << e.what() << "\n" << std::endl;
        }
    }
    boost::unique_future<void> future;
};


TEST_F(ServerTest, request) {
    try {
        // 创建上下文服务
        boost::asio::io_context ioc;
        // 构造endpoint
        tcp::endpoint             remote_ep(address::from_string("127.0.0.1"), 10086);
        tcp::socket               sock(ioc);
        boost::system::error_code error = boost::asio::error::host_not_found;
        ;
        sock.connect(remote_ep, error);
        if (error) {
            std::cout << "connect failed, code is " << error.value() << " error msg is "
                      << error.message();
            return;
        }
        std::cout << "Enter message: ";
        char request[MAX_LENGTH];
        std::cin.getline(request, MAX_LENGTH);
        size_t request_length = strlen(request);
        boost::asio::write(sock, boost::asio::buffer(request, request_length));
        char   reply[MAX_LENGTH];
        size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply, request_length));
        std::cout << "Reply is: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}