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

CREATE TABLE users( -- table of users in the application, the token is here for the security and manage the connection on the front
   email                   TEXT PRIMARY KEY  NOT NULL,
   pseudo                  TEXT              NOT NULL,
   password                TEXT              NOT NULL,
   name                    TEXT              NOT NULL,
   surname                 TEXT              NOT NULL,
   userRole                TEXT              NOT NULL,--énumération : nouveau, étudiant, professeur, administrateur
   token                   TEXT              NOT NULL
);

CREATE TABLE assignmentRequest( --allows administrators to see request and to accept an assignment
   emailUserForAssignment       TEXT NOT NULL,
   roleRequest                  TEXT NOT NULL,
   dateOfRequest                TEXT NOT NULL, --https://www.sqlite.org/datatype3.html 2.2. Date and Time Datatype => TEXT as ISO8601 strings ("YYYY-MM-DD HH:MM:SS.SSS"). 
   FOREIGN KEY(emailUserForAssignment) REFERENCES users(email)
);