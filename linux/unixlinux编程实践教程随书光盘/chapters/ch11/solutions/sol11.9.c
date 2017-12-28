/** sol11.9.c
 ** ------------------------------------------------------------
	A version of timeserv.c that prints the IP number and
	hostname of the caller is
	sol11.9.c.
	This program shows how to use inet_ntoa()
	and gethostbyaddr().

 ** ------------------------------------------------------------
 **
 **
 *  A version of timeserv.c that prints the IP number and hostname
 *  of the caller.  Shows how to use inet_ntoa() for IP string
 *  and gethostbyaddr() to get the name.
 *
 * timeserv.c - a socket-based time of day server

 *	build: cc sol11.9.c -o sol11.9
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <netdb.h>
#include  <time.h>
#include  <strings.h>
#include  <arpa/inet.h>

#define   PORTNUM  13000   /* our time service phone number */
#define   HOSTLEN  256
#define   oops(msg)      { perror(msg) ; exit(1) ; }

void caller_id(struct in_addr);

int main(int ac, char *av[])
{
	struct  sockaddr_in   saddr;   /* build our address here */
	struct	hostent		*hp;   /* this is part of our    */
	char	hostname[HOSTLEN];     /* address 	         */
	int	sock_id,sock_fd;       /* line id, file desc     */
	FILE	*sock_fp;              /* use socket as stream   */
	char    *ctime();              /* convert secs to string */
	time_t  thetime;               /* the time we report     */
	struct  sockaddr_in clnt_addr; /* who is calling us      */
	int     sin_len;;              /* and the length         */

      /*
       * Step 1: ask kernel for a socket
       */

	sock_id = socket( PF_INET, SOCK_STREAM, 0 );    /* get a socket */
	if ( sock_id == -1 ) 
		oops( "socket" );

      /*
       * Step 2: bind address to socket.  Address is host,port
       */

	bzero( (void *)&saddr, sizeof(saddr) ); /* clear out struct     */

	gethostname( hostname, HOSTLEN );       /* where am I ?         */
	hp = gethostbyname( hostname );         /* get info about host  */
	                                        /* fill in host part    */
	bcopy( (void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(PORTNUM);        /* fill in socket port  */
	saddr.sin_family = AF_INET ;            /* fill in addr family  */

	if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 )
	       oops( "bind" );

      /*
       * Step 3: allow incoming calls with Qsize=1 on socket
       */

	if ( listen(sock_id, 1) != 0 ) 
		oops( "listen" );

      /*
       * main loop: accept(), write(), close()
       */

	while ( 1 )
	{
		/*
		 * wait for a call, 
		 * provide space to receive caller's address
		 */
	       sin_len = sizeof(struct sockaddr);
	       sock_fd = accept(sock_id, 
			       	(struct sockaddr *)&clnt_addr, &sin_len);

	       if ( sock_fd == -1 )	       /* check for errors     */
		       oops( "accept" );

	       caller_id(clnt_addr.sin_addr);  /* print caller address */

	       sock_fp = fdopen(sock_fd,"w");  /* we'll write to the   */
	       if ( sock_fp == NULL )          /* socket as a stream   */
		       oops( "fdopen" );       /* unless we can't      */

	       thetime = time(NULL);           /* get time             */
					       /* and convert to strng */
	       fprintf( sock_fp, "The time here is .." );
	       fprintf( sock_fp, "%s", ctime(&thetime) ); 
	       fclose( sock_fp );              /* release connection   */
	}
	return 0;
}

/*
 * prints out the IP number and hostname associated with the addr 
 * passed
 */
void caller_id(struct in_addr addr)
{
	char		*ip_number ;
	struct hostent	*hostinfo;

	/* inet_ntoa converts an address to #.#.#.# format */
	ip_number = inet_ntoa(addr);
	printf("Call from: %s", ip_number);

	/* Get info about the host using its Internet address */
	hostinfo = gethostbyaddr((char *)&addr,sizeof(addr),AF_INET);
	if ( hostinfo != NULL)
		printf(" (%s) ", hostinfo->h_name);
	putchar('\n');
}
