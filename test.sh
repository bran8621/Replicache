#!/bin/bash

# Compile the server and client
gcc -o server server.c -lpthread
gcc -o client client.c
gcc -o server_test server_test.c
gcc -o client_test client_test.c

# Start the server test script in the background
./server_test &
SERVER_PID=$!

# Wait for a bit to ensure the server is up
sleep 2

# Run the client test script
./client_test

# Wait for the server to finish
wait $SERVER_PID

echo "Test completed."
