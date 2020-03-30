/*CREATE TABLE todos(
   ID VARCHAR(32) PRIMARY KEY NOT NULL,
   DESCRIPTION    TEXT    NOT NULL,
   COMPLETED      BOOLEAN DEFAULT FALSE
);

******************* Prise de notes du mardi 11 février à 16h45 :
--créer une base postregresql (local et en ligne)
--2 gits : un repo fronto et un backo
****fronto = deux formulaires ;
- login => retourne un JWT stocké dans le localStorage ou cookie
- création de compte
****backo : rest avec le reasonml
4 routes à faire :
*login
*register
*updatePrivilege = 2 routes
	*demandeAffectation (envoyer message dans fil rabbitnq, file lu par les admins => admin approuve ou pas les demandes)
	*validationAffectation par un administrateur UNIQUEMENT (gestion des rôles)
*checkToken (à travers la date et le hash avec la clé générant le token)
#Comment requêter en SQL de reasonml -> POSTGRESQL ?
Où héberger POSTGRESQL ? voir liens sur le pdf
#Les tables SQL :
User : pseudo,email,mdp,nom,prenom,idRole (rôle par défaut="NOUVEAU")
Role : idRole,nom (nouveau, étudiant, professeur, administrateur)
Persistance de la demande d'affectation ?
token : pseudo, email


*/


--Story 1 : En tant que « nouveau », je peux créer mon compte
CREATE TABLE users( -- table of users in the application, the token is here for the security and manage the connection on the front
   email                   TEXT PRIMARY KEY  NOT NULL,
   pseudo                  TEXT              NOT NULL,--Story 2 ""
   password                TEXT              NOT NULL,--Story 2 : En tant que « nouveau », si je dispose d’un compte, je peux me connecter
   name                    TEXT              ,
   surname                 TEXT              ,
   userRole                TEXT              NOT NULL,--énumération : nouveau, étudiant, professeur, administrateur
   token                   TEXT               --Story 5 : En tant que « utilisateur », je peux demander la validation de mon token. 
   --RG 1 : Le process d’authentification se base sur la mise en place de token JWT signés et expirant à leur date d’émission + 3 jours
   --RG 2 : Le secret JWT peut être partagé avec les backend le nécessitant tant qu’ils sont hébergé sur le même compte clever-cloud grâce à un add on « Configuration provider »
);
--Story 3 : En tant que « nouveau », je peux demander une élévation de privilège afin de devenir « étudiant » ou « professeur »
--RG 4 : Lorsqu’un nouveau compte est créé, un évènement est créé sur un channel RabbitMQ 
--RG 5 : Lorsqu’une demande d’élévation de privilège est réalisée, un évènement est créé sur un channel RabbitMQ.
--RG 6 : Lorsqu’une acceptation d’élévation de privilège est réalisée, un évènement est créé sur un channel RabbitMQ
--====>>>>>> A VOIR SI LA TABLE CI-DESSOUS est réellement UTILE car ça doit être géré via RabbitMQ ?
/*g. Broker RabbitMQ
Le broker de message est https://www.cloudamqp.com/ et sera mis en place par l’équipe projet « Projet 5 : Service d’échanges »
Donc la mise en place de RabbitMQ est hors de notre périmètre, 

TODO : à voir si on doit avoir la table ci-dessous pour la gestion des demandes ou non ?
*/
/*
CREATE TABLE assignmentRequest( --allows administrators to see request and to accept an assignment
   emailUserForAssignment       TEXT NOT NULL,
   roleRequest                  TEXT NOT NULL,
   dateOfRequest                TEXT NOT NULL, --https://www.sqlite.org/datatype3.html 2.2. Date and Time Datatype => TEXT as ISO8601 strings ("YYYY-MM-DD HH:MM:SS.SSS"). 
   accepted                     BOOLEAN DEFAULT FALSE, --Story 4 : En tant que « administrateur », je accepter ou refuser une élévation de privilège.
   FOREIGN KEY(emailUserForAssignment) REFERENCES users(email)
)*/