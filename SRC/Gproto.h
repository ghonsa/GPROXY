/*
	GProto - gregs protocol 
	
	  Command packets (4 byte command,4byte lenght of buffer, buffer)
	  
		commmands:


  PROT - protocol
		len = 4 
		buffer ="GREG" 
		from client
		
  VERS - version 
		len = 4 
		buffer ="0001" 
		both
		
  SECS - seed security 
		len = 4 
		buffer = dword index into security seed buffer 
		from server

  SIGN - sign on packet 
		len = strlen of buffer 
		buffer = md5 encrypted with selected seed string
		from client

  CONN - connected
		len = strlen of welcome message
		buffer = sign on message.....
		from server

  MESG - text message 
		len = strlen of msg
		buffer = "[TO]brodcast name [/TO] [FROM]sender name[/FROM][MSG, size= (len of msg)] ......[/MSG]
		both









 */