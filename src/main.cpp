#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <fbxsrvapp.h>
#include <mycurl.h>
#include <interface.h>

using namespace std;
using boost::program_options::options_description;



int main(int argc, char *argv[]){
    fbxSrvApp app(string("fbxRedir"),string("fbxRedir"),string("0.1"),string("Raspberry"));
    if(argc > 1){
        string lan_ip="",source_ip="",protocol="",comment="",mac="";
        string file="";
        int lan_port=-1,wan_port_start=-1,wan_port_stop=-1,wan_port=-1;
        bool enabled=false;
        bool open_port=false;
        bool close_port=false;
        bool update_port=false;
        bool save_fwdlist=false;
        bool load_fwdlist=false;
        bool save_staticDHCP=false;
        bool load_staticDHCP=false;
        bool add_staticDHCP=false;
        bool delete_staticDHCP=false;
        bool reboot=false;
        options_description desc;
        desc.add_options()
            ("help,h", "Help screen")
            ("reboot","Redemarre le freebox serveur")
            ("open_port","crée une redirection \n --open_port --lan_port 80 --lan_ip 192.168.0.1 (--wan_port_start 79) (--wan_port_stop 81) (--source_ip 0.0.0.0) (--protocol tcp/udp) (--comment commentaire)")
            ("update_port","modifie une redirection existante \n --update_port --lan_port 80 --lan_ip 192.168.0.1--enabled 1 or \n --update_port --wan_port 80 --enabled 1")("close_port","supprime une redirection \n --close_port --lan_port 80 --lan_ip 192.168.0.1  or \n --close_port --wan_port 80 (supprime la plage deport)") 
            ("lan_port", boost::program_options::value<int>(&lan_port), "port local")
            ("lan_ip", boost::program_options::value<string>(&lan_ip), "ip de la machine locale")
            ("wan_port_start", boost::program_options::value<int>(&wan_port_start), "debut de plage de port redirigée")
            ("wan_port_stop", boost::program_options::value<int>(&wan_port_stop), "fin de plage de port redirigée")
            ("source_ip", boost::program_options::value<string>(&source_ip)->default_value("0.0.0.0"), "ip source")
            ("protocol", boost::program_options::value<string>(&protocol)->default_value("tcp"), "protocol tcp / udp")
            ("comment", boost::program_options::value<string>(&comment)->default_value(""), "commentaire")
            ("enabled", boost::program_options::value<bool>(&enabled), "etat 0/1")
            ("wan_port", boost::program_options::value<int>(&wan_port), "port local")
            ("save_fwdlist", boost::program_options::value<string>(&file), "sauvegarde toutes les redirections dans un fichier Json \n --save_fwdlist file")
            ("load_fwdlist", boost::program_options::value<string>(&file), "restaure les redirections depuis un fichier Json \n --load_fwdlist file")
            ("save_staticDHCP", boost::program_options::value<string>(&file), "sauvegarde tous les baux statiques, dans un fichier Json\n --save_statcDHCP file")
            ("load_staticDHCP", boost::program_options::value<string>(&file), "restaure les baux statiques depuis un fichier Json\n --load_statcDHCP file")
            ("add_staticDHCP", "crée un bail statique \n --add_staticDHCP --mac AA:AA:AA:AA --lan_ip 192.168.0.1 (--comment commentaire)")
            ("delete_staticDHCP", "supprime un bail statique \n --delete_staticDHCP --mac AA:AA:AA:AA or \n --delete_staticDHCP --lan_ip 192.168.0.1")

            ("mac", boost::program_options::value<string>(&mac)->default_value(""), "adresse mac");
        boost::program_options::variables_map vm;
        boost::program_options::store(parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help")){
            std::cout << desc << '\n';
            return 0;
        }

        else if (vm.count("open_port")){
            bool test = true;
            string msg="";
            if (!vm.count("lan_port")){msg.append("lan_port manquant. ");test=false;}
            if (!vm.count("dest_ip")){msg.append("dest_ip manquant. ");test=false;}
            if (!vm.count("wan_port_start") && vm.count("wan_port_stop")){msg.append("wan_port_stop , mais wan_port_start manquant. ");test=false;}
            if (!vm.count("wan_port_start")){wan_port_start = lan_port;}
            if (!vm.count("wan_port_stop")){wan_port_stop = wan_port_start;}
            if(!test){
                msg.append("\n La commande devrait avoir la forme : \n --open_port --lan_port 80 --lan_ip 192.168.0.1 (--wan_port_start 79) (--wan_port_stop 81) (--source_ip 0.0.0.0) (--protocol tcp/udp) (--comment commentaire)");
                cout<<msg<<endl;
                return 0;}
            open_port=true;
        }
        else if (vm.count("close_port")){
            bool test=true;
            string msg="";
            if ( (vm.count("lan_port") && vm.count("lan_ip")) && vm.count("wan_port")){
                msg.append("Trop d'arguments. ");
                test=false;
            }
            else if((vm.count("lan_port") || vm.count("lan_ip")) && vm.count("wan_port")){
                msg.append("Trop d'arguments. "); 
                test=false;
            }
            else if(((!vm.count("lan_port") && vm.count("lan_ip")) || (vm.count("lan_port") && !vm.count("lan_ip")))){
                msg.append("Pas assez d'arguments. ");
                test=false;
            }
            else if (!vm.count("wan_port")){
                msg.append("Pas assez d'arguments. ");
                test=false;
            }
            if(!test){
                msg.append("\nLa commande devrait avoir la forme : \n --close_port --lan_port 80 --lan_ip 192.168.0.1  or \n --close_port --wan_port 80 (supprime la plage deport)");
                cout<<msg<<endl;
                return 0;
            }
            close_port=true;
        }
        else if (vm.count("update_port")){
            bool test=true;
            string msg="";
            if ( vm.count("lan_port") && vm.count("lan_ip") && vm.count("wan_port")){
                msg.append("Trop d'arguments. " + vm.count("lan_port")); cout<<1 ;
                test=false;
            }
            else if((vm.count("lan_port") && vm.count("wan_port")) || (vm.count("lan_ip") && vm.count("wan_port"))){
                msg.append("Trop d'arguments. " + !vm.count("lan_port")); 
                test=false;
            }
            else if((!vm.count("lan_port") && vm.count("lan_ip")) || (vm.count("lan_port") && !vm.count("lan_ip"))){
                msg.append("Pas assez d'arguments. ");
                test=false;
            }
            else if (!vm.count("wan_port")){
                msg.append("Pas assez d'arguments. ");
                test=false;
            }
            else if(!vm.count("enabled")){
                msg.append("--enabled manquant. ");
                test=false;
            }
            if(!test){
                msg.append("\nLa commande devrait avoir la forme : \n --close_port --lan_port 80 --lan_ip 192.168.0.1  or \n --close_port --wan_port 80 (supprime la plage deport)");
                cout<<msg<<endl;
                return 0;
            }
            if(1){}
            update_port=true;
        }
        else if (vm.count("save_fwdlist")){
            save_fwdlist=true;
        }
        else if (vm.count("load_fwdlist")){
                load_fwdlist=true;
        }
        else if (vm.count("save_staticDHCP")){
            save_staticDHCP=true;
        }
        else if (vm.count("load_staticDHCP")){
            load_staticDHCP=true;
        }
        else if (vm.count("add_staticDHCP")){
            bool test = true;
            string msg="";
            if (!vm.count("lan_ip")){msg.append("lan_ip manquant. ");test=false;}
            if (!vm.count("mac")){msg.append("mac manquant. ");test=false;}
            if(!test){
                msg.append("\nLa commande devrait avoir la forme : \n --add_staticDHCP --lan_ip 192.168.0.1 --mac AA:AA:AA:AA");
                cout<<msg<<endl;
                return 0;
            }
            add_staticDHCP=true;
        }
        else if (vm.count("delete_staticDHCP")){
            bool test = true;
            string msg="";
            if (!vm.count("lan_ip") && !vm.count("mac")){msg.append("Pas assez d'arguments. ");test=false;}
            if(!test){
                msg.append("\nLa commande devrait avoir la forme : \n --delete_staticDHCP --lan_ip 192.168.0.1 or \n --delete_staticDHCP --mac AA:AA:AA:AA");
                cout<<msg<<endl;
                return 0;
            }
            delete_staticDHCP=true;
        }
        else if (vm.count("reboot")){
            reboot=true;
        }



        char start_session = app.startSession();
        if (start_session==0){
            if(open_port){
                app.addPortForward(true,comment,lan_port,wan_port_start,wan_port_stop,lan_ip,protocol,source_ip);
            }
            else if(close_port){
                if (lan_port > 0 && lan_ip !=""){
                    app.deletePortForward(lan_port, lan_ip);
                }else if (wan_port > 0){
                    app.deletePortForward(wan_port);
                }
            }
            else if(update_port){
                if (lan_port > 0 && lan_ip !=""){
                app.updatePortForward(lan_port,lan_ip,enabled);
                }
                else if (wan_port > 0){
                    app.updatePortForward(wan_port,enabled);
                }
            }
            else if(save_fwdlist){
                app.savePortForward(file);
            }
            else if(load_fwdlist){
                app.loadPortForward(file);
            }
            else if(save_staticDHCP){
                app.saveStaticDHCP(file);
            }
            else if(add_staticDHCP){

                app.addStaticDHCP(lan_ip, mac, comment);
            }
            else if(delete_staticDHCP){
                if(lan_ip!=""){
                    app.deleteStaticDHCP(lan_ip);
                }
                else{ //mac!=""
                    app.deleteStaticDHCP(mac);
                }
                
            }
            else if(reboot){
                app.reboot();
            }
            app.closeSession();
        }
        else{ //todo : retourner les erreurs

        }
    }
    
    return 0;
}
