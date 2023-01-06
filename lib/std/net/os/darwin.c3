module std::net::os;
import libc;

$if (env::OS_TYPE == OsType.MACOSX):

const AI_NUMERICSERV = 0x1000;
const AI_ALL = 0x100;
const AI_V4MAPPED_CFG = 0x200;
const AI_ADDRCONFIG = 0x400;
const AI_V4MAPPED = 0x800;
const AI_UNUSABLE = 0x10000000;
const AI_DEFAULT = AI_V4MAPPED_CFG | AI_ADDRCONFIG;

struct AddrInfo
{
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	usz ai_addrlen;
    void* ai_addr;
	char* ai_canonname;
	AddrInfo* ai_next;
}

const int PLATFORM_AF_UNIX = 1;
const int PLATFORM_AF_INET = 2;
const int PLATFORM_AF_IMPLINK = 3;
const int PLATFORM_AF_PUP = 4;
const int PLATFORM_AF_CHAOS = 5;
const int PLATFORM_AF_NS = 6;
const int PLATFORM_AF_ISO = 7;
const int PLATFORM_AF_ECMA = 8;
const int PLATFORM_AF_DATAKIT = 9;
const int PLATFORM_AF_CCITT = 10;
const int PLATFORM_AF_SNA = 11;
const int PLATFORM_AF_DECNET = 12;
const int PLATFORM_AF_DLI = 13;
const int PLATFORM_AF_LAT = 14;
const int PLATFORM_AF_HYLINK = 15;
const int PLATFORM_AF_APPLETALK = 16;
const int PLATFORM_AF_ROUTE = 17;
const int PLATFORM_AF_LINK = 18;
const int PLATFORM_PSEUDO_AF_XTP = 19;
const int PLATFORM_AF_COIP = 20;
const int PLATFORM_AF_CNT = 21;
const int PLATFORM_PSEUDO_AF_RTIP = 22;
const int PLATFORM_AF_IPX = 23;
const int PLATFORM_AF_SIP = 24;
const int PLATFORM_PSEUDO_AF_PIP = 25;
const int PLATFORM_AF_NDRV = 27;
const int PLATFORM_AF_ISDN = 28;
const int PLATFORM_PSEUDO_AF_KEY = 29;
const int PLATFORM_AF_INET6 = 30;
const int PLATFORM_AF_NATM = 31;
const int PLATFORM_AF_SYSTEM = 32;
const int PLATFORM_AF_NETBIOS = 33;
const int PLATFORM_AF_PPP = 34;
const int PLATFORM_PSEUDO_AF_HDRCMPLT = 35;
const int PLATFORM_AF_IEEE80211 = 37;
const int PLATFORM_AF_UTUN = 38;
const int PLATFORM_AF_VSOCK = 40;
const int PLATFORM_AF_MAX = 41;

const int PLATFORM_O_NONBLOCK = 0x30;

$endif;