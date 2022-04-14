class CEtsScrh
{
public:
   ~CEtsScrh();

          void Destructor();
   static void HookScreenSaverMessage(HWND,int);
   static void ActivateScreenSaver(const char *);
   static void SetScreenSaverText(const char *);
   static bool IsScreenSaverActive();
};

