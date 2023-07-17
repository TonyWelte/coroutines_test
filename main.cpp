#include <iomanip>
#include <iostream>

#include <asio.hpp>
#include <asio/awaitable.hpp>

using asio::ip::tcp;

// https://stackoverflow.com/a/29865/6771938
void hexdump(void *ptr, int buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;
    for (i = 0; i < buflen; i += 16)
    {
        printf("%06x: ", i);
        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                printf("%02x ", buf[i + j]);
            else
                printf("   ");
        printf(" ");
        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
        printf("\n");
    }
}

asio::awaitable<void> read_socket(tcp::socket &socket)
{
    char data[1024];
    for (;;)
    {
        std::size_t n = co_await socket.async_read_some(asio::buffer(data), asio::use_awaitable);

        std::cout << "Read " << n << " bytes\n";
        hexdump(data, n);

        if (n == 0)
        {
            co_return;
        }
    }
}

int main(int argc, char **argv)
{
    // Deal with arguments
    if (argc != 4)
    {
        std::cerr << "Usage: coroutines_test <host> <port> <mountpoint>" << std::endl;
        return 1;
    }

    std::string host = argv[1];
    std::string port = argv[2];
    std::string mountpoint = argv[3];

    // ASIO common variables
    asio::error_code error;

    // Resolve host
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(host, port, error);
    if (error)
    {
        std::cerr << error.message() << std::endl;
        return 1;
    }

    // Establish socket connection
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints, error);
    if (error)
    {
        std::cerr << error.message() << std::endl;
        return 1;
    }

    // Send GET request
    std::string request = "GET /" + mountpoint + " HTTP/1.0\r\nUser-Agent: NTRIP ntrip_client_coroutines\r\n\r\n";
    asio::write(socket, asio::buffer(request));

    co_spawn(io_context, read_socket(socket), asio::detached);

    io_context.run();
}
