# Compiler 
CC = gcc
CFLAGS = -Wall -g

# Fichiers du client humain
CLIENT_HUMAIN_SRC = GestionnaireClient/ClientHumain.c GestionnaireClient/GestionnaireClient.c mainClientHumain.c
CLIENT_HUMAIN_HEADERS = GestionnaireClient/GestionnaireClient.h GestionnaireClient/ClientHumain.h 
CLIENT_HUMAIN_OBJ = $(CLIENT_HUMAIN_SRC:.c=.o)

# Fichiers du client robot
CLIENT_ROBOT_SRC = GestionnaireClient/ClientRobot.c GestionnaireClient/GestionnaireClient.c mainClientRobot.c
CLIENT_ROBOT_HEADERS = GestionnaireClient/GestionnaireClient.h GestionnaireClient/ClientRobot.h 
CLIENT_ROBOT_OBJ = $(CLIENT_ROBOT_SRC:.c=.o)

# Fichiers du serveur
SERVEUR_SRC = GestionnaireServeur/Serveur.c GestionnaireServeur/jeu.c mainServeur.c
SERVEUR_HEADERS = GestionnaireServeur/Serveur.h GestionnaireServeur/jeu.h 
SERVEUR_OBJ = $(SERVEUR_SRC:.c=.o)

# Exécutables
CLIENT_HUMAIN_EXE = client
CLIENT_ROBOT_EXE = robot
SERVEUR_EXE = serveur

all: $(CLIENT_HUMAIN_EXE) $(CLIENT_ROBOT_EXE) $(SERVEUR_EXE)

# Compilation du client humain
$(CLIENT_HUMAIN_EXE): $(CLIENT_HUMAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation du client robot
$(CLIENT_ROBOT_EXE): $(CLIENT_ROBOT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation du serveur
$(SERVEUR_EXE): $(SERVEUR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Règle générique pour compiler tous les fichiers .c en .o, en tenant compte des dépendances avec les fichiers .h
%.o: %.c $(CLIENT_HUMAIN_HEADERS) $(CLIENT_ROBOT_HEADERS) $(SERVEUR_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers compilés 
clean:
	rm -f $(CLIENT_HUMAIN_EXE) $(CLIENT_ROBOT_EXE) $(SERVEUR_EXE) *.o