#include "fbxsrvapp.h"

/*************************************************************************************
                                    Constructeur
*************************************************************************************/
fbxSrvApp::fbxSrvApp(string appID, string appName, string appVersion, string deviceName)
{
    //ctor
    this->appID = appID;
    this->appName = appName;
    this->appVersion = appVersion;
    this->deviceName = deviceName;

}
/*************************************************************************************
                                    Destructeur
*************************************************************************************/
fbxSrvApp::~fbxSrvApp()
{
    //dtor
}

/*************************************************************************************
                                    Start Session
Ouvre une ssession :
 -Vérifie la présence d'un "token"
 -Le récupère si besoin
 -Récupère le challenge
 -Généère le mot de passe pour l'ouverture de session
 -Récupère le token de session
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête du challenge : Retour négatif
    -2  Echec de la requête du challenge : Pas de retour
    -3  Echec de la requête du ouverture de session : Retour négatif
    -4  Echec de la requête du ouverture de session : Pas de retour
    -5  Echec validation accès sur freebox : Timeout
    -6  Echec validation accès sur freebox : Refusé
    -7  Echec validation accès sur freebox : Token invalide ou révoqué
    -8  Echec validation accès sur freebox : Retour négatif de la requête
    -9  Echec validation accès sur freebox : Pas de retour de la requête

*************************************************************************************/
char fbxSrvApp::startSession(){
    //cout<<"Starting session on Fbx Server"<<endl;
    //cout<<" - Verifying token presence"<<endl;
    if(this->hasToken()==-1){
        //cout<<" No token found";
        char test_auth = this->getAuthorization();
        if(test_auth!=0){
           //cout<<endl<< "Relaunch App and valid access on freebox server"<<endl;
           return test_auth;
        }

    }
    //cout<<" - Reading Token"<<endl;
    this->readToken();

    /*  Getting the challenge value */
    //cout<<" - Getting challenge"<<endl;
    string url = "https://mafreebox.freebox.fr/api/v8/login/";
    rapidjson::Document answer = fbxAnswer("GET",url);
    string pwd;
    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
                string challenge;
                chartostring((char*)answer["result"]["challenge"].GetString(),challenge);
                //cout<<"CHALLENGE : "<< answer["result"]["challenge"].GetString()<<endl;
                //cout<<"CHALLENGE : "<< challenge<<endl;
                /* Compute pwd  */
                //cout<<" - Compute Password"<<endl;
                pwd=this->computepwd(challenge);
        }
        else return -1;
    }
    else return -2;

                /*  Opening a session   */
    //cout<<" - Request Opening session"<<endl;
    url = "https://mafreebox.freebox.fr/api/v8/login/session/";
    rapidjson::Document data;
    data.SetObject();
    data.AddMember("app_id",rapidjson::StringRef(this->appID.c_str()),data.GetAllocator());
    data.AddMember("password",rapidjson::StringRef(pwd.c_str()),data.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);
    //cout<<pwd<<endl;
    //cout<<"Password :"<<buffer.GetString()<<endl;

    answer.RemoveAllMembers();
    answer = fbxAnswer("POST",url,buffer.GetString(),pwd);
    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
            //cout<<" - Session opened"<<endl<<endl;
            this->sessionToken = answer["result"]["session_token"].GetString();
            return 0;
        }
        return -3;
    }
    //cout<<endl<<endl<<"Opening session error : "<<answer2["msg"].GetString()<<endl<<endl;
    return -4;
}

/*************************************************************************************
                                    Stop Session
Ferme une ssession :
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête du challenge : Retour négatif
    -2  Echec de la requête du challenge : Pas de retour

*************************************************************************************/
char fbxSrvApp::closeSession(){
    string url = "https://mafreebox.freebox.fr/api/v8/login/logout/";
    rapidjson::Document answer = fbxAnswer("POST",url,"",this->sessionToken);

    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
            //cout<<endl<<endl<<"Session closed "<<endl;
            return 0;
        }
        //cout<<endl<<endl<<"Session not closed "<<endl;
        return -1;
    }
    return -2;
}

/*************************************************************************************
                                    hasToken
Ouvre une ssession :
 -Vérifie la présence d'un "token"
 -Retour:
    0   Présence du fichier à la racine
    -1  Absence du fichier à la racine

*************************************************************************************/
char fbxSrvApp::hasToken(){
    ifstream fichier("token");
    if(fichier){
        //cout<<"     - Token found"<<endl;
        fichier.close();
        return 0;
    }
    //cout<<"     - No token found"<<endl;
    return -1;
}

/*************************************************************************************
                                    storeToken
Enregistre le Token d'accès à la freebox dans un fichier à la racine :
 -Retour:
    0   Fichier écrit
    -1  Erreur de création du fichier

*************************************************************************************/
char fbxSrvApp::storeToken(){
    ofstream tokenFile("token");
    if(tokenFile){
        tokenFile<<this->token;
        tokenFile.close();
        return 0;
    }
    return -2;
}

/*************************************************************************************
                                    readToken
Lit le token dans le fichier token
 -Retour:
    0   token lu et stocker dans la propriété token de l'objet
    -1  Erreur d'ouverture du fichier

*************************************************************************************/
char fbxSrvApp::readToken(){
    ifstream tokenFile("token");
    if(tokenFile){
        getline(tokenFile, this->token);
        tokenFile.close();
        return 0;
    }
    return -2;

}

/*************************************************************************************
                                    computepwd
Génère le mot de passe pour l'ouverture de session (Dépendance à openssl)
 -HMAC-SHA1(token,challenge)
 -Conversion en chaîne hexadecimale

 -Paramètre :
    key : challenge fournit par l'API freebox
 -Retour:
    pwd : mot de passe

*************************************************************************************/
string fbxSrvApp::computepwd(string key){
    unsigned char *char_key=NULL;
    stringtoUchar(key,char_key);
    unsigned char *char_token=NULL;
    stringtoUchar(this->token,char_token);

    unsigned char* digest;
    digest=(unsigned char*)malloc(21);
    unsigned int result_len=0;
    HMAC(EVP_sha1(),(const unsigned char*)char_token, (int)token.size(), char_key, (int)key.size(), digest, &result_len);
    string pwd;
    Uchartohexstring(digest,result_len, pwd);

    return pwd;
 }

/*************************************************************************************
                                    getAuthorization
Récupération du token du programme
 -Demande l'autorisation d'accès à l'api
 -Validation (par l'utilisateur) sur l'écran de la freebox
 -Stocke le token dans un fichier
 -Retour:
    0   Pas d'erreur
    -5  Echec validation accès sur freebox : Timeout
    -6  Echec validation accès sur freebox : Refusé
    -7  Echec validation accès sur freebox : Token invalide ou révoqué
    -8  Echec validation accès sur freebox : Retour négatif de la requête
    -9  Echec validation accès sur freebox : Pas de retour de la requête

*************************************************************************************/
char fbxSrvApp::getAuthorization(){
    //cout<<"  - Getting Autorization for App"<<endl;
    string url = "https://mafreebox.freebox.fr/api/v8/login/authorize/";

    rapidjson::Document data;
    data.SetObject();
    data.AddMember("app_id",rapidjson::StringRef(this->appID.c_str()),data.GetAllocator());
    data.AddMember("app_name",rapidjson::StringRef(this->appName.c_str()),data.GetAllocator());
    data.AddMember("app_versiont",rapidjson::StringRef(this->appVersion.c_str()),data.GetAllocator());
    data.AddMember("device_name",rapidjson::StringRef(this->deviceName.c_str()),data.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);

    rapidjson::Document answer= fbxAnswer("POST",url,buffer.GetString());

    buffer.Clear();
    writer.Reset(buffer);
    answer.Accept(writer);
    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
            string temporaryToken = answer["result"]["app_token"].GetString();
            cout << "waiting for validation on the freebox ";
            cout.flush();
            string url="https://mafreebox.freebox.fr/api/v8/login/authorize/" + to_string(answer["result"]["track_id"].GetInt());

            rapidjson::Document track = fbxAnswer("GET",url);
            string status = track["result"]["status"].GetString();

            while(status.compare("granted")!=0  && status.compare("timeout")!=0 && status.compare("denied")!=0 && status.compare("unknown")!=0){
                cout  << ".";
                cout.flush();
                sleep(1);
                track = fbxAnswer(string("GET"),url);
                status = track["result"]["status"].GetString();
            }
            cout<<endl;
            if (status.compare("granted")==0){
                //cout<< "       - Access Granted"<<endl;
                this->token = temporaryToken;
                this->storeToken();
                return 0;
            }
            else if(status.compare("timeout")){
                //cout<<"        -Access validation timeout"<<endl;
                    return -5;
            }
            else if(status.compare("denied")==0){
                //cout<<"        -Access validation denied"<<endl;
                return -6;
            }
            //cout<<"        -Access validation status :"<<status<<endl;
            return -7;
        }
        return -8;
    }
    return -9;
}

/*************************************************************************************
                                    fbxAnser
Envoi les requêtes HTTP vers l'API (Dépendance à licurl et openssl)
 -Paramètres :
    methode : type de requête GET / POST / PUT / DELETE
 -Retour:
    answer : réponse de l'API au format Json (Dépendance rapidjson

*************************************************************************************/
rapidjson::Document fbxSrvApp::fbxAnswer(string methode,string url,string data, string auth){
    struct write_memory* response;
    response=(struct write_memory*)malloc(sizeof(write_memory));
    response->memptr=(char*)malloc(1);
    response->size=0;

    curl(methode,url, data, response, auth);
    rapidjson::Document answer;
    answer.SetObject();
    answer.Parse(response->memptr);
    return answer;
}

/*************************************************************************************
                                    listPortForward
Récupère la liste des redirections
et la stocke dans un docment rapidjson passé par référence
 -Paramètres :
    d : Document rapidjson
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête : Retour négatif
    -2  Echec de la requête : Pas de retour

*************************************************************************************/
char fbxSrvApp::listPortForward(rapidjson::Document &d){
    d = fbxAnswer("GET","https://mafreebox.freebox.fr/api/v8/fw/redir/","", this->sessionToken);
    if (d.HasMember("success")){
        if (d["success"].GetBool()){
            return 0;
        }
    }
    return -1;
}

/*************************************************************************************
                                    searchPortFwdId
Récupère l'id d'une redirection de port
 -Paramètres :
    lan_port : port local vers lequel s'effecutue la redirection
    lan_ip : ip local vers laquelle pointe la redirection
  ou
    wan_port : port du serveur redirigé
 -Retour:
    >0  id de la redirection
    0   Pas de redirection vers ce port
    -1  Echec de la requête pour lister toutes les redirections : Retour négatif
    -2  Echec de la requête pour lister toutes les redirections : Pas de retour

*************************************************************************************/
int fbxSrvApp::searchPortFwdId(int lan_port, string lan_ip){
    rapidjson::Document fwdList;
    char test = this->listPortForward(fwdList);
    if (test == 0){
        if(fwdList["success"].GetBool()){
            for (rapidjson::Value::ConstValueIterator itr = fwdList["result"].Begin(); itr != fwdList["result"].End(); ++itr){
                if(itr->GetObject()["lan_port"].GetInt()==lan_port && itr->GetObject()["lan_ip"].GetString()==lan_ip){
                    int id = itr->GetObject()["id"].GetInt();
                    return id;
                }
            }

        }
        //Pas de redirection
        return 0;
    }
    //Echec requete
    return test;
}

int fbxSrvApp::searchPortFwdId(int wan_port){
    rapidjson::Document fwdList;
    char test = this->listPortForward(fwdList);
    if (test == 0){
        if(fwdList["success"].GetBool()){
            for (rapidjson::Value::ConstValueIterator itr = fwdList["result"].Begin(); itr != fwdList["result"].End(); ++itr){
                if(itr->GetObject()["wan_port_start"].GetInt()<=wan_port && itr->GetObject()["wan_port_stop"].GetInt()>=wan_port){
                    int id = itr->GetObject()["id"].GetInt();
                    return id;
                }
            }

        }
        //Pas de redirection
        return 0;
    }
    //Echec requete
    return test;
}

/*************************************************************************************
                                    addPortForward
Crée une redirection de port sur la freebox
 -Paramètres :
    enable : redirection active/inactive
    comment : commentaire
    lan_port : port sur la machine locale
    wan_port_start : début de la plage de ports redirigée
    wan_port_stop : fin de la plage de ports redirigée
    protocol : tcp / udp
    ip_source : 0.0.0.0 pour toutes les ips
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête : Retour négatif
    -2  Echec de la requête : Pas de retour

*************************************************************************************/
char fbxSrvApp::addPortForward(bool enable, string comment,int lan_port,int wan_port_start,
                               int wan_port_end, string lan_ip, string protocol, string ip_source){

    rapidjson::Document d;
    d.SetObject();
    d.AddMember("enabled",enable,d.GetAllocator());
    d.AddMember("comment",rapidjson::StringRef(comment.c_str()),d.GetAllocator());
    d.AddMember("lan_port",lan_port,d.GetAllocator());
    d.AddMember("wan_port_end",wan_port_end,d.GetAllocator());
    d.AddMember("wan_port_start",wan_port_start,d.GetAllocator());
    d.AddMember("lan_ip",rapidjson::StringRef(lan_ip.c_str()),d.GetAllocator());
    d.AddMember("ip_proto",rapidjson::StringRef(protocol.c_str()),d.GetAllocator());
    d.AddMember("src_ip",rapidjson::StringRef(ip_source.c_str()),d.GetAllocator());


    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    rapidjson::Document answer=fbxAnswer("POST","https://mafreebox.freebox.fr/api/v8/fw/redir/",buffer.GetString(), this->sessionToken);
    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
            return 0;
        }
        else{
            return -1;
        }
    }
    return -2;
}

/*************************************************************************************
                                    deletePortForward
Supprime une redirection de port sur la freebox
 -Paramètres :
    lan_port : port local vers lequel s'effecutue la redirection
    lan_ip : ip local vers laquelle pointe la redirection
  ou
    wan_port : port du serveur redirigé
 -Retour:
    0   Pas de redirection vers ce port
    -1  Echec de la requête pour trouver l'id de la redirection : Retour négatif
    -2  Echec de la requête pour trouver l'id de la redirection : Pas de retour
    -3  Echec de la requête de suppression de la redirection : Retour négatif
    -4  Echec de la requête de suppression de la redirection : Pas de retour

*************************************************************************************/
char fbxSrvApp::deletePortForward(int lan_port,string lan_ip){
    int id = searchPortFwdId(lan_port, lan_ip);
    //cout<<"Id :"<<id<<endl;
    if(id>0){
        string url="https://mafreebox.freebox.fr/api/v8/fw/redir/" + to_string(id);
        rapidjson::Document answer=fbxAnswer("DELETE",url,"", this->sessionToken);
        if(answer.HasMember("succes")){
            if(answer["success"].GetBool()){
                return 0;
            }
            else{
                return -3;
            }
        }
        return -4;
    }
    return id;

}

char fbxSrvApp::deletePortForward(int wan_port){
    int id = searchPortFwdId(wan_port);
    //cout<<"Id :"<<id<<endl;
    if(id>0){
        string url="https://mafreebox.freebox.fr/api/v8/fw/redir/" + to_string(id);
        rapidjson::Document answer=fbxAnswer("DELETE",url,"", this->sessionToken);
        if(answer.HasMember("succes")){
            if(answer["success"].GetBool()){
                return 0;
            }
            else{
                return -3;
            }
        }
        return -4;
    }
    return id;

}

/*************************************************************************************
                                    savePortForward
Crée un fichier JSON listant toutes les redirections de port de la freebox
Le fichier contient uniquement les informations nécessaires
à la restauration des redirections
 -Paramètres :
    output : fichier de sortie
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête pour lister toutes les redirections : Retour négatif
    -2  Echec de la requête pour lister toutes les redirections : Pas de retour
    -3  Erreur de création du fichier

*************************************************************************************/
char fbxSrvApp::savePortForward(string output){
    rapidjson::Document fwdList;
    char test = this->listPortForward(fwdList);
    if(test == 0){
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Value newfwd;
        newfwd.SetObject();
        char* id=NULL;
        if(fwdList["success"].GetBool()){
            for (rapidjson::Value::ConstValueIterator itr = fwdList["result"].Begin(); itr != fwdList["result"].End(); ++itr){
                inttochar(itr->GetObject()["id"].GetInt(),id);
                newfwd.AddMember("enabled",itr->GetObject()["enabled"].GetBool(),d.GetAllocator());
                newfwd.AddMember("comment",rapidjson::StringRef(itr->GetObject()["comment"].GetString()),d.GetAllocator());
                newfwd.AddMember("lan_port",itr->GetObject()["lan_port"].GetInt(),d.GetAllocator());
                newfwd.AddMember("wan_port_end",itr->GetObject()["wan_port_end"].GetInt(),d.GetAllocator());
                newfwd.AddMember("wan_port_start",itr->GetObject()["wan_port_start"].GetInt(),d.GetAllocator());
                newfwd.AddMember("lan_ip",rapidjson::StringRef(itr->GetObject()["lan_ip"].GetString()),d.GetAllocator());
                newfwd.AddMember("ip_proto",rapidjson::StringRef(itr->GetObject()["ip_proto"].GetString()),d.GetAllocator());
                newfwd.AddMember("src_ip",rapidjson::StringRef(itr->GetObject()["src_ip"].GetString()),d.GetAllocator());
                d.AddMember(rapidjson::StringRef((char*)id),newfwd,d.GetAllocator());
                newfwd.SetObject();
            }
            free(id); id=NULL;
            ofstream file(output);
            if (file){
                rapidjson::OStreamWrapper osw(file);

                rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
                d.Accept(writer);
                file.close();
                return 0;
            }
            return -3;
        }
    }
    return test;
}

/*************************************************************************************
                                    loadPortForward
Restaure toutes les redirections de port contenues dans le fichier JSON créé par savePortForward

 -Paramètres :
    input : fichier Json contenant toutes les redirections à restaurer
 -Retour:
    0   Pas de redirection vers ce port
    -1  Echec d'une des requêtes de création de redirection: Retour négatif
    -2  Echec d'une des requêtes de création de redirection: : Pas de retour
    -3  Erreur d'ouverture du fichier

*************************************************************************************/
char fbxSrvApp::loadPortForward(string input){
    ifstream file(input);
    if(file){
        rapidjson::Document d;
        rapidjson::IStreamWrapper isw(file);
        d.ParseStream(isw);
        for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin();itr != d.MemberEnd(); ++itr)
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            itr->value.Accept(writer);

            rapidjson::Document answer=fbxAnswer("POST","https://mafreebox.freebox.fr/api/v8/fw/redir/",buffer.GetString(), this->sessionToken);
            if(answer.HasMember("success")){
                if(!answer["success"].GetBool()){
                    return -1;
                }
            }
            else{
                return -2;
            }
        }
        return 0;
    }
    return -3;
}

/*************************************************************************************
                                    updatePortForward
Mets à jour l'état actif/inactif d'une redirection de port sur la freebox
 -Paramètres :
    lan_port : port local vers lequel s'effecutue la redirection
    lan_ip : ip local vers laquelle pointe la redirection
    enable : true = actif / false = inactif
  ou
    wan_port : port du serveur redirigé
    enable : true = actif / false = inactif

 -Retour:
    0   Pas de redirection vers ce port
    -1  Echec de la requête pour trouver l'id de la redirection : Retour négatif
    -2  Echec de la requête pour trouver l'id de la redirection : Pas de retour
    -3  Echec de la requête de suppression de la redirection : Retour négatif
    -4  Echec de la requête de suppression de la redirection : Pas de retour

*************************************************************************************/
char fbxSrvApp::updatePortForward(int lan_port, string lan_ip, bool enable){
    int id = searchPortFwdId(lan_port,lan_ip);
    if(id>0){
        rapidjson::Document d;
        d.SetObject();
        d.AddMember("lan_port",lan_port,d.GetAllocator());
        d.AddMember("enabled",enable,d.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        string url="https://mafreebox.freebox.fr/api/v8/fw/redir/" + to_string(id);
        rapidjson::Document answer=fbxAnswer("PUT",url.c_str(),buffer.GetString(), this->sessionToken);
        if(answer.HasMember("success")){
            if(answer["success"].GetBool()){
                return 0;
            }
            return -3;
        }
        return  -4;
    }
    return id;
}

char fbxSrvApp::updatePortForward(int wan_port, bool enable){
    int id = searchPortFwdId(wan_port);
    if(id>0){
        rapidjson::Document d;
        d.SetObject();
        d.AddMember("enabled",enable,d.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        string url="https://mafreebox.freebox.fr/api/v8/fw/redir/" + to_string(id);
        rapidjson::Document answer=fbxAnswer("PUT",url.c_str(),buffer.GetString(), this->sessionToken);
        if(answer.HasMember("success")){
            if(answer["success"].GetBool()){
                return 0;
            }
            return -3;
        }
        return  -4;
    }
    return id;
}

/*************************************************************************************
                                    listStaticDHCP
Récupère la liste des tous les baux statiques
et la stocke dans un document rapidjson passé par référence
 -Paramètres :
    d : Document rapidjson
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête : Retour négatif
    -2  Echec de la requête : Pas de retour

*************************************************************************************/
char fbxSrvApp::listStaticDHCP(rapidjson::Document &d){
    d = fbxAnswer("GET","https://mafreebox.freebox.fr/api/v8/dhcp/static_lease/","", this->sessionToken);
    if (d.HasMember("success")){
        if (d["success"].GetBool()){
            return 0;
        }
    }
    return -1;
}

/*************************************************************************************
                                    saveStaticDHCP
Crée un fichier JSON listant tous les baux statiques de la freebox
Le fichier contient uniquement les informations nécessaires
à la restauration
 -Paramètres :
    output : fichier de sortie
 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête pour lister tous les baux statiques : Retour négatif
    -2  Echec de la requête pour lister tous les baux statiques : Pas de retour
    -3  Erreur de création du fichier

*************************************************************************************/
char fbxSrvApp::saveStaticDHCP(string output){
    rapidjson::Document dhcpList;
    char test = this->listStaticDHCP(dhcpList);
    if(test == 0){
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Value newdhcp;
        newdhcp.SetObject();
        
        if(dhcpList["success"].GetBool()){
            for (rapidjson::Value::ConstValueIterator itr = dhcpList["result"].Begin(); itr != dhcpList["result"].End(); ++itr){
                
                newdhcp.AddMember("ip",rapidjson::StringRef(itr->GetObject()["ip"].GetString()),d.GetAllocator());
                newdhcp.AddMember("mac",rapidjson::StringRef(itr->GetObject()["mac"].GetString()),d.GetAllocator());
                newdhcp.AddMember("comment",rapidjson::StringRef(itr->GetObject()["comment"].GetString()),d.GetAllocator());
                
                d.AddMember(rapidjson::StringRef(itr->GetObject()["id"].GetString()),newdhcp,d.GetAllocator());

                newdhcp.SetObject();                
            }
            ofstream file(output);
            if (file){
                rapidjson::OStreamWrapper osw(file);

                rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
                d.Accept(writer);
                file.close();
                return 0;
            }
            return -3;
        }
    }
    return test;
}

/*************************************************************************************
                                    loadStaticDHCP
Restaure toutes les redirections de port contenues dans le fichier JSON créé par saveStaticDHCP

 -Paramètres :
    input : fichier Json contenant toutes les redirections à restaurer
 -Retour:
    0   Pas de redirection vers ce port
    -1  Echec d'une des requêtes de création de redirection: Retour négatif
    -2  Echec d'une des requêtes de création de redirection: : Pas de retour
    -3  Erreur d'ouverture du fichier

*************************************************************************************/
char fbxSrvApp::loadStaticDHCP(string input){
    ifstream file(input);
    if(file){
        rapidjson::Document d;
        rapidjson::IStreamWrapper isw(file);
        d.ParseStream(isw);
        for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin();itr != d.MemberEnd(); ++itr)
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            itr->value.Accept(writer);

            rapidjson::Document answer=fbxAnswer("POST","https://mafreebox.freebox.fr/api/v8/dhcp/static_lease/",buffer.GetString(), this->sessionToken);
            if(answer.HasMember("success")){
                if(!answer["success"].GetBool()){
                    return -1;
                }
            }
            else{
                return -2;
            }
        }
        return 0;
    }
    return -3;
}

/*************************************************************************************
                                    addStaticDHCP
Crée un bail statique, pour une adresse MAC, sur la freebox
 -Paramètres :
    lan_ip : ip a affecter à la machine
    mac : adresse de la machine 
    comment : commentaire

 -Retour:
    0   Pas d'erreur
    -1  Echec de la requête : Retour négatif
    -2  Echec de la requête : Pas de retour

*************************************************************************************/
char fbxSrvApp::addStaticDHCP(string lan_ip, string mac, string comment){

    rapidjson::Document d;
    d.SetObject();
    d.AddMember("lan_ip",rapidjson::StringRef(lan_ip.c_str()),d.GetAllocator());
    d.AddMember("mac",rapidjson::StringRef(mac.c_str()),d.GetAllocator());
    d.AddMember("comment",rapidjson::StringRef(comment.c_str()),d.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    rapidjson::Document answer=fbxAnswer("POST","https://mafreebox.freebox.fr/api/v8/dhcp/static_lease/",buffer.GetString(), this->sessionToken);
    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
            return 0;
        }
        else{
            return -1;
        }
    }
    return -2;
}

/*************************************************************************************
                                    deleteStaticDHCP
Crée une redirection de port sur la freebox
 -Paramètres :
    addr : adresse mac ou adresse IP du bail statique
 -Retour:
    0   Pas de bail pour cette adresse
    -1  Echec de la requête pour trouver l'id du bail : Retour négatif
    -2  Echec de la requête pour trouver l'id du bail : Pas de retour
    -3  Echec de la requête de suppression du bail : Retour négatif
    -4  Echec de la requête de suppression du bail : Pas de retour

*************************************************************************************/
char fbxSrvApp::deleteStaticDHCP(string addr){
    string id; 
    char test= searchStaticDHCPId(addr,id);
    //cout<<"Id :"<<id<<endl;
    if(test>0){
        string url="https://mafreebox.freebox.fr/api/v8/dhcp/static_lease/" + id;
        rapidjson::Document answer=fbxAnswer("DELETE",url,"", this->sessionToken);
        if(answer.HasMember("succes")){
            if(answer["success"].GetBool()){
                return 0;
            }
            else{
                return -3;
            }
        }
        return -4;
    }
    return test;

}

/*************************************************************************************
                                    searchStaticDHCPId
Récupère l'id d'un bail static
 -Paramètres :
    addr : adresse mac ou adresse IP du bail statique
    id : vairiable passée par référence pour récupérer l'id du bail
 -Retour:
    1  id trouvé et passé à la référence en paramètre
    0   Pas de bail pour cette adresse
    -1  Echec de la requête pour lister tous les baux : Retour négatif
    -2  Echec de la requête pour lister tous les baux : Pas de retour

*************************************************************************************/
char fbxSrvApp::searchStaticDHCPId(string addr, string &id){
    rapidjson::Document dhcpList;
    char test = this->listStaticDHCP(dhcpList);
    if (test == 0){
        if(dhcpList["success"].GetBool()){
            for (rapidjson::Value::ConstValueIterator itr = dhcpList["result"].Begin(); itr != dhcpList["result"].End(); ++itr){
                if(itr->GetObject()["lan_ip"].GetString()==addr || itr->GetObject()["mac"].GetString()==addr){
                    id = itr->GetObject()["id"].GetString();
                    return 1;
                }
            }

        }
        //Pas de redirection
        return 0;
    }
    //Echec requete
    return test;
}


/*************************************************************************************
                                    reboot
Redemarre le freebox serveur

 -Retour:
    0   Pas d'erreur
    -1  Echec de la requêtes : Retour négatif
    -2  Echaec de la requête : Pas de retour

*************************************************************************************/
char fbxSrvApp::reboot(){
    string url = "https://mafreebox.freebox.fr/api/v8/system/reboot/";
    rapidjson::Document answer= fbxAnswer("POST",url);
    if(answer.HasMember("success")){
        if(answer["success"].GetBool()){
            return 0;
        }
        else{
            return -1;
        }
    }
    return -2;
}



rapidjson::Document fbxSrvApp::listPlayer(){
    return fbxAnswer("GET","https://mafreebox.freebox.fr/api/v8/player/","", this->sessionToken);
}