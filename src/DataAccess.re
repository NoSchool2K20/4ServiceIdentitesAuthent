/*
 ** knex is an excellent query manager in JS http://knexjs.org/
 ** bs-knex is unmaintaned and incomplete, but existing binding are good to manage connection
 ** We can use rawBinding query which is a safe raw query instead of any unexisting bindings
 ** While SQLite is not really suitable for production this part is more a POC to facilitate the conception of an API.
 */


let connection =
  KnexConfig.Connection.make(~filename="./database/yatdb.sqlite", ());

let config =
  KnexConfig.make(
    ~client="sqlite3",
    ~connection,
    ~acquireConnectionTimeout=2000,
    (),
  );

let knex = Knex.make(config);

let algo = Password.Algorithm.Bcrypt;

module Users = {
  let getAll = () =>
    Js.Promise.(
      knex
      |> Knex.fromTable("users")
      |> Knex.toPromise
      |> then_(results => {
           Model.Users.fromJson(results)
           |> List.map(user => {//  let make = (email, pseudo, password, name, surname, userRole) => {email, pseudo, password, name, surname, userRole};
                Model.User.make(
                  Model.User.getEmail(user),
                  Model.User.getPseudo(user),
                  Model.User.getPassword(user),
                  Model.User.getName(user),
                  Model.User.getSurname(user),
                  Model.User.getUserRole(user),
                )
              })
           |> Model.Users.toJson
           |> resolve
         })
    );

  let getByUserRole = userRole =>
    Js.Promise.(
      knex
      |> Knex.fromTable("users")
      |> Knex.where({"userRole": userRole})
      |> Knex.toPromise
      |> then_(results => {
           Model.Users.fromJson(results)
           |> List.map(user => {
                Model.User.make(
                  Model.User.getEmail(user),
                  Model.User.getPseudo(user),
                  Model.User.getPassword(user),
                  Model.User.getName(user),
                  Model.User.getSurname(user),
                  Model.User.getUserRole(user),
                )
              })
           |> Model.Users.toJson
           |> resolve
         })
    );


  let getByEmail: string => Js.Promise.t(Js.Json.t) =
    email =>
      Js.Promise.(
        knex
        |> Knex.fromTable("users")
        |> Knex.where({"email": email})
        |> Knex.toPromise
        |> then_(results => {
             Model.Users.fromJson(results)
             |> List.map(user => {
                  Model.User.make(
                    Model.User.getEmail(user),
                    Model.User.getPseudo(user),
                    Model.User.getPassword(user),
                    Model.User.getName(user),
                    Model.User.getSurname(user),
                    Model.User.getUserRole(user),
                  )
                })
             |> Model.Users.toJson
             |> resolve
           })
      );
   //email, pseudo, password, name, surname, userRole, token
  let update = (email, pseudo, password, name, surname, userRole) => {
    let user = Model.User.make(email, pseudo, password, name, surname, userRole);
    Js.Promise.(
      knex
      |> Knex.rawBinding(
           "UPDATE users SET 
           pseudo=?, 
           password=?, 
           name=?, 
           surname=?,
           userRole=?
           WHERE email=?",
           (
            Model.User.getPseudo(user),
            Model.User.getPassword(user),
            Model.User.getName(user),
            Model.User.getSurname(user),
            Model.User.getUserRole(user),
            Model.User.getEmail(user),
           ),
         )
      |> Knex.toPromise
      |> then_(_ => {resolve()})
    );
  };

  let create = (email, pseudo, password, name, surname, userRole) => {  
    let userPromise = Password.Promise.deriveKey(algo, password)
    |> Js.Promise.then_(result =>
      switch (result) {
      | Belt.Result.Error(e) => raise(e)
      | Belt.Result.Ok((_, hash)) =>  Model.User.make(email, pseudo, hash, name, surname, userRole);
      }
      |> Js.Promise.resolve
    );

    Js.Promise.(
      userPromise
      |> then_(user => 
      Knex.rawBinding(
           "INSERT INTO users VALUES (?,?,?,?,?,?)",
           (
            Model.User.getEmail(user),
            Model.User.getPseudo(user),
            Model.User.getPassword(user),
            Model.User.getName(user),
            Model.User.getSurname(user),
            Model.User.getUserRole(user),
           ), knex
         )
      |> Knex.toPromise
      )
      |> then_(_ => {resolve()})
      );
  };

  let connection = (email, password) => {
    Js.Promise.(
      getByEmail(email)
      |> then_(user => {
        let users = user |> Model.Users.fromJson;
        if(users |> List.length == 0){
          raise(Not_found)
        }
        let hash = users |> List.hd |> Model.User.getPassword;
        Password.Promise.verify(algo, hash, password)
          |> then_(isValid =>
            switch(isValid){
              | Belt.Result.Error(e) => raise(e)
              | Belt.Result.Ok(bool) => 
                switch(bool){
                  | true => 
                  let options = Some({ ...JsonWebToken.emptyOptions, algorithm: HS256, notBefore: "1 days"});
                  let myUser = users |> List.hd;
                  let payload = [
                    ("email", Json_encode.string(Model.User.getEmail(myUser))),
                    ("pseudo", Json_encode.string(Model.User.getPseudo(myUser))),
                    ("name", Json_encode.string(Model.User.getName(myUser))),
                    ("surname", Json_encode.string(Model.User.getSurname(myUser))),
                    ("userRole", Json_encode.string(Model.User.getUserRole(myUser))),
                  ]
                  let jwt = JsonWebToken.sign(~secret=`string("issou"), ~options, `json(Json_encode.object_(payload)));
                  jwt |> resolve
                  | false => raise(Not_found)
                }
            }
          ) 
      })
      );
  };
};