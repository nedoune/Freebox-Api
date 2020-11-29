# **Freebox API**
 Le programme gère la récupération du token et la connexion à l'API FreeboxOS.

 Le programme s'utilise en ligne de commande. 

 Permet l'ouverture automatique d'un port de la freebox à partir d'un script.

 Permet la sauvegarde et la resturation des redirections de la Freebox.

---
## **Dépendances**
---
- libopenssl

- boost::program_options

- rapidjson (inclus dans le répertoire include)

## **Utilisation**
---
### **Ouverture de port** :

 - Paramètres disponibles :
    - --dest_ip : ip de la machine locale
    - --lan_port : port sur la machine locale  
    - --source_ip (optionnel) : par défaut 0.0.0.0
    - --wan_port_start (optionnel) : début de la plage de port redirigée
    - --wan_port_stop (optionnel) : fin de la plage de port redirigée
    - --protocol (optionnel) : protocol ip tcp/udp (défaut tcp)
    - --fwd_comment (optionnel) : commentaire de la redirection

 Si ils ne sont pas précisés, wan_port_start & wan_port_stop reprennent par défaut la valeur de lan port.
 Pour redirigé un port différent de lan_port vers lan_port : définir la valeur de wan_port_start (wan_port_stop prendra la même valeur).

- Commande minimale : `./freeboxapi --open_port  --lan_port 80 --dest_ip 192.168.0.32 `

### **Fermer un port** :
 Supprime la redirection dans FreeboxOS
- `./freeboxapi --close_port  --lan_port 80`

### **Sauvegarder les redirections**
 Sauvegarde toutes les redirections de la freebox, avec leur état (actif/inactif), dans un fichier au format JSON 
- `./freeboxapi --save_fwdlist savefile`

### **Restaurer les redirections**
 Restaure toutes les redirections de la freebox, avec leur état (actif/inactif), depuis un fichier au format JSON 
- `./freeboxapi --load_fwdlist savefile

## **TODO**
- Inclure DHCP (sauvegarde/restauration)
- Ajouter commande reboot
- Ajout d'une interface ncurses
----
---
### *Note*
J'ai créé ce programme pour répondre à un besoin personnel, à savoir l'ouverture fermeture de ports sur la Freebox en automatique.

J'ai ajouté la sauvegarde des redirections car je n'ai rien trouvé d'existant pour le faire (ce qui m'aurait été utile).