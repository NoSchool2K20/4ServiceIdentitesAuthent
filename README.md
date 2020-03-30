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

curl http://127.0.0.1:8080/todos
curl http://127.0.0.1:8080/todos?completed=true
curl http://127.0.0.1:8080/todos?completed=false

curl -X POST -H "Content-Type: application/json" -d "{ \"MESSAGE\": \"value1\" }" http://127.0.0.1:8080/todos

# replace :id by a real id
curl -X PUT -H "Content-Type: application/json" -d "{ \"MESSAGE\": \"value1\", \"COMPLETED\": true }" http://127.0.0.1:8080/todos/:id
```
