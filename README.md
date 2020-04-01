# YATA - Yet Another Todo Api

Sample of a REST/JSON API built with ReasonML and compiled to JS with Buckelscript

Run those commands to use it :

```sh
git clone git@github.com:oteku/yat

npm i

npm run db:create

npm run dev
```

Test it :

```sh
curl http://127.0.0.1:8080

curl -X POST -H "Content-Type: application/json" -d "{ 
        \"email\": \"thomas@test.com\",
        \"pseudo\": \"Gotyge\",
        \"password\": \"momdp\",
        \"name\": \"AUGUET\",
        \"surname\": \"Thomas\",
        \"userRole\": \"Administrateur\",
        \"token\": \"montoken\" 
      }" http://127.0.0.1:8080/users
      
curl -X POST -H "Content-Type: application/json" -d "{ 
        \"email\": \"chris@test.com\",
        \"pseudo\": \"Wello\",
        \"password\": \"momdp\",
        \"name\": \"DUCROCQ\",
        \"surname\": \"Christopher\",
        \"userRole\": \"Etudiant\",
        \"token\": \"sontoken\" 
      }" http://127.0.0.1:8080/users
      
# replace :id by a real id
curl -X PUT -H "Content-Type: application/json" -d "{ 
        \"pseudo\": \"GotygeModifie\",
        \"password\": \"momdp\",
        \"name\": \"AUGUET\",
        \"surname\": \"Thomas\",
        \"userRole\": \"Administrateur\",
        \"token\": \"montoken\" 
      }" http://127.0.0.1:8080/users/thomas@test.com
      
curl http://127.0.0.1:8080/users
curl http://127.0.0.1:8080/users/thomas@test.com