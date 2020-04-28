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

  let create = (email, pseudo, password, name, surname) => {
    let userPromise = Password.Promise.deriveKey(algo, password)
    |> Js.Promise.then_(result =>
      switch (result) {
      | Belt.Result.Error(e) => raise(e)
      | Belt.Result.Ok((_, hash)) =>  Model.User.make(email, pseudo, hash, name, surname, "Nouveau");
      }
      |> Js.Promise.resolve
    );

    Js.Promise.(
      userPromise
      |> then_(user => {
      let insert = Knex.rawBinding(
        "INSERT INTO users VALUES (?,?,?,?,?,?)",
        (
         Model.User.getEmail(user),
         Model.User.getPseudo(user),
         Model.User.getPassword(user),
         Model.User.getName(user),
         Model.User.getSurname(user),
         Model.User.getUserRole(user),
        ), knex
      ) |> Knex.toPromise
        Js.Promise.all([|user |> resolve, insert|])
      }
      )
      |> then_( promises => {
        AmqpSender.sendNewUser(Model.User.toJsonWithoutPassword(promises[0])) |> ignore
        resolve()
      })
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
                  let options = Some({ ...JsonWebToken.emptyOptions, algorithm: HS256, expiresIn: "3 days"});
                  let myUser = users |> List.hd |> Model.User.toJsonWithoutPassword;
                  let jwt = JsonWebToken.sign(~secret=`string("issou"), ~options, `json(myUser));
                  Json_encode.object_([("token", Json_encode.string(jwt))]) |> resolve
                  | false => raise(Not_found)
                }
            }
          ) 
      })
      );
  };


  let verify = token => {
    let bool = JsonWebToken.verify(token,`string("issou")) |> Belt.Result.isOk
    Js.Promise.(
      Json_encode.bool(bool) |> resolve
    );
  };

};

module AssignmentRequest = {
  let create = (emailUserForAssignment,roleRequest) => {
    let assignmentrequest = Model.AssignmentRequest.makeNew(emailUserForAssignment, roleRequest, false, false);
    Js.Promise.(
      knex
      |> Knex.rawBinding(
           "INSERT INTO assignmentrequest
           (assignmentRequestId,emailUserForAssignment,roleRequest,decision,processed)
            VALUES (?,?,?,?,?)",
           (
             Model.AssignmentRequest.getAssignmentRequestId(assignmentrequest),
             Model.AssignmentRequest.getEmailUserForAssignment(assignmentrequest),
             Model.AssignmentRequest.getRoleRequest(assignmentrequest),
             Model.AssignmentRequest.getDecision(assignmentrequest),
             Model.AssignmentRequest.getProcessed(assignmentrequest)
           ),
         )
      |> Knex.toPromise
      |> then_(_ => {resolve()})
    );
  };


  let getAllWithProcessed = (processed) =>
    Js.Promise.(
      knex
      |> Knex.fromTable("assignmentrequest")
      |> Knex.where({"processed": processed})
      |> Knex.toPromise
      |> then_(results => {
          Model.AssignmentRequests.fromJson(results)
           |> List.map(assiR => {
                Model.AssignmentRequest.make(
                  Model.AssignmentRequest.getAssignmentRequestId(assiR),
                  Model.AssignmentRequest.getEmailUserForAssignment(assiR),
                  Model.AssignmentRequest.getRoleRequest(assiR),
                  Model.AssignmentRequest.getDecision(assiR),
                  Model.AssignmentRequest.getProcessed(assiR)
                )
              })
           |> Model.AssignmentRequests.toJson
           |> resolve
        })
  );

  let getAll = () =>
  Js.Promise.(
    knex
    |> Knex.fromTable("assignmentrequest")
    |> Knex.toPromise
    |> then_(results => {
        Model.AssignmentRequests.fromJson(results)
         |> List.map(assiR => {
              Model.AssignmentRequest.make(
                Model.AssignmentRequest.getAssignmentRequestId(assiR),
                Model.AssignmentRequest.getEmailUserForAssignment(assiR),
                Model.AssignmentRequest.getRoleRequest(assiR),
                Model.AssignmentRequest.getDecision(assiR),
                Model.AssignmentRequest.getProcessed(assiR)
              )
            })
         |> Model.AssignmentRequests.toJson
         |> resolve
      })
);

let getByUuid: string => Js.Promise.t(Js.Json.t) =
    uuid =>
      Js.Promise.(
        knex
        |> Knex.fromTable("assignmentrequest")
        |> Knex.where({"assignmentRequestId": uuid})
        |> Knex.toPromise
        |> then_(result => {
          Model.AssignmentRequests.fromJson(result)
          |> List.hd
          |> Model.AssignmentRequest.toJson
          |> resolve
          })
      );

let update = (assignmentRequestId, decision, processed) => {
  let assign = Model.AssignmentRequest.make(assignmentRequestId,"","",decision,processed)
  Js.Promise.(
    knex
    |> Knex.rawBinding(
         "UPDATE assignmentrequest SET 
         decision=?, 
         processed=?
         WHERE assignmentRequestId=? AND processed == false",
         (
          Model.AssignmentRequest.getDecision(assign),
          Model.AssignmentRequest.getProcessed(assign),
          Model.AssignmentRequest.getAssignmentRequestId(assign)
         ),
       )
    |> Knex.toPromise
    |> then_(_ => {resolve()})
  );
};

let accept = (uuid, decision) => {
  Js.Promise.(
    uuid |> getByUuid
    |> then_(assign => {
      let assign = assign |> Model.AssignmentRequest.fromJsonWithBoolean
      let id = assign |> Model.AssignmentRequest.getAssignmentRequestId
      if(decision){
          assign |> Model.AssignmentRequest.getEmailUserForAssignment |> Users.getByEmail
          |> then_(user => {
              let user = user |> Model.Users.fromJson |> List.hd
              Users.update(
                Model.User.getEmail(user),
                Model.User.getPseudo(user),
                Model.User.getPassword(user),
                Model.User.getName(user),
                Model.User.getSurname(user),
                Model.AssignmentRequest.getRoleRequest(assign)
              )
          }) |> ignore
      }
      update(id, decision, true)
    })
  );
}

};