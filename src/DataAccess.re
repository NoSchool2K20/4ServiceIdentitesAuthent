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

module Users = {
  let getAll = () =>
    Js.Promise.(
      knex
      |> Knex.fromTable("users")
      |> Knex.toPromise
      |> then_(results => {
           Model.Users.fromJson(results)
           |> List.map(user => {//  let make = (email, pseudo, password, name, surname, userRole, token) => {email, pseudo, password, name, surname, userRole, token};
                Model.User.make(
                  Model.User.getEmail(user),
                  Model.User.getPseudo(user),
                  Model.User.getPassword(user),
                  Model.User.getName(user),
                  Model.User.getSurname(user),
                  Model.User.getUserRole(user),
                  Model.User.getToken(user),
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
                  Model.User.getToken(user),
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
                    Model.User.getToken(user),
                  )
                })
             |> Model.Users.toJson
             |> resolve
           })
      );
   //email, pseudo, password, name, surname, userRole, token
  let update = (email, pseudo, password, name, surname, userRole, token) => {
    let user = Model.User.make(email, pseudo, password, name, surname, userRole, token);
    Js.Promise.(
      knex
      |> Knex.rawBinding(
           "UPDATE users SET 
           pseudo=?, 
           password=?, 
           name=?, 
           surname=?,
           userRole=?,
           token=?
           WHERE email=?",
           (
            Model.User.getPseudo(user),
            Model.User.getPassword(user),
            Model.User.getName(user),
            Model.User.getSurname(user),
            Model.User.getUserRole(user),
            Model.User.getToken(user),
            Model.User.getEmail(user),
           ),
         )
      |> Knex.toPromise
      |> then_(_ => {resolve()})
    );
  };

  let create = (email, pseudo, password, name, surname, userRole, token) => {  

    let test = Password.Promise.deriveKey(Password.Algorithm.Bcrypt, password)
    |> Js.Promise.then_(result =>
      switch (result) {
      | Belt.Result.Error(e) => raise(e)
      | Belt.Result.Ok((_, hash)) =>  Model.User.make(email, pseudo, hash, name, surname, userRole, token);
      }
      |> Js.Promise.resolve
    );

    Js.Promise.(
      test
      |> then_(user => 
      Knex.rawBinding(
           "INSERT INTO users VALUES (?,?,?,?,?,?,?)",
           (
            Model.User.getEmail(user),
            Model.User.getPseudo(user),
            Model.User.getPassword(user),
            Model.User.getName(user),
            Model.User.getSurname(user),
            Model.User.getUserRole(user),
            Model.User.getToken(user),
           ), knex
         )
      |> Knex.toPromise
      )
      |> then_(_ => {resolve()})
      );
  };
};