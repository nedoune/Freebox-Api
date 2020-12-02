#ifndef FBXSRVAPP_H
#define FBXSRVAPP_H

#include <iostream>
#include<fstream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <unistd.h>
#include <curl/curl.h>
#include<openssl/evp.h>
#include<openssl/sha.h>
#include<openssl/hmac.h>
#include<cstring>
#include<iomanip>
#include "mycurl.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"
#include"utils.h"

using namespace std;

class fbxSrvApp
{
    public:
        fbxSrvApp(string appID, string appName, string appVersion, string deviceName);
        virtual ~fbxSrvApp();

        string GetappID() { return appID; }
        void SetappID(string val) { appID = val; }
        string GetappName() { return appName; }
        void SetappName(string val) { appName = val; }
        string GetappVersion() { return appVersion; }
        void SetappVersion(string val) { appVersion = val; }
        string GetdeviceName() { return deviceName; }
        void SetdeviceName(string val) { deviceName = val; }

        /* login */
        //bool isloggedin();
        char startSession();
        char closeSession();

        rapidjson::Document listPlayer();
        /* Port Forward */
        char addPortForward(bool enable, string comment,int lan_port,int wan_port_start,int wan_port_end, string lan_ip, string protocol, string ip_source);
        char deletePortForward(int lan_port, string lan_ip);
        char deletePortForward(int wan_port);
        char savePortForward(string output);
        char loadPortForward(string input);
        char updatePortForward(int lan_port,string lan_ip, bool enable);
        char updatePortForward(int wan_port,bool enable);
//        rapidjson::Document getPortForward();
//        rapidjson::Document getPortForward(int port);

        /* DHCP */

        char saveStaticDHCP(string output);
        char loadStaticDHCP(string input);
        char addStaticDHCP(string lan_ip, string mac, string comment);
        char deleteStaticDHCP(string addr); 
//        char updateDHCP(string mac);
//        rapidjson::Document getStaticDHCP();
//        rapidjson::Document getStaticDHCP(string addr);

        char reboot();

    protected:

    private:
        string appID;
        string appName;
        string appVersion;
        string deviceName;
        string token ="";
        string sessionToken="";

        /* login*/
        char hasToken();
        char getAuthorization();
        char storeToken();
        char readToken();
        string computepwd(string key);

        /*request*/
        rapidjson::Document fbxAnswer(string methode,string url,string data="", string auth="");

        /* Port Forward */
        int searchPortFwdId(int lan_port,string lan_ip);
        int searchPortFwdId(int wan_port);
        char searchStaticDHCPId(string addr, string &id);
        char listPortForward(rapidjson::Document &d);
        char listStaticDHCP(rapidjson::Document &d);
};

#endif // fbxSrvApp_H
