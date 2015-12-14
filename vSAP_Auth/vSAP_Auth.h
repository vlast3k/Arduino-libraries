/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef vSAP_Auth_h
#define vSAP_Auth_h


// library interface description
class vSAP_Auth
{
  // user-accessible "public" interface
  public:
    vSAP_Auth(const char *user, const char *pass);
   

  // library-accessible "private" interface
  private:
    char SAP_AUTH_USER[20];
    char SAP_AUTH_PASS[20];
    int checkSAPAuth(const char *user, const char *pass);
    int sendPing();
    int httpAuthSAP();
};

#endif

