# ECE 6122 Lab 5 UDP socket communication and SFML Graphics Implemetation
## Simulate the movement of a robot on a 2D screen using SFML for graphics and UDP sockets for communication. The robot’s location will be updated by sending direction data over a UDP socket from a client to a server. The server will render the updated direction for the robot on the SFML window.

### Nov 12th, 2024
* Finished the basic frameworks for UDPServer and UDPClient, including UDP communications with given port, update the robot's position and direction when receiving packets from client.
* Here are some additional requirements need to finish:
    * __use an image for the robot and rotating the image as the robot's direction changes__
    * __server application ends when the winodw is closed and informs the client that the server is about to close__
* By the way, this Lab may only ask to update the robot's position when a new command packet received from client. In the future, one improvement is that make the move and change direction separately. The robot will continue moving and change its direction when a command received from client. To do that, there are serveral ways can implement here. But so far, I can come up with two main methods.
    1. use ***non-block socket*** to receive message and only updates the direction when receive a command. This will be relatively easy as it is done in single thread.
    2. use ***OpenMP or threads*** to implement parallelism. This may be more challenging as it needs some atomic or mutex function to avoid data race or memory competition. 
