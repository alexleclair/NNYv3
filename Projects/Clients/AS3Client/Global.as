﻿package{	public class Global	{		//DEBUG		public static const  PCKT_X_DEBUG:uint = 0x0009 //PCKT decimal = 9		public static const  PCKT_X_HACKING_ATTEMPT:uint = 0x000A //PCKT decimal = 10				//REALM		public static const PCKT_R_WELCOME:uint = 0x0001 //PCKT decimal = 1		public static const PCKT_R_DOWNLOAD:uint = 0x0003 //PCKT decimal = 3		public static const PCKT_R_DOWNLOAD_EOF:uint = 0x0004 //PCKT decimal = 4		public static const PCKT_R_WORLD:uint = 0x0005 //PCKT decimal = 5		public static const PCKT_R_WORLD_EOF:uint = 0x0008 //PCKT decimal = 8		public static const PCKT_R_SYNC_KEY_ACK:uint = 0x0007 //PCKT decimal = 7		//WORLD		public static const PCKT_W_SYNC_KEY :uint = 0x0006 //PCKT decimal = 6		public static const PCKT_W_WELCOME :uint = 0x000B //PCKT decimal = 11		public static const PCKT_W_AUTH_ACK :uint = 0x000D //PCKT decimal = 13		public static const PCKT_W_CHARLIST_ADD :uint = 0x000F //PCKT decimal = 15		public static const PCKT_W_CHARLIST_EOF :uint = 0x0010 //PCKT decimal = 16		public static const PCKT_W_ENTER_WORLD_ACK :uint = 0x0013 //PCKT decimal = 19				//ACK		public static const ACK_FAILURE:uint = 0x00		public static const ACK_SUCCESS:uint = 0x01		public static const ACK_NOT_FOUND:uint = 0x02		public static const ACK_DOESNT_MATCH:uint = 0x03		public static const ACK_ALREADY:uint = 0x04		public static const ACK_REFUSED:uint = 0x05	}}