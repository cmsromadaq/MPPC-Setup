/*****************************************************************************/
/*                                                                           */
/*        --- CAEN Engineering Srl - Computing Systems Division ---          */
/*                                                                           */
/*   CMDWRAPP.C                                                              */
/*                                                                           */
/*   June      2000:  Rel. 1.0                                               */
/*   February  2001:  Rel. 1.1                                               */
/*   March     2002:  Rel. 2.0 - Linux - Deleted braces around strings       */
/*   September 2002:  Rel. 2.6                                               */
/*   November  2002:  Rel. 2.7                                               */
/*                                                                           */
/*****************************************************************************/
#include <signal.h>
#ifdef UNIX
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "MainWrapp.h"
#include "CAENHVWrapper.h"
#include "console.h"

#define   BS                 8
#define   LF                 10
#define   CR                 13
#define   MAXPASSWORDLENGTH  16
#define   MAX_FAN_IN         6

// If the macro below remains undefined, the programs implicitely logs to the HVPS as
// Administrator
#undef EXPLICIT_LOGIN


typedef struct ParPropTag
			{
				unsigned long	Type, Mode;
				float			MinVal, MaxVal;
				unsigned short	Unit;
				short			Exp;
				char			OnState[30], OffState[30];
			} ParProp;

static char *ParamTypeStr[] = {
				  "Numeric " ,
				  "Boolean " ,
				  "ChStatus" ,
				  "BdStatus" 
};

static char *ParamModeStr[] = {
				  "Read Only " ,
				  "Write Only" ,
				  "Read/Write" 
};

static char *ParamUnitStr[] = {
				  "None    " ,
				  "Ampere  " ,
				  "Volt    " ,
				  "Watt    " ,
				  "Celsius " ,
				  "Hertz   " ,
				  "Bar     " ,
				  "Volt/sec" ,
				  "sec     " ,
			 	  "rpm     "  ,     // Rel. 2.0. - Linux
			 	  "counts  "        // Rel. 2.6
};

static char *SysPropTypeStr[] = {
				  "String  " ,
				  "Real    " ,
				  "Unsign2B" ,
				  "Unsign4B" ,
				  "Int2B   " ,
				  "Int4B   " ,
				  "Boolean " 
};

static char *SysPropModeStr[] = {
				  "Read Only " ,
				  "Write Only" ,
				  "Read/Write" 
};

extern int loop;

/*****************************************************************************/
/*                                                                           */
/*  HVPS                                                                     */
/*  Internal function                                                        */
/*                                                                           */
/*****************************************************************************/
static int noHVPS(void)
{	
	int i = 0;

	while( System[i].ID == -1 && i != (MAX_HVPS - 1)) i++;
	return ( ( i == MAX_HVPS - 1 ) ? 1 : 0 );
}

/*****************************************************************************/
/*                                                                           */
/*  OneHVPS                                                                  */
/*  Internal function                                                        */
/*                                                                           */
/*****************************************************************************/
static int OneHVPS(void)
{	
	int i, j, k;

	for( i = 0, k = 0 ; i < (MAX_HVPS - 1) ; i++ )
		if( System[i].ID != -1 )
		{
			j = i;
			k++;
		}

	return ( ( k != 1 ) ? -1 : j );
}

/*****************************************************************************/
/*                                                                           */
/*  GETPASSWORD                                                              */
/*  Internal function                                                        */
/*                                                                           */
/*****************************************************************************/
static void GetPassword(char *PassWrd)
{
int c, i=0;

/* modificata cosi' perche' il linux prende CR come LF */
c = con_getch();
while( c != CR && c != LF)
  {
   if(c >= ' ')
      PassWrd[i++] = c;
   if(c == BS && i)
      i--;
   if(i >= MAXPASSWORDLENGTH)
      i--;
   c = con_getch();
  }
  
PassWrd[i] = '\0';
}

/*****************************************************************************/
/*                                                                           */
/*  READPARAMPROP                                                            */
/*  Internal function                                                        */
/*                                                                           */
/*****************************************************************************/
static CAENHVRESULT ReadParamProp
(char *name, unsigned short Slot, unsigned short Ch, char *ParName, ParProp *pp)
{
	CAENHVRESULT ret;

	if( Ch == 0xffff )	
	{
	 do{
/* First we read the "Type" and "Mode" properties, which are always present */
		ret = CAENHVGetBdParamProp(name, Slot, ParName, "Type", &(pp->Type));
		if( ret != CAENHV_OK )
		{
			con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
						CAENHVGetError(name), ret);
			con_getch();
			return ret;
		}

		ret = CAENHVGetBdParamProp(name, Slot, ParName, "Mode", &(pp->Mode));
		if( ret != CAENHV_OK )
		{
			con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
						CAENHVGetError(name), ret);
			con_getch();
			return ret;
		}

		if( pp->Type == PARAM_TYPE_NUMERIC )
		{
			ret = CAENHVGetBdParamProp(name, Slot, ParName, "Minval", &(pp->MinVal));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetBdParamProp(name, Slot, ParName, "Maxval", &(pp->MaxVal));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetBdParamProp(name, Slot, ParName, "Unit", &(pp->Unit));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetBdParamProp(name, Slot, ParName, "Exp", &(pp->Exp));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}
		}
		else if( pp->Type == PARAM_TYPE_ONOFF )
		{
			ret = CAENHVGetBdParamProp(name, Slot, ParName, "Onstate", pp->OnState);
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetBdParamProp(name, Slot, ParName, "Offstate", pp->OffState);
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}
		}
        
        if( con_kbhit() ) break;   
       }
     while(loop);
	}
	else
	{
	 do{
/* First we read the "Type" and "Mode" properties, which are always present */
		ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Type", &(pp->Type));
		if( ret != CAENHV_OK )
		{
			con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
						CAENHVGetError(name), ret);
			con_getch();
			return ret;
		}

		ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Mode", &(pp->Mode));
		if( ret != CAENHV_OK )
		{
			con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
						CAENHVGetError(name), ret);
			con_getch();
			return ret;
		}

		if( pp->Type == PARAM_TYPE_NUMERIC )
		{
			ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Minval", &(pp->MinVal));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Maxval", &(pp->MaxVal));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Unit", &(pp->Unit));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Exp", &(pp->Exp));
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}
		}
		else if( pp->Type == PARAM_TYPE_ONOFF )
		{
			ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Onstate", pp->OnState);
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}

			ret = CAENHVGetChParamProp(name, Slot, Ch, ParName, "Offstate", pp->OffState);
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", 
							CAENHVGetError(name), ret);
				con_getch();
				return ret;
			}
		}
        
        if( con_kbhit() ) break;   
       }
     while(loop);
	}

	return CAENHV_OK;
}

/*****************************************************************************/
/*                                                                           */
/*  DISPLAYPARAMPROP                                                         */
/*  Internal function                                                        */
/*                                                                           */
/*****************************************************************************/
static void DisplayParamProp
(unsigned short Slot, unsigned short Ch, char (*ParName)[MAX_PARAM_NAME], 
 int parNum, ParProp *pp)
{
	int p, 
		offset = parNum%6,
		page   = parNum/6 + ( offset ? 1 : 0 );

	for( p = 0 ; ; p = ( p + 1 ) % page )
	{
		int i;

		clrscr();
		if( Ch == 0xffff )
			con_printf("Board %2d", Slot);
		else
			con_printf("Board %2d,  Channel %2d", Slot, Ch);

		gotoxy(1, 24);
		con_printf(" Press 'Q' to exit, any other key to see all the Parameters ");

		for( i = 0 ; i < ( ( p < page-1 || !offset ) ? 6 : offset ) ; i++ )
		{
			int index = i + 6*p;
			int x     = 2+28*(i%3),
				y     = i < 3 ? 4 : 14;

			gotoxy(x, y);
			con_printf("   Param : %-s", ParName[index]);

			gotoxy(x, y+1);
			con_printf( "    Type : %-s", ParamTypeStr[pp[index].Type]);
			gotoxy(x, y+2);
			con_printf( "    Mode : %-s", ParamModeStr[pp[index].Mode]);
			if( pp[index].Type == PARAM_TYPE_NUMERIC )
			{
				gotoxy(x, y+3);
				con_printf( "  MinVal : %-10.2f", pp[index].MinVal);
				gotoxy(x, y+4);
				con_printf( "  MaxVal : %-10.2f", pp[index].MaxVal);
				gotoxy(x, y+5);
				con_printf( "    Unit : %-s", ParamUnitStr[pp[index].Unit]);
				gotoxy(x, y+6);
				con_printf( "     Exp : %-d", pp[index].Exp);
			}
			else 
			{
				gotoxy(x, y+3);
				con_printf( "  MinVal : ----------");
				gotoxy(x, y+4);
				con_printf( "  MaxVal : ----------");
				gotoxy(x, y+5);
				con_printf( "    Unit : ----------");
				gotoxy(x, y+6);
				con_printf( "     Exp : ----------");
			}
			if( pp[index].Type == PARAM_TYPE_ONOFF )
			{
				gotoxy(x, y+7);
				con_printf( " Onstate : %s", pp[index].OnState);
				gotoxy(x, y+8);
				con_printf( "Offstate : %s", pp[index].OffState);
			}
			else 
			{
				gotoxy(x, y+7);
				con_printf( " Onstate : ----------");
				gotoxy(x, y+8);
				con_printf( "Offstate : ----------");
			}
		}

		if( toupper(con_getch()) == 'Q' )
			break;    
	}
}

/*****************************************************************************/
/*                                                                           */
/*  HVSystemLOGIN                                                            */
/*                                                                           */
/*****************************************************************************/
void HVSystemLogin()
{
	char          arg[30], userName[20], passwd[30], name[64];
	int           i, c, link;
	CAENHVRESULT  ret;

	i = 0;
	while( System[i].ID != -1 && i != (MAX_HVPS - 1)) i++;
	if( i == MAX_HVPS - 1 )
	{
		con_printf("Too many connections");
		con_getch();
		return;
	}

	con_printf("Enter System Name: ");
	con_scanf("%s", name);

	con_printf("Comm. Layer: ""TCP/IP""-> 0, ""RS232""-> 1, ""CAENET""->2 ): ");
	c = con_getch();
	while( c != '0' && c != '1' && c != '2' )
		c = con_getch();

	switch( c )
	{
		case '0':
			con_printf("\nTCP/IP address (i.e. 192.9.200.1) : ");
			con_scanf("%s", arg);
			link = LINKTYPE_TCPIP;
			break;
		case '1':
			con_printf("\nCOM Port (i.e. COM1) : ");
			con_scanf("%s", arg);
			link = LINKTYPE_RS232;
			break;
		case '2':
			{
				char *SoftRel;

				SoftRel = CAENHVLibSwRel();
				if( !strncmp(SoftRel,"2.0",3) )
					con_printf("\nA303 Address and CAENET Crate Nr. (i.e. 300_1) : ");
				else if ( SoftRel[0] == '2' )		// Rel > 2.0 ...
				{
					con_printf("\n\n Please enter a string in the format: CARD_aaa_cc \n");
					con_printf(" where CARD is \"A303\" or \"A303A\" or \"A1303\", cc is the\n");
					con_printf(" caenet crate number and aaa, if CARD == \"A1303\", is the\n");
					con_printf(" index of the board starting from 0, otherwise it is the A303 address:  ");
				}
				con_scanf("%s", arg);
				link = LINKTYPE_CAENET;
				break;
			}
	}

#ifdef EXPLICIT_LOGIN
	if( c != '2' )
	{
		con_printf("User Name: ");
		con_scanf("%s", userName );
		con_printf("Password: ");
		GetPassword(passwd);
	}
#else
	strcpy(userName, "admin");
	strcpy(passwd, "admin");
#endif

	ret = CAENHVInitSystem(name, link, arg, userName, passwd);

	con_printf("\nCAENHVInitSystem: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	con_getch();

	if( ret == CAENHV_OK )
	{
		i = 0;
		while( System[i].ID != -1 ) i++;
		System[i].ID = ret;
		strcpy(System[i].Name, name); 
	}

}

/*****************************************************************************/
/*                                                                           */
/*  HVSystemLOGOUT                                                           */
/*                                                                           */
/*****************************************************************************/
void HVSystemLogout()
{
char name[64];
int i;
CAENHVRESULT ret;

if( noHVPS() )
   return;

if( ( i = OneHVPS() ) >= 0 )
	strcpy(name, System[i].Name);
else
{
	con_printf("\nEnter System Name: ");
	con_scanf("%s", name);
}

ret = CAENHVDeinitSystem(name);
if( ret == CAENHV_OK )
	con_printf("CAENHVDeinitSystem: Connection closed (num. %d)\n\n", ret);
else
	con_printf("CAENHVDeinitSystem: %s (num. %d)\n\n", CAENHVGetError(name), ret);
con_getch();

if( ret == CAENHV_OK )
  {
   i = 0;
   while( strcmp(System[i].Name,name) ) i++;
   for( ; System[i].ID != -1; i++ )
     {
      System[i].ID = System[i+1].ID;
	  strcpy(System[i].Name, System[i+1].Name);
	 }
  }

return;
}

/*****************************************************************************/
/*                                                                           */
/*  HVLIBSWREL                                                               */
/*                                                                           */
/*****************************************************************************/
void HVLibSwRel()
{
	char *SoftRel;

	SoftRel = CAENHVLibSwRel();

	con_printf("\nCAEN HV Wrapper Library Release: %s", SoftRel);
	con_getch();
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETCHNAME                                                              */
/*                                                                           */
/*****************************************************************************/
void HVGetChName()
{
char name[64];
int i, temp;
CAENHVRESULT ret;
unsigned short slot, NrOfCh, n, listaCh[2048], Ch;
char  (*listNameCh)[MAX_CH_NAME];

if( noHVPS() )
   return;

if( ( i = OneHVPS() ) >= 0 )
	strcpy(name, System[i].Name);
else
{
	con_printf("\nEnter System Name: ");
	con_scanf("%s", name);
}

/* slot */
con_printf("Enter Slot: ");
con_scanf("%d", &temp);
slot = temp;

/* numero di canali */
con_printf("Enter Number of Channel(s): ");
con_scanf("%d", &temp);
NrOfCh = temp;

if( NrOfCh > 3 )
  {
   clrscr();
   gotoxy(1,2);
  }

for( n = 0; n < NrOfCh; n++ )
    {
     con_printf("Enter Channel:");
     con_scanf("%d", &temp);
     Ch = temp;
     listaCh[n] = Ch;
    }

listNameCh = malloc(NrOfCh*MAX_CH_NAME);

do{
	ret = CAENHVGetChName(name, slot, NrOfCh, listaCh, listNameCh);
	if( ret != CAENHV_OK )
	   {
	    free(listNameCh);
	    con_printf("CAENHVGetChName: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	    con_getch();
	    return;
	   }
	else
	   {
	    clrscr();
	    gotoxy(1,2);
	    con_printf("CHANNEL NAME\n");    
	    for( n = 0; n < NrOfCh; n++ )
	       con_printf("Channel n. %d: %s\n", listaCh[n], listNameCh[n]);
	   }
   if( con_kbhit() ) break;   
  }
while(loop);

fflush(stdout);
con_getch();
/* per quando era definito come un array di puntatori a char
for( n = 0; n < NrOfCh; n++)
    free(listNameCh[n]);
*/
free(listNameCh);

return;
}

/*****************************************************************************/
/*                                                                           */
/*  HVSETCHNAME                                                              */
/*                                                                           */
/*****************************************************************************/
void HVSetChName(void)
{
int i, temp;
CAENHVRESULT ret;
unsigned short Slot, NrOfCh, n, listaCh[2048], Ch;
char ChName[20], name[64];;

if( noHVPS() )
   return;

if( ( i = OneHVPS() ) >= 0 )
	strcpy(name, System[i].Name);
else
{
	con_printf("\nEnter System Name: ");
	con_scanf("%s", name);
}

clrscr();
gotoxy(1,2);

/* slot */
con_printf("Enter Slot: ");
con_scanf("%d", &temp);
Slot = temp;

/* Ch name */
con_printf("Ch name: ");
con_scanf("%s", ChName);

/* numero di canali */
con_printf("Enter Number of Channel(s): ");
con_scanf("%d", &temp);
NrOfCh = temp;

for( n = 0; n < NrOfCh; n++ )
  {
   con_printf("Enter Channel:");
   con_scanf("%d", &temp);
   Ch = temp;
   listaCh[n] = Ch;
  }

ret = CAENHVSetChName(name, Slot, NrOfCh, listaCh, ChName);
con_printf("CAENHVSetChName: %s (num. %d)\n\n", CAENHVGetError(name), ret);
con_getch();

return;
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETCHPARAMPROP                                                         */
/*                                                                           */
/*****************************************************************************/
void HVGetChParamProp()
{
	int				i, temp, parNum;
	unsigned short	Slot, Ch;
	CAENHVRESULT	ret;
	char			name[64];
	char			*ParNameList = (char *)NULL;
	char			(*par)[MAX_PARAM_NAME];
	ParProp			*pp = NULL;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

/* slot */
	con_printf("Enter Slot: ");
	con_scanf("%d", &temp);
	Slot = temp;

/* channel */
	con_printf("Enter Channel:");
	con_scanf("%d", &temp);
	Ch = temp;
   
	clrscr();
	gotoxy(1,2);  

do{
	ParNameList = (char *)NULL;
	pp = NULL;
	ret = CAENHVGetChParamInfo(name, Slot, Ch, &ParNameList);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetChParamInfo: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		con_getch();
		return;
	}

	par = (char (*)[MAX_PARAM_NAME])ParNameList;

	for( i = 0 ; par[i][0] ; i++ );

	parNum = i;
	pp = calloc(parNum, sizeof(ParProp));

	for( i = 0 ; i < parNum ; i++ )
	{
		ret = ReadParamProp(name, Slot, Ch, par[i], &pp[i]);
		if( ret != CAENHV_OK )
			break;
	}

	if( ret == CAENHV_OK )
		DisplayParamProp(Slot, Ch, par, parNum, pp);

	if( ParNameList != NULL )
		free(ParNameList);
	if( pp != NULL )
		free(pp);
   if( con_kbhit() ) break;   
  }
while(loop);
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETCHPARAM                                                             */
/*                                                                           */
/*****************************************************************************/
void HVGetChParam()
{
	int				i, temp;
	unsigned short	Slot, ChNum, *ChList, Ch;
	float			*fParValList = NULL;
	unsigned long	tipo, *lParValList = NULL;
	char			name[64], ParName[30];
	CAENHVRESULT	ret;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

	clrscr();
	gotoxy(1,2);  

/* slot */
	con_printf("Enter Slot: ");
	con_scanf("%d", &temp);
	Slot = temp;

/* Param name */
	con_printf("Param name: ");
	con_scanf("%s", ParName);

/* numero di canali */
	con_printf("Enter Number of Channel(s): ");
	con_scanf("%d", &temp);
	ChNum = temp;

	ChList = malloc(ChNum * sizeof(unsigned short));
	for( i = 0; i < ChNum; i++ )
	{
		con_printf("Enter Channel:");
		con_scanf("%d", &temp);
		Ch = temp;
		ChList[i] = Ch;
	} 

do{
	ret = CAENHVGetChParamProp(name, Slot, ChList[0], ParName, "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		goto end;
	}
	
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		fParValList = malloc(ChNum*sizeof(float));
		ret = CAENHVGetChParam(name, Slot, ParName, ChNum, ChList, fParValList);
	}
	else
	{
		lParValList = malloc(ChNum*sizeof(long));
		ret = CAENHVGetChParam(name, Slot, ParName, ChNum, ChList, lParValList);
	}

	if( ret != CAENHV_OK )
		con_printf("CAENHVGetChParam: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	else
	{
		clrscr();
		gotoxy(1,2);
		con_printf("PARAM VALUE");
		if( tipo == PARAM_TYPE_NUMERIC )
			for( i = 0; i < ChNum; i++ )   
				con_printf("\nSlot: %2d  Ch: %3d  %s: %10.2f", Slot, ChList[i],
					                                     ParName, fParValList[i]); 
		else
			for( i = 0; i < ChNum; i++ )   
				con_printf("\nSlot: %2d  Ch: %3d  %s: %x ", Slot, ChList[i],
					                                     ParName, lParValList[i]); 
	}

end:
	if( !loop ) con_getch(); 

	if( tipo == PARAM_TYPE_NUMERIC )
	{
		if( fParValList != NULL )
			free(fParValList); 
	}
	else
	{
		if( lParValList != NULL )
			free(lParValList);
	}

   if( con_kbhit() ) break;   
  }
while(loop);

	free(ChList);
}

/*****************************************************************************/
/*                                                                           */
/*  HVSETCHPARAM                                                             */
/*                                                                           */
/*****************************************************************************/
void HVSetChParam()
{
	int				i, temp;
	unsigned short	Slot, ChNum, *ChList;
	float			fParVal;
	unsigned long	tipo, lParVal;
	char			name[64], ParName[30];
	CAENHVRESULT	ret;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

	clrscr();
	gotoxy(1,2);  

/* slot */
	con_printf("Enter Slot: ");
	con_scanf("%d", &temp);
	Slot = temp;

/* Param name */
	con_printf("Param name: ");
	con_scanf("%s", ParName);

/* numero di canali */
	con_printf("Enter Number of Channel(s): ");
	con_scanf("%d", &temp);
	ChNum = temp;

	ChList = malloc(ChNum * sizeof(unsigned short));
	for( i = 0; i < ChNum; i++ )
	{
		con_printf("Enter Channel:");
		con_scanf("%d", &temp);
		ChList[i] = (unsigned short)temp;
	}	 

	ret = CAENHVGetChParamProp(name, Slot, ChList[0], ParName, "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetChParamProp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		goto end;
	}
	
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		con_printf("Value %s: ",ParName);
		con_scanf("%f", &fParVal);
		ret = CAENHVSetChParam(name, Slot, ParName, ChNum, ChList, &fParVal);
	}
	else
	{
		con_printf("Value %s: ",ParName);
		con_scanf("%ld", &lParVal);
		ret = CAENHVSetChParam(name, Slot, ParName, ChNum, ChList, &lParVal);
	}

	con_printf("CAENHVSetChParam: %s (num. %d)\n\n", CAENHVGetError(name), ret);

end:
	con_getch();
	free(ChList);
}

/*****************************************************************************/
/*                                                                           */
/*  HVTSTBDPRES                                                          */
/*                                                                           */
/*****************************************************************************/
void HVTstBdPres()
{
int i, temp;
unsigned short slot, NrOfCh, serNumb;
unsigned char fmwMax, fmwMin;
char Model[15], Descr[80];
CAENHVRESULT ret; 
char name[64];

if( noHVPS() )
   return;

if( ( i = OneHVPS() ) >= 0 )
	strcpy(name, System[i].Name);
else
{
	con_printf("\nEnter System Name: ");
	con_scanf("%s", name);
}

/* slot */
con_printf("\nEnter Slot: ");
con_scanf("%d", &temp);
slot = temp;

do{
	ret = CAENHVTestBdPresence(name, slot, &NrOfCh, Model, Descr, &serNumb, &fmwMin, &fmwMax);
	if( ret != CAENHV_OK )
	  {
	   con_printf("CAENHVTestBdPreset: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	   con_getch();
	   return;
	  }

	con_printf("Slot %d: Mod. %s %s Nr.Ch: %d  Ser. %d Rel. %d.%d\n",slot,Model,Descr,
                                                    NrOfCh, serNumb,fmwMax,fmwMin);
   if( con_kbhit() ) break;   
  }
while(loop);

fflush(stdout);
con_getch();

return;
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETBDPARAMPROP                                                     */
/*                                                                           */
/*****************************************************************************/
void HVGetBdParamProp()
{
	int				i, temp, parNum;
	unsigned short	Slot;
	CAENHVRESULT	ret;
	char			name[64];
	char			*ParNameList = (char *)NULL;
	char			(*par)[MAX_PARAM_NAME];
	ParProp			*pp = NULL;
	
	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else	
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

/* slot */
	con_printf("Enter Slot: ");
	con_scanf("%d", &temp);
	Slot = temp;

	clrscr();
	gotoxy(1,2);  

do{
	ParNameList = (char *)NULL;
	pp = NULL;
	ret = CAENHVGetBdParamInfo(name, Slot, &ParNameList);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetBdParamInfo: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		con_getch();
		return;
	}

	par = (char (*)[MAX_PARAM_NAME])ParNameList;

	for( i = 0 ; par[i][0] ; i++ );

	parNum = i;
	pp = calloc(parNum, sizeof(ParProp));

	for( i = 0 ; i < parNum ; i++ )
	{
		ret = ReadParamProp(name, Slot, 0xffff, par[i], &pp[i]);
		if( ret != CAENHV_OK )
			break;
	}

	if( ret == CAENHV_OK )
		DisplayParamProp(Slot, 0xffff, par, parNum, pp);

	if( ParNameList != NULL )
		free(ParNameList);
	if( pp != NULL )
		free(pp);
    if( con_kbhit() ) break;   
  }
while(loop);
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETBDPARAM                                                             */
/*                                                                           */
/*****************************************************************************/
void HVGetBdParam()
{
	int				i, temp;
	unsigned short	NrOfSlot, *SlotList;
	float			*fParValList = NULL;
	unsigned long	tipo, *lParValList = NULL;
	char			name[64], ParName[30];
	CAENHVRESULT	ret;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

	clrscr();
	gotoxy(1,2);  

/* Param name */
	con_printf("Param name: ");
	con_scanf("%s", ParName);

/* numero di slot */
	con_printf("Enter Number of Slot(s): ");
	con_scanf("%d", &temp);
	NrOfSlot = temp;

	SlotList = malloc(NrOfSlot * sizeof(unsigned short));
	for( i = 0; i < NrOfSlot ; i++ )
	{
		con_printf("Enter Slot:");
		con_scanf("%d", &temp);
		SlotList[i] = temp;
	} 

do{
	ret = CAENHVGetBdParamProp(name, SlotList[0], ParName, "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		goto end;
	}
	
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		fParValList = malloc(NrOfSlot*sizeof(float));
		ret = CAENHVGetBdParam(name, NrOfSlot, SlotList, ParName, fParValList);
	}
	else
	{
		lParValList = malloc(NrOfSlot*sizeof(long));
		ret = CAENHVGetBdParam(name, NrOfSlot, SlotList, ParName, lParValList);
	}

	if( ret != CAENHV_OK )
		con_printf("CAENHVGetBdParam: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	else
	{
		clrscr();
		gotoxy(1,2);
		con_printf("PARAM VALUE");
		if( tipo == PARAM_TYPE_NUMERIC )
			for( i = 0 ; i < NrOfSlot ; i++ )   
				con_printf("\nSlot: %2d  %s: %10.2f", SlotList[i],
					                                  ParName, fParValList[i]); 
		else
			for( i = 0 ; i < NrOfSlot ; i++ )   
				con_printf("\nSlot: %2d  %s: %x ", SlotList[i],
					                               ParName, lParValList[i]); 
	}

end:
	if( !loop ) con_getch(); 

	if( tipo == PARAM_TYPE_NUMERIC )
	{
		if( fParValList != NULL )
			free(fParValList); 
	}
	else
	{
		if( lParValList != NULL )
			free(lParValList);
	}

    if( con_kbhit() ) break;   
  }
while(loop);
	free(SlotList);
}

/*****************************************************************************/
/*                                                                           */
/*  HVSETBDPARAM                                                             */
/*  Rel. 2.7                                                                 */
/*                                                                           */
/*****************************************************************************/
void HVSetBdParam()
{
	int				i, temp;
	unsigned short	NrOfSlot, *SlotList;
	float			fParVal;
	unsigned long	tipo, lParVal;
	char			name[64], ParName[30];
	CAENHVRESULT	ret;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

	clrscr();
	gotoxy(1,2);  

/* Param name */
	con_printf("Param name: ");
	con_scanf("%s", ParName);

/* numero di slot */
	con_printf("Enter Number of Slot(s): ");
	con_scanf("%d", &temp);
	NrOfSlot = temp;

	SlotList = malloc(NrOfSlot * sizeof(unsigned short));
	for( i = 0; i < NrOfSlot ; i++ )
	{
		con_printf("Enter Slot:");
		con_scanf("%d", &temp);
		SlotList[i] = temp;
	} 

	ret = CAENHVGetBdParamProp(name, SlotList[0], ParName, "Type", &tipo);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetBdParamProp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		goto end;
	}
	
	if( tipo == PARAM_TYPE_NUMERIC )
	{
		con_printf("Value %s: ",ParName);
		con_scanf("%f", &fParVal);
		ret = CAENHVSetBdParam(name, NrOfSlot, SlotList, ParName, &fParVal);
	}
	else
	{
		con_printf("Value %s: ",ParName);
		con_scanf("%ld", &lParVal);
		ret = CAENHVSetBdParam(name, NrOfSlot, SlotList, ParName, &lParVal);
	}

	con_printf("CAENHVSetBdParam: %s (num. %d)\n\n", CAENHVGetError(name), ret);

end:
	con_getch();
	free(SlotList);
}

#ifdef pippo
/*****************************************************************************/
/*                                                                           */
/*  HVGETGRPCOMP                                                         */
/*                                                                           */
/*****************************************************************************/
void HVGetGrpComp()
{
int temp, i, j;
unsigned short ChNum, group;
unsigned long *ChList = (unsigned long *)NULL;
CAENHVRESULT ret;
char name[64];

con_printf("\nEnter System Name: ");
con_scanf("%s", name);

/* group */
con_printf("\nEnter Group: ");
con_scanf("%d", &temp);
group = temp;

ret = CAENHVGetGrpComp(name, group, &ChNum, &ChList);
if( ret != CAENHV_OK )
   {
    con_printf("CAENHVGetGrpComp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
    con_getch();
    return;
   }

clrscr();
gotoxy(1,2);
con_printf("GROUP %d",group);
for( i = 0, j = 0; i < ChNum; i++, j++ )
  {
   if( j == 80 )
     {
	  con_getch();
      clrscr();
	  gotoxy(1,2);
	  con_printf("GROUP %d",group);
	  j = 0;
	 }
   gotoxy((j/20)*16+1, j%20+3);
   con_printf("Slot %d  Ch %d", (ChList[i])>>16, ChList[i]&0xffff);
  }
con_getch();

free(ChList);

return;
}

/*****************************************************************************/
/*                                                                           */
/*  HVADDCHTOGRP                                                         */
/*                                                                           */
/*****************************************************************************/
void HVAddChToGrp(void)
{
int temp;
unsigned short Group, NrOfCh, i;
unsigned long ChList[1000], templ;
CAENHVRESULT ret;
char name[64];

con_printf("\nEnter System Name: ");
con_scanf("%s", name);

/* Group */
con_printf("Enter Group: ");
con_scanf("%d", &temp);
Group = temp;

/* numero di canali */
con_printf("Enter Number of Channel(s): ");
con_scanf("%d", &temp);
NrOfCh = temp;

clrscr();
gotoxy(1,2);
for( i = 0; i < NrOfCh; i++ )
  {
   con_printf("Enter Slot (crate+slot):");
   con_scanf("%d", &temp);
   templ = temp;

   con_printf("Enter Ch:");
   con_scanf("%d", &temp);

   ChList[i] = (templ<<16)+temp;
  }

ret = CAENHVAddChToGrp(name, Group, NrOfCh, ChList);
con_printf("CAENHVAddChToGrp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
con_getch();

return;
}

/*****************************************************************************/
/*                                                                           */
/*  HVREMCHFROMGRP                                                       */
/*                                                                           */
/*****************************************************************************/
void HVRemChFromGrp(void)
{
int temp;
unsigned short Group, NrOfCh, i;
unsigned long ChList[1000], templ;
CAENHVRESULT ret;
char name[64];

con_printf("\nEnter System Name: ");
con_scanf("%s", name);

/* Group */
con_printf("Enter Group: ");
con_scanf("%d", &temp);
Group = temp;

/* numero di canali */
con_printf("Enter Number of Channel(s): ");
con_scanf("%d", &temp);
NrOfCh = temp;

clrscr();
gotoxy(1,2);
for( i = 0; i < NrOfCh; i++ )
  {
   con_printf("Enter Slot (crate+slot):");
   con_scanf("%d", &temp);
   templ = temp;

   con_printf("Enter Ch:");
   con_scanf("%d", &temp);

   ChList[i] = (templ<<16)+temp;
  }

ret = CAENHVRemChToGrp(name, Group, NrOfCh, ChList);
con_printf("CAENHVRemChToGrp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
con_getch();

return;
}
#endif

/*****************************************************************************/
/*                                                                           */
/*  HVGETCRATEMAP                                                        */
/*                                                                           */
/*****************************************************************************/
void HVGetCrateMap()
{ 
	unsigned short	NrOfSl, *SerNumList, *NrOfCh;
	char			*ModelList, *DescriptionList;
	unsigned char	*FmwRelMinList, *FmwRelMaxList;
	CAENHVRESULT	ret;
	char			name[64];
	int				i;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

do{
	ret = CAENHVGetCrateMap(name, &NrOfSl, &NrOfCh, &ModelList, &DescriptionList, &SerNumList,
                                  &FmwRelMinList, &FmwRelMaxList );
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetCrateMap: %s (num. %d)\n\n", CAENHVGetError(name), ret);
		if( !loop )
			con_getch();
	}
	else
	{
		int		i;
		char	*m = ModelList, *d = DescriptionList;

		clrscr();
		gotoxy(1,2);
		con_printf("System MAP");
		for( i = 0; i < NrOfSl; i++ , m += strlen(m) + 1, d += strlen(d) + 1 )
			if( *m == '\0' )
				con_printf("\nBoard %2d: Not Present", i);
			else             
				con_printf("\nBoard %2d: %s %s  Nr. Ch: %d  Ser. %d   Rel. %d.%d",
							i, m, d, NrOfCh[i], SerNumList[i], FmwRelMaxList[i], 
									 FmwRelMinList[i]);
		if( !loop ) con_getch(); 

		free(SerNumList), free(ModelList), free(DescriptionList), free(FmwRelMinList),
		free(FmwRelMaxList), free(NrOfCh);
	}
    if( con_kbhit() ) break;   
  }
while(loop);
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETSYSCOMP                                                         */
/*                                                                           */
/*****************************************************************************/
#ifdef pippo
void HVGetSysComp()
{
int i, j;
char NrOfCr;
unsigned short *CrNrOfSlList = (unsigned short *)NULL;
unsigned long *SlotChList = (unsigned long *)NULL;
CAENHVRESULT ret;
char name[64];

if( ( i = OneHVPS() ) >= 0 )
	strcpy(name, System[i].Name);
else
{
	con_printf("\nEnter System Name: ");
	con_scanf("%s", name);
}

ret = CAENHVGetSysComp(name, &NrOfCr, &CrNrOfSlList,  &SlotChList);                  
if( ret != CAENHV_OK )
  {
   con_printf("CAENHVGetSysComp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
   con_getch();
   return;
  }

clrscr();
gotoxy(1,2);
for( i = 0; i< NrOfCr; i++ )
  for( j = 0; j < (CrNrOfSlList[i]&0xff); j++ )
    con_printf("Crate Nr. %d  Board: %d  Nr. of Ch.: %d\n", 
	  CrNrOfSlList[i]>>8, (SlotChList[j]>>16)&0xff, SlotChList[j]&0xffff);
if( NrOfCr) con_getch();

free(CrNrOfSlList);
free(SlotChList);

return;
}
#endif

/*****************************************************************************/
/*                                                                           */
/*  HVGETEXECLIST															 */
/*                                                                           */
/*****************************************************************************/
void HVGetExecList()
{
	int				i;
	unsigned short	NrOfExec;
	char			*c;
	char			*ExecList = (char *)NULL;
	CAENHVRESULT	ret;
	char			name[64];

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

do{
	ExecList = (char *)NULL;
	ret = CAENHVGetExecCommList(name, &NrOfExec, &ExecList);
                                                 
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetExecCommList: %s (num. %d)\n\n", 
			        CAENHVGetError(name), ret);
		con_getch();
	}
	else
	{
		c = ExecList;
		clrscr();
		gotoxy(1,2);
		con_printf("EXECUTE COMMAND LIST");
		for( i = 0; i < NrOfExec; i++, c += strlen(c) + 1 )   
		{
			gotoxy((i>>4)*30+1, i%16+3);
			con_printf("Execute Command %2d: %s", i, c);
		}
		if( !loop ) con_getch();

		if( ExecList != NULL )
			free(ExecList);
	}
    if( con_kbhit() ) break;   
  }
while(loop);
}

/*****************************************************************************/
/*                                                                           */
/*  HVGETSYSPROP                                                             */
/*                                                                           */
/*****************************************************************************/
void HVGetSysProp()
{
	union	{
		char			cBuff[4096];
		float			fBuff;
		unsigned short	ui2Buff;
		unsigned long	ui4Buff;
		short			i2Buff;
		long			i4Buff;
		unsigned		bBuff;
	}				app;
	CAENHVRESULT	ret;
	char			name[64];
	int				i;
	unsigned short	NrOfProp;
	char			*p;
	char			*PropList = (char *)NULL;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

do{
	PropList = (char *)NULL;
	ret = CAENHVGetSysPropList(name, &NrOfProp, &PropList);                                                 
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetSysPropList: %s (num. %d)\n\n", 
			        CAENHVGetError(name), ret);
		con_getch();
	}
	else
	{
		p = PropList;    
		clrscr();
		gotoxy(1,2);
		con_printf("Property Name         Property Value       Property Mode  Property Type");
		gotoxy(1,3);
		con_printf("-------------         ------------------   -------------  -------------");

		for( i = 0 ; i < NrOfProp ; i++, p += strlen(p) + 1 )   
		{
			unsigned Mode, Type;

			ret = CAENHVGetSysPropInfo(name, p, &Mode, &Type);
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVGetSysPropInfo: %s (num. %d)\n\n", 
				            CAENHVGetError(name), ret);
				goto end;
			}

			ret = CAENHVGetSysProp(name, p, &app);
			if(   ret != CAENHV_OK && ret != CAENHV_GETPROPNOTIMPL 
         && ret != CAENHV_NOTGETPROP )
			{
				con_printf("CAENHVGetSysProp: %s (num. %d)\n\n", 
				            CAENHVGetError(name), ret);
				goto end;
			}

			gotoxy(1, 4+i);
			con_printf("%-17s",p);
			gotoxy(23, 4+i);
			if( Mode == SYSPROP_MODE_WRONLY || ret == CAENHV_GETPROPNOTIMPL 
        || ret == CAENHV_NOTGETPROP )
				con_printf("------------------");
			else
				switch( Type )
				{
				case SYSPROP_TYPE_STR:
					app.cBuff[18] = '\0';
					con_printf("%-17s", app.cBuff);
					break;

				case SYSPROP_TYPE_REAL:
					con_printf("%-17.2f", app.fBuff);
					break;

				case SYSPROP_TYPE_UINT2:
					con_printf("%-x", app.ui2Buff);
					break;

				case SYSPROP_TYPE_UINT4:
					con_printf("%-x", app.ui4Buff);
					break;

				case SYSPROP_TYPE_INT2:
					con_printf("%-d", app.i2Buff);
					break;

				case SYSPROP_TYPE_INT4:
					con_printf("%-d", app.i4Buff);
					break;

				case SYSPROP_TYPE_BOOLEAN:
					con_printf("%-d", app.bBuff);
					break;
			}
			
			gotoxy(43, 4+i);
			con_printf(" %-17s",SysPropModeStr[Mode]);
			gotoxy(59, 4+i);
			con_printf("%-17s",SysPropTypeStr[Type]);
		}

end:
		if( !loop ) con_getch();
		if( PropList != NULL )
			free(PropList);
	}
    if( con_kbhit() ) break;   
  }
while(loop);
}

/*****************************************************************************/
/*                                                                           */
/*  HVSETSYSPROP                                                             */
/*                                                                           */
/*****************************************************************************/
void HVSetSysProp()
{
	int				i, temp;
	unsigned		Mode, Type;
	CAENHVRESULT	ret;
	char			name[64], SetPropName[40];
	union	{
		char			cBuff[4096];
		float			fBuff;
		unsigned short	ui2Buff;
		unsigned long	ui4Buff;
		short			i2Buff;
		long			i4Buff;
		unsigned		bBuff;
	}				app;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

/* Set Property name */
	con_printf("Set Property Name: ");
	con_scanf("%s", SetPropName);

	ret = CAENHVGetSysPropInfo(name, SetPropName, &Mode, &Type);
	if( ret != CAENHV_OK )
	{
		con_printf("CAENHVGetSysPropInfo: %s (num. %d)\n\n", 
		            CAENHVGetError(name), ret);
		goto end;
	}

	con_printf("Enter Property Value: ");   

	switch( Type )
	{
	case SYSPROP_TYPE_STR:
		con_scanf("%s",app.cBuff);
		break;

	case SYSPROP_TYPE_REAL:
		con_scanf("%f", &app.fBuff);
		break;

	case SYSPROP_TYPE_UINT2:
		con_scanf("%x", &temp);
		app.ui2Buff = temp;
		break;

	case SYSPROP_TYPE_UINT4:
		con_scanf("%x", &app.ui4Buff);
		break;

	case SYSPROP_TYPE_INT2:
		con_scanf("%d", &temp);
		app.i2Buff = temp;
		break;

	case SYSPROP_TYPE_INT4:
		con_scanf("%d", &app.i4Buff);
		break;

	case SYSPROP_TYPE_BOOLEAN:
		con_scanf("%d", &app.bBuff);
		break;
	}

	ret = CAENHVSetSysProp(name, SetPropName, &app);

	con_printf("CAENHVSetSysProp: %s (num. %d)\n\n", CAENHVGetError(name), ret);
end:
	con_getch();
}

/*****************************************************************************/
/*                                                                           */
/*  HVEXECCOMM                                                           */
/*                                                                           */
/*****************************************************************************/
void HVExecComm()
{
	char			ExecCommName[20];
	CAENHVRESULT	ret;
	char			name[64];
	int				i;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

/* Exec Command name */
	con_printf("Execute Command Name: ");
	con_scanf("%s", ExecCommName);

	ret = CAENHVExecComm(name, ExecCommName);

	con_printf("CAENHVExecComm: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	con_getch();
}

/*****************************************************************************/
/*                                                                           */
/*  HVCAENETCOMM                                                             */
/*                                                                           */
/*  Rel. 1.1                                                                 */
/*                                                                           */
/*****************************************************************************/
void HVCaenetComm()
{
	int		temp, i, j;
	unsigned short	crate, code, NrWCode, WCode[20], NrOfData;
	unsigned short  *Data = (unsigned short *)NULL;
	char			name[64];
	short           ErrorCodeCaenet;
	CAENHVRESULT	ret;

	if( noHVPS() )
		return;

	if( ( i = OneHVPS() ) >= 0 )
		strcpy(name, System[i].Name);
	else
	{
		con_printf("\nEnter System Name: ");
		con_scanf("%s", name);
	}

	clrscr();
	gotoxy(1,2);  

/* crate */
        con_printf("\nEnter Crate Number: ");
        con_scanf("%d", &temp);
        crate = temp;

/* code */
        con_printf("\nEnter CAENET Code : ");
        con_scanf("%d", &temp);
        code = temp;

/* NrWCode */
        con_printf("\nEnter Nr of additional word code : ");
        con_scanf("%d", &temp);
        NrWCode = temp;

        for( i = 0; i < NrWCode; i++ )
        {
         con_printf("\nEnter word code %d: ", i+1);
         con_scanf("%d", &temp);
         WCode[i] = temp;    
        }

	ret = CAENHVCaenetComm(name,crate,code,NrWCode,WCode,&ErrorCodeCaenet,&NrOfData,&Data);
	if( ret != CAENHV_OK )
		con_printf("CAENHVCaenetComm: %s (num. %d)\n\n", CAENHVGetError(name), ret);
	else
	{
		clrscr();
		gotoxy(1,2);
		con_printf("CAENET ERROR CODE: %x\n", ErrorCodeCaenet);
                con_printf("DATA");
                for( i = 0, j = 0; i < NrOfData; i++, j++ )
                {
                 if( j == 80 )
                   {
	            con_getch();
                    clrscr();
                    gotoxy(1,3);
                    con_printf("CAENET ERROR CODE: %x\n", ErrorCodeCaenet);
                    con_printf("DATA");
                    j = 0;
	           }
                 gotoxy((j/20)*16+1, j%20+4);
                 con_printf("Data %d: %x", i, Data[i]);
                }
	}

	con_getch();
        free(Data);
}

/*****************************************************************************/
/*                                                                           */
/*  HVNOFUNCTION                                                         */
/*                                                                           */
/*****************************************************************************/
void HVnoFunction()
{
}

/*****************************************************************************/
/*                                                                           */
/*  QUITPROGRAM                                                              */
/*                                                                           */
/*  Aut: A. Morachioli                                                       */
/*                                                                           */
/*****************************************************************************/
void quitProgram(void)
{
	int i;
	CAENHVRESULT ret;

	i = 0;
	for( i = 0 ; i < MAX_HVPS ; i++ )
		if( System[i].ID != -1 )
		{
			ret = CAENHVDeinitSystem(System[i].Name);
			if( ret != CAENHV_OK )
			{
				con_printf("CAENHVDeinitSystem: %s (num. %d)\n\n", 
							CAENHVGetError(System[i].Name), ret);
				con_getch();
			}
		}
	con_end();
	exit(0);
}

