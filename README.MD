# Freebox API
Le programme gère la récupération du token et la connexion à l'API FreeboxOS.
 
 
## Dépendances
- libopenssl

- boost::program_options

- rapidjson (inclus dans le répertoire include)

## Utilisation
### Ouverture de port :

- Paramètres disponibles :
    - --dest_ip : ip de la machine locale
    - --lan_port : port sur la machine locale  
    - --source_ip (optionnel) : par défaut 0.0.0.0
    - --wan_port_start (optionnel) : début de la plage de port redirigée
    - --wan_port_stop (optionnel) : fin de la plage de port redirigée
    - --protocol (optionnel) : protocol ip tcp/udp (défaut tcp)
    - --fwd_comment (optionnel) : commentaire de la redirection

wan_port_start & wan_port_stop reprennent par défaut la valeur de lan port.
Pour redirigé un port différent de lan_port vers lan_port : définir la valeur de wan_port_start (wan_port_stop prendra la même valeur).


- Commande minimale : `./freeboxapi --open_port  --lan_port 80 --dest_ip 192.168.0.32 `

### Fermer un port :
##### Supprime la redirection dans FreeboxOS
- `./freeboxapi --close_port  --lan_port 80`

### Sauvegarder les redirections
##### Sauvegarde toutes les redirections de la freebox, avec leur état (actif/inactif), dans un fichier au format JSON 
- `./freeboxapi --save_fwdlist savefile

### Restaurer les redirections
##### Restaure toutes les redirections de la freebox, avec leur état (actif/inactif), depuis un fichier au format JSON 
- `./freeboxapi --load_fwdlist savefile
