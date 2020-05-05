# service des identités et des authentifications 

REST/JSON API built with ReasonML and compiled to JS with Buckelscript

## Run those commands to use it :

```sh
git clone git@github.com:NoSchool2K20/4ServiceIdentitesAuthent.git

npm i

npm run db:create

npm run dev
```

## User stories :

```sh
Story 1 : En tant que « nouveau », je peux créer mon compte
Story 2 : En tant que « nouveau », si je dispose d’un compte, je peux me connecter
Story 3 : En tant que « nouveau », je peux demander une élévation de privilège afin de devenir « étudiant » ou
« professeur »
Story 4 : En tant que « administrateur », je accepter ou refuser une élévation de privilège.
Story 5 : En tant que « utilisateur », je peux demander la validation de mon token. 
```

## Règles de gestion :

```sh
RG 1 : Le process d’authentification se base sur la mise en place de token JWT signés et expirant à leur date
d’émission + 3 jours
RG 2 : Le secret JWT peut être partagé avec les backend le nécessitant tant qu’ils sont hébergé sur le même
compte clever-cloud grâce à un add on « Configuration provider »
RG 3 : Un composant ReasonReact devra être fournit au projet Front-End
RG 4 : Lorsqu’un nouveau compte est créé, un évènement est créé sur un channel RabbitMQ
RG 5 : Lorsqu’une demande d’élévation de privilège est réalisée, un évènement est créé sur un channel
RabbitMQ
RG 6 : Lorsqu’une acceptation d’élévation de privilège est réalisée, un évènement est créé sur un channel
RabbitMQ 
```

## Database :

2 tables : users and assignmentrequest  

1 compte admin par défaut (créé dans le fichier `scripts/create_db.sql`) avec en tant qu'identifiant de connexion:
- email : admin@test.com
- username: admin  
- password: admin

```
CREATE TABLE users(
   email                   TEXT PRIMARY KEY  NOT NULL,
   pseudo                  TEXT              NOT NULL,
   password                TEXT              NOT NULL,
   name                    TEXT              ,
   surname                 TEXT              ,
   userRole                TEXT              NOT NULL -- Nouveau, Etudiant, Professeur, Administrateur
);

CREATE TABLE assignmentrequest(
   assignmentRequestId          TEXT PRIMARY KEY,
   emailUserForAssignment       TEXT NOT NULL,
   roleRequest                  TEXT NOT NULL,
   decision                     BOOLEAN DEFAULT NULL,
   processed                    BOOLEAN DEFAULT FALSE,
   FOREIGN KEY(emailUserForAssignment) REFERENCES users(email)
);
```

## Routes :

You can find a json collection in `tests/integrations/NoSchool test.postman_collection.json` to help you !

### Create an user

POST - http://127.0.0.1:8080/users

Request body : 
```
{ 
  "email": "toood13@test.com",
  "pseudo": "toood13",
  "password": "toood",
  "name": "AUGUET",
  "surname": "Thomas"
}
```
UserRole is "Nouveau" by default and password will be hash in database

### Connexion

POST - http://127.0.0.1:8080/connection

Request body : 
```
{ 
  "email": "toood13@test.com",
  "password": "toood"
}
```
Return the jwt of the user with user's object without password in payload

### Verify a token validity

POST - http://127.0.0.1:8080/verify

Request body : 
```
{ 
  "token": "jwt_token"
}
```
Return true if jwt is valid otherwise false

### Create an assignmentRequest

POST - http://127.0.0.1:8080/assignmentRequest

Request body :
``` 
{ 
  "roleRequest": "Professeur"
}
```
This request needs a jwt token in header  
The user in the jwt payload needs to have **"Nouveau"** as role   
roleRequest needs to be **"Professeur"** or **"Etudiant"**

### Get all assignmentRequest

GET - http://127.0.0.1:8080/assignmentRequest

This request needs a jwt token in header  
The user in the jwt payload needs to have **"Administrateur"** as role  

### Get all assignmentRequest processed

GET - http://127.0.0.1:8080/assignmentRequest/processed

This request needs a jwt token in header  
The user in the jwt payload needs to have **"Administrateur"** as role 

### Get all assignmentRequest not processed

GET - http://127.0.0.1:8080/assignmentRequest/notprocessed

This request needs a jwt token in header  
The user in the jwt payload needs to have **"Administrateur"** as role 

### Accept an assignmentRequest

POST - http://127.0.0.1:8080/assignmentRequest/:assignmentRequestId/accept

This request needs a jwt token in header  
The user in the jwt payload needs to have **"Administrateur"** as role 

### Decline an assignmentRequest

POST - http://127.0.0.1:8080/assignmentRequest/:assignmentRequestId/decline

This request needs a jwt token in header  
The user in the jwt payload needs to have **"Administrateur"** as role 