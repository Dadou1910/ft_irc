ft_irc

📌 Description

ft_irc est une implémentation d'un serveur IRC (Internet Relay Chat) conforme au protocole RFC 1459. Ce projet, réalisé dans le cadre du cursus 42, vise à comprendre les bases de la communication réseau, la gestion des sockets, et l'interprétation des commandes selon un protocole client/serveur.

Le serveur accepte plusieurs connexions simultanées de clients, traite les commandes IRC de base et permet aux utilisateurs de communiquer via des canaux.

🧠 Compétences acquises

Programmation réseau en C++

Gestion de multiples connexions avec poll()

Manipulation fine des sockets TCP/IP

Respect du protocole IRC (RFC 1459)

Gestion d’un projet structuré en équipe

🚀 Installation

git clone https://github.com/Dadou1910/ft_irc.git
cd ft_irc
make

🛠️ Utilisation

Lancer le serveur IRC :

./ircserv <port> <password>

<port> : Port d'écoute du serveur (ex : 6667)

<password> : Mot de passe que les clients doivent fournir à la connexion via la commande PASS.

Exemple :

./ircserv 6667 42network

Pour se connecter, vous pouvez utiliser un client IRC comme irc ou irssi, ou netcat pour des tests manuels.

📚 Commandes IRC implémentées

Voici la liste minimale des commandes gérées par le serveur (selon les exigences du projet) :

PASS

NICK

USER

JOIN

PART

PRIVMSG

TOPIC

INVITE

KICK

MODE (i, t, k, o, l)

QUIT

📁 Structure du projet

ft_irc/
├── .cpp/.hpp      # Fichiers d'en-tête et code source
├── cmds/          # Commandes disponibles
├── Makefile
└── README.md

🧪 Tests

Vous pouvez tester la conformité du serveur avec un client IRC comme Irssi ou netcat. 

🤝 Auteurs

Projet réalisé par :

👤 @Dadou1910

👤 @BuyuC

📄 Licence

Projet réalisé dans le cadre du cursus 42, à but pédagogique. Toute réutilisation du code est soumise aux règles de l'école.
