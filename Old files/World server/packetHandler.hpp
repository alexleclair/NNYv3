ByteArray buffer;
while (true) {
    //===== Here the famous packet parser!
    //+++ so all the code following must be put in the section that receive/process incoming packets +++
    //+++ this is usually placed whiting a while(true) loop with a socketRead blocking just before +++

    //here we have read all bytes from the socket and put it into 'input'
    ByteArray input;
    td.socket >> input;
    //'buffer' is a special ByteArray class to read/write a byte array.
    //This object is unique to all threads and is never destroyed.
    //(well its actually destroyed when the socket disconnect but yeah)
    buffer.append(input); //so we append the new input to the end of the buffer

    //!!!!!!!!!!!!!!!!!!
    if (!td.socket.isDataReceived()) break; //if more than 0 bytes has been read
    //!!!!!!!!!!!!!!!!!!

    //a while() here:
    //yeah sometimes there's many capsules in a packet so we have to
    //do this loop more than once, but otherwise (and usually) it will simply break().
    while (true) {
    //----------------------------------
        //Set the buffer seek to the begening so we can perform our checks without errors.
        //By errors I mean this is making sure we arnt lost somewhere else in the buffer
        //like... you guessed it... doing this loop twice.
        //That could totally result in a massive crash if we dont reset the seek pointer.
        buffer.setSeek(0); //Here, problem solved, whats next :D

        //Now the seek is placed to the begining, so we are supposed to
        //read a "~" (0x7E) byte firstly.
        if (buffer.read<BYTE>() == 0x7E) { //[byte] Packet begining signature
            DWORD length = buffer.read<DWORD>(); //we will need this later on so we know when we are done accumulating data

            //Is the length too big? We never know, some hackers could send a fake
            //packet to force the server to put 1Go of trash in memory.
            //Lets fix a limit of 1MB, i doubt we will somewhen have a packet that long
            //but whatever. 1MB is way enough.
            if (length > 1048576) { //1024 Bytes * 1024 KiloBytes = (1 MB) 1048576 Bytes
                packetToSend.add<CMD>(PCKT_X_HACKING_ATTEMPT);
                td.socket << packetToSend;
                td.socket.socket_close();
                std::cerr << "[packetHandler] @ERROR: Length refused!" << std::endl;
                break;
            }

            //Okay now the interesting part
            //is the [buffer data] (full size - packet header)
            //greater OR equal the [capsule data] length we have been told earlier?
            if (buffer.size()-5 >= length) {
                //Awesome we have enough bytes now :D
                //Lets continue the "so waited" code part
                //Copy the 5+length first bytes from the buffer to 'capsule'.
                ByteArray capsule;
                std::string substring = buffer.getRaw().substr(5, length);
                capsule.append(substring); //position to 5 and read length bytes
                //-----------------------------------------------
                //PROCESS THE CAPSULE (switches and stuff)
                #include "capsuleHandler.hpp"
                //-----------------------------------------------
                //Delete the 5+length first bytes from the buffer
                buffer.erase(0, 5+length); //position to 0 and delete 5+length character
                //We already processed it, we no need anymore. Anyway keeping it
                //would totally screw up the code because the Seek is always set
                //to the begening remember? So trust me, delete it >:D

                //Oh well, whats next, the buffer might have move capsules in it
                //(except the one we just processed and deleted)
                //so, easy solution, lets do the loop one more time :P
                //But before looping, lets check if I was right
                if (buffer.size() == 0) {
                    //We have read all the capsules in this packet
                    //Eheh, oh well we dont need to loop :)
                    break; //break the processing loop to wait for another packet from the socket
                } else {
                    //Eheh, i was right, there is really is another capsule in this packet :D
                }
            } else {
                //The buffer doesnt have enough bytes yet,
                //we still need to append new inputs to the buffer :(
                //So lets break for now to read another packet from the socket
                //and... well... continue accumulating
                //until we have enough bytes to process that damn packet :D
                break; //break the processing loop to wait for another packet from the socket
            }

        } else {
            //At this point, i hope we arent expecting a new capsule
            //because if its the case, that capsule doesnt starts with 0x7E.
            //Something is definitivelly going wrong.
            std::cerr << "[packetHandler] @ERROR: Packets are corrupted!" << std::endl;
            std::cerr << "[packetHandler] @ERROR: There is another capsule in this packet, not starting with 0x7E!" << std::endl;
            td.socket.socket_close();
            break;
        }
    }
}
