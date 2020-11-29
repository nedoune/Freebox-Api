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
        string dest_ip,source_ip,protocol,fwd_comment;
        string file;
        int lan_port,wan_port_start,wan_port_stop;
        bool open_port=false;
        bool close_port=false;
        bool save_fwdlist=false;
        bool load_fwdlist=false;
        options_description desc;
        desc.add_options()
            ("help,h", "Help screen")
            ("open_port","crée une redirection")
            //("update_port","modifie une redirection existante")
            ("close_port","supprime une redirection")
            ("lan_port", boost::program_options::value<int>(&lan_port), "port local")
            ("dest_ip", boost::program_options::value<string>(&dest_ip), "ip de la machine locale")
            ("wan_port_start", boost::program_options::value<int>(&wan_port_start), "debut de plage de port redirigée")
            ("wan_port_stop", boost::program_options::value<int>(&wan_port_stop), "fin de plage de port redirigée")
            ("source_ip", boost::program_options::value<string>(&source_ip)->default_value("0.0.0.0"), "ip source")
            ("protocol", boost::program_options::value<string>(&protocol)->default_value("tcp"), "protocol tcp / udp")
            ("fwd_comment", boost::program_options::value<string>(&fwd_comment)->default_value(""), "commentaire de la redirection")
            ("save_fwdlist", boost::program_options::value<string>(&file), "sauvegarde toutes les redirections, avec leur état, dans un fichier Json")
            ("load_fwdlist", boost::program_options::value<string>(&file), "restaure les redirections depuis un fichier Json");
        boost::program_options::variables_map vm;
        boost::program_options::store(parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help")){
            std::cout << desc << '\n';
            return 0;
        }
        else if (vm.count("open_port")){
            bool test = true;
            if (!vm.count("lan_port")){cout<<"lan_port non specified"<<endl;test=false;}
            if (!vm.count("dest_ip")){cout<<"dest_ip non specified"<<endl;test=false;}
            if (!vm.count("wan_port_start") and vm.count("wan_port_stop")){cout<<"wan_port_stop specified but wan_port__start not specified"<<endl;test=false;}
            if (!vm.count("wan_port_start")){wan_port_start = lan_port;}
            if (!vm.count("wan_port_stop")){wan_port_stop = wan_port_start;}
            if(!test){return 0;}
            open_port=true;
        }
        else if (vm.count("close_port")){
            bool test=true;
            if (!vm.count("lan_port")){cout<<"lan_port non specified"<<endl;test=false;}
            if(!test){return 0;}
            close_port=true;
        }
        else if (vm.count("save_fwdlist")){
            save_fwdlist=true;
        }
        else if (vm.count("load_fwdlist")){
                load_fwdlist=true;
        }



        char start_session = app.startSession();
        if (start_session==0){
            if(open_port){
                app.addPortForward(true,fwd_comment,lan_port,wan_port_start,wan_port_stop,dest_ip,protocol,source_ip);
            }
            else if(close_port){
                app.deletePortForward(lan_port);
                }
            else if(save_fwdlist){
                app.savePortForward(file);
            }
            else if(load_fwdlist){
                app.loadPortForward(file);
            }
            app.closeSession();
        }
        else{ //todo : retourner les erreurs

        }
    }
    
    return 0;
}
