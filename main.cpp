#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
//#include <conio.h>

using namespace std;

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

void writestr( tcp::socket *sock, const string &str )
{
    int l = str.length();
    system::error_code err;

    asio::write( *sock, asio::buffer( (void*) &l, 4 ), transfer_all(), err );
    asio::write( *sock, asio::buffer(str), transfer_all(), err );

    cout << "    >>> (" << l << "): " << str << "\n";
}

string readstr( tcp::socket *sock )
{
    int l = 0;
    system::error_code err;
    string res;

    asio::read( *sock, asio::mutable_buffer( (void*) &l, 4 ), transfer_all(), err );

    res.resize( l, ' ' );

    char buf[1000];

    asio::read( *sock, asio::mutable_buffer( buf, l ), transfer_all(), err );
    buf[l] = 0;

    cout << "    <<< (" << l << "): " << buf << "\n";

    return string( buf );
}

int main() {
    string name, cmd, s;
    int i, cnt;

    cout << "enter name: ";
    cin >> name;
    cout << "Connection. Press q to quit\n";

    tcp::endpoint port { ip::address::from_string( "127.0.0.1" ), 2000 };
    io_service io_serv;
    tcp::socket *sock = new tcp::socket( io_serv );

    sock->connect( port );

    writestr( sock, "helo" );
    cmd = readstr( sock );

    if (cmd != "ehlo")
    {
        cout << "Failed to connect\n";
        sock->close();
        return 1;
    }

    writestr( sock, name );

    while (true)
    {
        if (sock->available () > 0)
        {
            cmd = readstr( sock );

            if (cmd == "ping")
                writestr( sock, "pong" );
            if (cmd == "clientschange")
            {
                writestr( sock, "getclients" );
                s = readstr( sock );

                cnt = stoi( s );

                cout << "Clients changed\n";
                for( i = 0; i < cnt; i++ )
                {
                    s = readstr( sock );
                    cout << "  : " << s << "\n";
                }
            }
        }

        //Sleep(100);

        /*if (_kbhit())
        {
            char ch = _getch ();

            if (ch == 'q')
                break;
        }*/
    }

    writestr( sock, "buy" );
    s = readstr( sock );

    if (s == "buybuy")
        cout << "All closed fine\n";

    sock->close();

    return 0;
}
