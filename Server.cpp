#include "Class.cpp"

int main() {
	Server app;
	int socket = app.startServer("8000");

	int client = app.acceptConnection(socket);
	app.connection(client);
	while(true) {
		while(client > 0) {
			client = app.acceptConnection(socket);
			app.connection(client);
			client = 0;
		}
	}
	return 0;
}