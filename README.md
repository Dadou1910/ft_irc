# ft_irc

## 📌 Description

`ft_irc` est une implémentation d'un serveur IRC (Internet Relay Chat) conforme au protocole RFC 1459.  
Ce projet, réalisé dans le cadre du cursus 42, vise à comprendre les bases de la communication réseau,  
la gestion des sockets, et l'interprétation des commandes selon un protocole client/serveur.

Le serveur accepte plusieurs connexions simultanées de clients, traite les commandes IRC de base  
et permet aux utilisateurs de communiquer via des canaux.

---

## 🧠 Compétences acquises

- Programmation réseau en C++
- Gestion de multiples connexions avec `poll()`
- Manipulation fine des sockets TCP/IP
- Respect du protocole IRC (RFC 1459)
- Gestion d’un projet structuré en équipe

---

## 🚀 Installation

```bash
git clone https://github.com/Dadou1910/ft_irc.git
cd ft_irc
make
```

---

## 🛠️ Utilisation

Lancer le serveur IRC :
./ircserv <port> <password>

    <port> : Port d'écoute du serveur (ex : 6667)

    <password> : Mot de passe que les clients doivent fournir à la connexion via la commande PASS

Exemple :
./ircserv 6667 secretpass

Pour se connecter, vous pouvez utiliser un client IRC comme irssi, HexChat, ou netcat pour tester manuellement.
📚 Commandes IRC implémentées

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

---

## 📁 Structure du projet
``` bash
ft_irc/
├── cmds/      # Commandes disponibles
├── .cpp/.hpp
├── Makefile
└── README.md
```
---

🧪 Tests

Utilisez des clients IRC comme Irssi ou netcat pour tester la conformité du serveur
et simuler des échanges entre utilisateurs.
---

## 🤝 Auteurs

Projet réalisé par :

    👤 @Dadou1910
    👤 @ByuyC

📄 Licence

Projet réalisé dans le cadre du cursus 42, à but pédagogique.
Toute réutilisation du code est soumise aux règles de l'école.


