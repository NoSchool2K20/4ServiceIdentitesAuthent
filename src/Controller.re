open Express;

module Users = {
  let getAll =
    PromiseMiddleware.from((_next, req, rep) => {
      let queryDict = Request.query(req);
      (
        switch (queryDict->Js.Dict.get("userRole")) {//TODO voir comment adapter ?
        | Some(c) =>
          switch (c |> Json.Decode.string |> bool_of_string_opt) {
          | Some(cfilter) => DataAccess.Users.getByUserRole(cfilter)
          | None => DataAccess.Users.getAll()
          }
        | None => DataAccess.Users.getAll()
        }
      )
      |> Js.Promise.(
           then_(todoJson => {
             rep
             |> Response.setHeader("Status", "200")
             |> Response.sendJson(todoJson)
             |> resolve
           })
         );
    });

  let get =
    PromiseMiddleware.from((_next, req, rep) =>
      switch (Request.params(req)->Js.Dict.get("email")) {
      | None => rep |> Response.sendStatus(BadRequest) |> Js.Promise.resolve
      | Some(email) =>
        email
        |> Json.Decode.string
        |> DataAccess.Users.getByEmail
        |> Js.Promise.(
             then_(todoJson => {
               rep
               |> Response.setHeader("Status", "200")
               |> Response.sendJson(todoJson)
               |> resolve
             })
           )
      }
    );

    
    
  let update =
    PromiseMiddleware.from((_next, req, rep) =>
      Js.Promise.(
        (
          switch (Request.params(req)->Js.Dict.get("email")) {
          | None => reject(Failure("INVALID email"))
          | Some(email) =>
            switch (Request.bodyJSON(req)) {
            | None => reject(Failure("INVALID email"))
            | Some(reqJson) =>
              switch (
                reqJson |> Json.Decode.(field("pseudo", optional(string))),
                reqJson |> Json.Decode.(field("password", optional(string))),
                reqJson |> Json.Decode.(field("name", optional(string))),
                reqJson |> Json.Decode.(field("surname", optional(string))),
                reqJson |> Json.Decode.(field("userRole", optional(string))),
              ) {   //email, pseudo, password, name, surname, userRole
              | exception e => reject(e)
              | (Some(pseudo), Some(password), Some(name), Some(surname), Some(userRole)) =>
                DataAccess.Users.update(
                  Json.Decode.string(email),
                  pseudo,
                  password,
                  name,
                  surname,
                  userRole,
                )
              | _ => reject(Failure("INVALID email"))
              }
            }
          }
        )
        |> then_(() => {
             rep
             |> Response.setHeader("Status", "201")
             |> Response.sendJson(
                  Json.Encode.(object_([("text", string("Updated user"))])),
                )
             |> resolve
           })
        |> catch(err => {
             // Sadly no way to get Js.Promise.error message in a safe way
             Js.log(err);

             rep
             |> Response.setHeader("Status", "400")
             |> Response.sendJson(
                  Json.Encode.(
                    object_([
                      (
                        "error",
                        string("INVALID REQUEST OR MISSING email FIELD"),
                      ),
                    ])
                  ),
                )
             |> resolve;
           })
      )
    );

  let connection =
  PromiseMiddleware.from((_next, req, rep) =>
    Js.Promise.(
      (
        switch (Request.bodyJSON(req)) {
        | None => reject(Failure("INVALID REQUEST"))
        | Some(reqJson) =>
          switch (
            reqJson |> Json.Decode.(field("email", optional(string))),
            reqJson |> Json.Decode.(field("password", optional(string))),
          ) {
          | exception e => reject(e)
          | (Some(email),Some(password)) => 
              DataAccess.Users.connection(email, password)
          | _ => reject(Failure("Manque de données"))
          }
        }
      )
      |> then_(jwt => {
          rep
          |> Response.setHeader("Status", "200")
          |> Response.sendJson(jwt)
          |> resolve
        })
      |> catch(err => {
          // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
          Js.log(err);
          rep
          |> Response.setHeader("Status", "400")
          |> Response.sendJson(
                Json.Encode.(
                  object_([
                    (
                      "error",
                      string("Connexion échouée"),
                    ),
                  ])
                ),
              )
          |> resolve;
        })
      )
    );

    let verify =
  PromiseMiddleware.from((_next, req, rep) =>
    Js.Promise.(
      (
        switch (Request.bodyJSON(req)) {
        | None => reject(Failure("INVALID REQUEST"))
        | Some(reqJson) =>
          switch (
            reqJson |> Json.Decode.(field("token", optional(string))),
          ) {
          | exception e => reject(e)
          | (Some(token)) => 
              DataAccess.Users.verify(token)
          | _ => reject(Failure("Manque de données"))
          }
        }
      )
      |> then_(isValid => {
          rep
          |> Response.setHeader("Status", "200")
          |> Response.sendJson(isValid)
          |> resolve
        })
      |> catch(err => {
          // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
          Js.log(err);
          rep
          |> Response.setHeader("Status", "400")
          |> Response.sendJson(
                Json.Encode.(
                  object_([
                    (
                      "error",
                      string("Connexion échouée"),
                    ),
                  ])
                ),
              )
          |> resolve;
        })
      )
    );
  
  let create =
  PromiseMiddleware.from((_next, req, rep) =>
    Js.Promise.(
      (
        switch (Request.bodyJSON(req)) {
        | None => reject(Failure("INVALID REQUEST"))
        | Some(reqJson) =>
          switch (
            reqJson |> Json.Decode.(field("email", optional(string))),
            reqJson |> Json.Decode.(field("pseudo", optional(string))),
            reqJson |> Json.Decode.(field("password", optional(string))),
            reqJson |> Json.Decode.(field("name", optional(string))),
            reqJson |> Json.Decode.(field("surname", optional(string))),
            reqJson |> Json.Decode.(field("userRole", optional(string))),
          ) {
          | exception e => reject(e)
          | (Some(email),Some(pseudo), Some(password), Some(name), Some(surname), Some(userRole)) => 
              DataAccess.Users.create(email, pseudo, password, name, surname, userRole)
          | _ => reject(Failure("Manque de données"))
          }
        }
      )
      |> then_(() => {
           rep
           |> Response.setHeader("Status", "201")
           |> Response.sendJson(
                Json.Encode.(object_([("text", string("Created user"))])),
              )
           |> resolve
         })
      |> catch(err => {
           // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
           Js.log(err);
           rep
           |> Response.setHeader("Status", "400")
           |> Response.sendJson(
                Json.Encode.(
                  object_([
                    (
                      "error",
                      string("INVALID REQUEST OR MISSING email FIELD"),
                    ),
                  ])
                ),
              )
           |> resolve;
         })
    )
  );
};

let welcome =
  Middleware.from((_next, _req) => {
    Json.Encode.(
      object_([("text", string("Welcome to Yet Antother Todo api"))])
    )
    |> Response.sendJson
  });

let logRequest =
  Middleware.from((next, req) => {
    Request.ip(req)
    ++ " "
    ++ Request.methodRaw(req)
    ++ " ressource "
    ++ Request.path(req)
    |> Logger.info;
    next(Next.middleware);
  });

let badRessource =
  Middleware.from((_next, _req, rep) =>
    rep |> Response.sendStatus(NotFound)
  );