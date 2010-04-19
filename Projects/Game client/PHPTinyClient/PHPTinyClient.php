<?php
/////////////////// CONFIGURATION ///////////////////
$REMOTE_IP = '127.0.0.1';
$REMOTE_PORT = 6131;
$PROTOCOL_HPP_FILE = 'protocol.hpp';
$REVISION = 0;
$SERVERID = 1;
$SYNC_KEY = 'SDFXqjs7nX'; 
$VERBOSE = False;
/////////////////////////////////////////////////////
echo 'PHPTinyClient v1.0.8'."\n";
include('ByteArray.php');
include('ProtocolLoader.php'); //Load protocol #defines

echo "\n";

$mainsock = socket_create(AF_INET, SOCK_STREAM, 0);
socket_set_option($mainsock, SOL_SOCKET,SO_REUSEADDR, 1);
//socket_bind($mainsock, '0', 5000) or die('Could not bind to address');
//socket_listen($mainsock);

$clients=Array();
$compteur=0;
echo 'Connecting...'."\n";

$socket=@socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!@socket_connect($socket, $REMOTE_IP, $REMOTE_PORT)) {
	echo '@ERROR: Unable to connect to '.$REMOTE_IP.' on port '.$REMOTE_PORT."\n";
	pause_exit();
}
$compteur++;
$nb=count($clients);
$clients[$nb]['SOCKET']=$socket;
$clients[$nb]['UID']=$compteur;
$clients[$nb]['BUFFER']=new ByteArray();

$BA = new ByteArray();
$BA->addCmd(PCKT_C_REVISION);
$BA->addDword($REVISION);
socket_write($socket, $BA->getPacket());
$BA->clear();

while(true){
$toread=Array();
array_push($toread,$mainsock);
for ($i=0;$i<count($clients);$i++){ // pour tous les clients
array_push($toread,$clients[$i]['SOCKET']);
}

socket_select($toread,$a=null,$a=null,$a=null);

if(in_array($mainsock, $toread)){// le mainsock est dans le tableau $toread.
// c'est notre mainsock donc un nouveau client
$sock=socket_accept($mainsock);
$compteur++;
$nb=count($clients);
$clients[$nb]['SOCKET']=$sock;
$clients[$nb]['UID']=$compteur;

}else{
// c'est un client qui dit quelque chose
for ($i=0;$i<count($clients);$i++){ // on cherche le client
if(in_array($clients[$i]['SOCKET'], $toread)) { // celui la est dans le tableau toread
$input = @socket_read($clients[$i]['SOCKET'], 1024);
if($input==null){
/// deconnection du client !
for ($j=0;$j<count($clients);$j++){ // on le cherche dans le tableau
if($clients[$j]==$clients[$i]){ // trouv�
echo '@ERROR: Connection closed by the remote host'."\n";
	if ($RECONNECT) {
		$clients[$j]['SOCKET']=@socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if (!@socket_connect($clients[$j]['SOCKET'], $RECONNECT_IP, $RECONNECT_PORT)) {
			echo '@ERROR: Unable to re-connect to '.$RECONNECT_IP.' on port '.$RECONNECT_PORT."\n";
			pause_exit();
		}
		echo '@INFO: Now connected to '.$RECONNECT_IP.' on port '.$RECONNECT_PORT."\n";
		$clients[$nb]['BUFFER']=new ByteArray();
	} else {
		array_splice($clients,$j,1); // on le retire du tableau
		$i--;
	}
}
}
}else{
$BUFFER = &$clients[$i]['BUFFER'];
//========================================================================
if ($VERBOSE) echo '[Recv '.$clients[$i]['UID'].'] : '.$input."\n";

//+++ so all the code following must be put in the section that receive +++
//+++ and process thoses incoming packets +++
//+++ this is usually placed whitting a while(true) loop with a socketRead blocking just before +++

//here we have read all bytes from the socket and put it into $input

//$BUFFER is a special ByteArray class to read/write a byte array.
//This object is unique to all threads and is never destroyed.
//(well its actually destroyed when the client disconnect but yeah)
$BUFFER->append($input); //so we append the new input to the end of the buffer

//a while() here:
//yeah sometimes there's many capsules in a packet so we have to
//do this loop more than once, but otherwise (and usually) it will simply break().
while (true) {
//----------------------------------
	//Set the buffer seek to the begening so we can perform our checks without errors.
	//By errors I mean this is making sure we arnt lost somewhere else in the buffer
	//like... you guessed it... doing this loop twice.
	//That could totally result in a massive crash if we dont reset the seek pointer.
	$BUFFER->setSeek(0); //Here, problem solved, whats next :D

	//Now the seek is placed to the begening, so we are supposed to
	//read a "~" (0x7E) byte firstly.
	if ($BUFFER->readByte() == 0x7E) { //[byte] Packet begining signature
		$length = $BUFFER->readDword(); //we will need this later on so we know when we are done accumulating data
		$nbCmds = $BUFFER->readByte(); //might be usefull for debugging even if its useless now
		if ($VERBOSE) echo 'Length: '.$length."\n".'NbCmds: '.$nbCmds."\n";
		//Is the length too big? We never know, some hackers could send a fake
		//packet to force the server to put 1Go of trash in memory.
		//Lets fix a limit of 1MB, i doubt we will somewhen have a packet that long
		//but whatever. 1MB is way enough.
		if ($length > 1048576) { //1024 Bytes * 1024 KiloBytes = (1 MB) 1048576 Bytes
			//Geez, that guy suck...
			//1 - Send PCKT_S_HACKING_ATTEMPT
			//2 - Disconnect the client by closing the socket
			//3 - Log / Save IP / Ban IP, whatever will do.
			//4 - and terminate thread.
			echo 'Error: LENGTH REFUSED!'."\n";
		}
		
		//Okay now the interesting part
		//is the [buffer data] (full size - packet header)
		//greater OR equal the [capsule data] length we have been told earlier?
		if ($BUFFER->size()-6 >= $length) {
			if ($VERBOSE) echo 'Enough bytes, processing...'."\n";
			//Awesome we have enough bytes now :D
			//Lets continue the "so waited" code part
			//Copy the 6+length first bytes from the buffer to $CAPSULE.
			$CAPSULE = new ByteArray();
			$CAPSULE->append(substr($BUFFER->getBuffer(), 6, $length)); //position to 6 and read length bytes
			//PROCESS THE CAPSULE (switch and stuff)
			//-----------------------------------------------
			if ($VERBOSE) echo 'Capsule: '.$CAPSULE->getBuffer()."\n";
			while (true) { //a loop to parse all CMDs in the capsule
				switch ($CAPSULE->readWord()) {
					case PCKT_R_SERVER_GONE:
						echo '[capsuleHandler] Server is gone, ahah!'."\n";
						break;
					case PCKT_R_WELCOME:
						echo '[capsuleHandler] Realm server welcome packet'."\n";
						break;
					case PCKT_W_WELCOME:
						echo '[capsuleHandler] World server welcome packet'."\n";
						break;
					case PCKT_R_CONNECT:
						$RECONNECT = true;
						$RECONNECT_IP = $CAPSULE->readString();
						$RECONNECT_PORT = $CAPSULE->readWord();
						echo '[capsuleHandler] Connect to server on '.$RECONNECT_IP.':'.$RECONNECT_PORT."\n";
						break;
					case PCKT_R_SYNC_KEY_ACK:
						if ($CAPSULE->readByte()) {
							echo '[capsuleHandler] Server synced!'."\n";
						} else {
							echo '[capsuleHandler] Server refused attempt!'."\n";
						}
						break;
					case PCKT_R_DOWNLOAD:
						$CAPSULE->readDword();
						$CAPSULE->readString();
						$CAPSULE->readString();
						echo '[capsuleHandler] File download requested but ignored'."\n";
						break;
					case PCKT_R_DOWNLOAD_EOF:
						echo '[capsuleHandler] List complete!'."\n";
						break;
					case PCKT_X_DEBUG:
						echo '[capsuleHandler] Debug message: '.$CAPSULE->readString()."\n";
						break;
					default:
						$CAPSULE->modSeek(-1);
						echo '[capsuleHandler] Unknow packet cmdID '.$CAPSULE->readWord().'! STOP!'."\n";
						$CAPSULE->setSeek($CAPSULE->size()); //GoTo EOF
						break;
				}
				if ($CAPSULE->eof()) break; //break the loop, no more CMDs
			}
			//-----------------------------------------------
			//Delete the 6+length first bytes from the buffer
			$BUFFER->delete(6+$length); //position to 6+length and read the rest
			//We already processed it, we no need anymore. Anyway keeping it
			//would totally screw up the code because the Seek is always set
			//to the begening remember? So trust me, delete it >:D
			
			//Oh well, whats next, the buffer might have move capsules in it
			//(except the one we just processed and deleted)
			//so, easy solution, lets do the loop one more time :P
			//But before looping, lets check if I was right
			if ($BUFFER->size() == 0) {
				//We have read all the capsules in this packet
				//Eheh, oh well we dont need to loop :)
				if ($VERBOSE) echo 'We have read all the capsules in this packet!'."\n";
				break; //break the processing loop to wait for another packet from the socket
			} else {
				//Eheh, i was right, there is really is another capsule in this packet :D
				if ($VERBOSE) echo 'There is another capsule in this packet :D'."\n";
			}
		} else {
			if ($VERBOSE) echo 'Still accumulating...'."\n";
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
		echo '@ERROR: PACKETS ARE CORRUPTED!'."\n";
		echo '@ERORR: There is another capsule in this packet, not starting with 0x7E!'."\n";
		pause_exit();
	}
//----------------------------------
}
//========================================================================
}
}
}
}
}

function pause_exit() {
while (true) { sleep(60); }
exit();
}
?>