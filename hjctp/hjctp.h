// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HJCTP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HJCTP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HJCTP_EXPORTS
#define HJCTP_API __declspec(dllexport)
#else
#define HJCTP_API __declspec(dllimport)
#endif

// This class is exported from the hjctp.dll
class HJCTP_API Chjctp {
public:
	Chjctp(void);
	// TODO: add your methods here.
};

extern HJCTP_API int nhjctp;

HJCTP_API int fnhjctp(void);
