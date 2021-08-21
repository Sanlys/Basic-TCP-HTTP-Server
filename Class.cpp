#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdio.h>

class Server {
public:
	int startServer(std::string port) {
		int PORT = std::stoi(port);

		int listening = socket(AF_INET, SOCK_STREAM, 0);
		
		if(listening == -1) {
			std::cerr << "Can't create socket\n";
			return -1;
		}

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(PORT);
		inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);


		if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
			std::cerr << "Can't bind to IP/port\n";
			return -2;
		}

		if (listen(listening, SOMAXCONN) == -1) {
			std::cerr << "Can't Listen\n";
			return -3;
		}

		return listening;
	}

	int acceptConnection(int listening) {
		sockaddr_in client;
		socklen_t clientSize = sizeof(client);
		char host[NI_MAXHOST];
		char svc[NI_MAXSERV];


		int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

		if (clientSocket == -1) {
			std::cerr << "Client couldn't connect\n";
		}

		//close listening socket
		close(listening);

		memset(host, 0, NI_MAXHOST);
		memset(svc, 0, NI_MAXSERV);

		int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

		if (result) {
			std::cout << host << " connected on " << svc << "\n";
		}
		else {
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on " << ntohs(client.sin_port) << "\n";
		}

		return clientSocket;
	}

	void connection(int clientSocket) {
		char buf[4096];
		while (true) {
			//Clear buffer
			memset(buf, 0, 4096);

			//Wait for message
			int bytesRecv = recv(clientSocket, buf, 4096, 0);
			if (bytesRecv == -1) {
				std::cerr << "Could not connect\n";
				break;
			}

			if (bytesRecv == 0) {
				std::cout << "The client disconnected\n";
				break;
			}

			//Display message
			std::cout << "Recieved: " << std::string(buf, 0, bytesRecv) << "\n";

			char res[4096] = "HTTP/1.1 200 ok\nContent-Type: text/html; charset=UTF-8\n\n<HTML><body><h1>test</h1></body></HTML>";
			send(clientSocket, res, sizeof(res) + 1, 0);
			std::cout << res << "\n";
			close(clientSocket);
		}
	}
};