/* FIXME: From sys/sysvi386/sys */
#ifndef	_SYS_TERMIOS_H
# define _SYS_TERMIOS_H

# define _XCGETA (('x'<<8)|1)
# define _XCSETA (('x'<<8)|2)
# define _XCSETAW (('x'<<8)|3)
# define _XCSETAF (('x'<<8)|4)
# define _TCSBRK (('T'<<8)|5)
# define _TCFLSH (('T'<<8)|7)
# define _TCXONC (('T'<<8)|6)

# define TCOOFF	0
# define TCOON	1
# define TCIOFF	2
# define TCION	3

# define TCIFLUSH	0
# define TCOFLUSH	1
# define TCIOFLUSH	2

# define NCCS 13

# define TCSAFLUSH	_XCSETAF
# define TCSANOW	_XCSETA
# define TCSADRAIN	_XCSETAW
# define TCSADFLUSH	_XCSETAF

# define IGNBRK	000001
# define BRKINT	000002
# define IGNPAR	000004
#define	PARMRK		0x00000008	/* mark parity and framing errors */
# define INPCK	000020
# define ISTRIP	000040
# define INLCR	000100
# define IGNCR	000200
# define ICRNL	000400
# define IXON	002000
# define IXOFF	010000

# define OPOST	000001
# define OCRNL	000004
# define ONLCR	000010
# define ONOCR	000020
# define TAB3	014000

# define CLOCAL	004000
# define CREAD	000200
# define CSIZE	000060
# define CS5	0
# define CS6	020
# define CS7	040
# define CS8	060
# define CSTOPB	000100
# define HUPCL	002000
# define PARENB	000400
# define PAODD	001000

# define ECHO	0000010
# define ECHOE	0000020
# define ECHOK	0000040
# define ECHONL	0000100
# define ICANON	0000002
# define IEXTEN	0000400	/* anybody know *what* this does?! */
# define ISIG	0000001
# define NOFLSH	0000200
# define TOSTOP	0001000

# define VEOF	4	/* also VMIN -- thanks, AT&T */
# define VEOL	5	/* also VTIME -- thanks again */
# define VERASE	2
# define VINTR	0
# define VKILL	3
# define VMIN	4	/* also VEOF */
# define VQUIT	1
# define VSUSP	10
# define VTIME	5	/* also VEOL */
# define VSTART	11
# define VSTOP	12

# define B0	000000
# define B50	000001
# define B75	000002
# define B110	000003
# define B134	000004
# define B150	000005
# define B200	000006
# define B300	000007
# define B600	000010
# define B1200	000011
# define B1800	000012
# define B2400	000013
# define B4800	000014
# define B9600	000015
# define B19200	000016
# define B38400	000017

typedef unsigned char cc_t;
typedef unsigned short tcflag_t;
typedef char speed_t;


struct termios {
	tcflag_t	c_iflag;
	tcflag_t	c_oflag;
	tcflag_t	c_cflag;
	tcflag_t	c_lflag;
	char		c_line;
	cc_t		c_cc[NCCS];
	speed_t		c_ispeed;
	speed_t		c_ospeed;
        unsigned        raw;
};

# ifndef _NO_MACROS

#  define cfgetospeed(tp)	((tp)->c_ospeed)
#  define cfgetispeed(tp)	((tp)->c_ispeed)
#  define cfsetospeed(tp,s)	(((tp)->c_ospeed = (s)), 0)
#  define cfsetispeed(tp,s)	(((tp)->c_ispeed = (s)), 0)
#  define tcdrain(fd)		_ioctl (fd, _TCSBRK, 1)
# endif	/* _NO_MACROS */


/*
 * Control flags - hardware control of terminal
 */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define	CIGNORE		0x00000001	/* ignore control flags */
#endif
#define CSIZE		0x00000300	/* character size mask */
#define     CS5		    0x00000000	    /* 5 bits (pseudo) */
#define     CS6		    0x00000100	    /* 6 bits */
#define     CS7		    0x00000200	    /* 7 bits */
#define     CS8		    0x00000300	    /* 8 bits */
#define CSTOPB		0x00000400	/* send 2 stop bits */
#define CREAD		0x00000800	/* enable receiver */
#define PARENB		0x00001000	/* parity enable */
#define PARODD		0x00002000	/* odd parity, else even */
#define HUPCL		0x00004000	/* hang up on last close */
#define CLOCAL		0x00008000	/* ignore modem status lines */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define CCTS_OFLOW	0x00010000	/* CTS flow control of output */
#define CRTSCTS		(CCTS_OFLOW | CRTS_IFLOW)
#define CRTS_IFLOW	0x00020000	/* RTS flow control of input */
#define	CDTR_IFLOW	0x00040000	/* DTR flow control of input */
#define CDSR_OFLOW	0x00080000	/* DSR flow control of output */
#define	CCAR_OFLOW	0x00100000	/* DCD flow control of output */
#define	MDMBUF		0x00100000	/* old name for CCAR_OFLOW */
#endif

int tcgetattr(int fd,struct termios *termios_p);
int tcsetattr(int fd,int optional_actions,const struct termios *termios_p);
void cfmakeraw(struct termios *termios_p);
int tgetnum(char *id);
#endif	/* _SYS_TERMIOS_H */

