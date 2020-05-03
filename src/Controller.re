open Express;

module AssignmentRequest = {
  let getAll =
    PromiseMiddleware.from((_next, _, rep) => {
        DataAccess.AssignmentRequest.getAll()
      
      |> Js.Promise.(
           then_(requests => {
             rep
             |> Response.status(Ok)
             |> Response.sendJson(requests)
             |> resolve
           })
         );
  });

  let getAllWithProcessed = processed => {
  PromiseMiddleware.from((_next, _, rep) => {
      DataAccess.AssignmentRequest.getAllWithProcessed(processed)
    
    |> Js.Promise.(
         then_(requests => {
           rep
           |> Response.status(Ok)
           |> Response.sendJson(requests)
           |> resolve
         })
       );
});
  };
  
  let acceptOrDecline = choice => {
  PromiseMiddleware.from((_next, req, rep) => 
    Js.Promise.(
      (
        switch (Request.params(req)->Js.Dict.get("uuid")) {
        | None => reject(Failure("INVALID uuid"))
        | Some(uuid) => {
            switch(uuid |> Js.Json.decodeString){
              | Some(uuid) => DataAccess.AssignmentRequest.accept(uuid, choice)
              | None => reject(Failure("INVALID uuid"))
            }
          }
        }
      )
      |> then_(() => {
           rep
           |> Response.status(Created)
           |> Response.sendJson(
                Json.Encode.(object_([("text", string("Updated assignment"))])),
              )
           |> resolve
         })
      |> catch(err => {
           // Sadly no way to get Js.Promise.error message in a safe way
           Js.log(err);
           rep
           |> Response.status(BadRequest)
           |> Response.sendJson(
                Json.Encode.(
                  object_([
                    (
                      "error",
                      string("INVALID REQUEST OR BAD UUID"),
                    ),
                  ])
                ),
              )
           |> resolve;
         })
    )
  );
        };
  
    let create =
    PromiseMiddleware.from((_next, req, rep) =>
      Js.Promise.(
        (
          switch (Request.bodyJSON(req)) {
          | None => reject(Failure("INVALID REQUEST"))
          | Some(reqJson) =>
            switch (
              reqJson |> Json.Decode.(field("roleRequest", optional(string))),
            ) {
            | exception e => reject(e)
            | (Some(roleRequest)) => 
                switch (req |> Model.User.fromRequest){
                  | Some(user) => {
                    if(roleRequest == "Etudiant" || roleRequest == "Professeur"){
                      DataAccess.AssignmentRequest.create(user, roleRequest)
                    } else {
                      reject(Failure("INVALID ROLE ASKED"))
                    }
                  }
                  | None => reject(Failure("INVALID USER"))
                }
            | _ => reject(Failure("Manque de données"))
            }
          }
        )
        |> then_(() => {
             rep
             |> Response.status(Created)
             |> Response.sendJson(
                  Json.Encode.(object_([("text", string("Created AR"))])),
                )
             |> resolve
           })
        |> catch(err => {
             // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
             Js.log(err);
             rep
             |> Response.status(BadRequest)
             |> Response.sendJson(
                  Json.Encode.(
                    object_([
                      (
                        "error",
                        string("INVALID REQUEST OR MISSING FIELDS"),
                      ),
                    ])
                  ),
                )
             |> resolve;
           })
      )
    );

};

module Users = {
  let getAll =
    PromiseMiddleware.from((_next, req, rep) => {
      let queryDict = Request.query(req);
      (
        switch (queryDict->Js.Dict.get("userRole")) {
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
             |> Response.status(Ok)
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
               |> Response.status(Ok)
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
             |> Response.status(Created)
             |> Response.sendJson(
                  Json.Encode.(object_([("text", string("Updated user"))])),
                )
             |> resolve
           })
        |> catch(err => {
             // Sadly no way to get Js.Promise.error message in a safe way
             Js.log(err);

             rep
             |> Response.status(BadRequest)
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
          |> Response.status(Ok)
          |> Response.sendJson(jwt)
          |> resolve
        })
      |> catch(err => {
          // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
          Js.log(err);
          rep
          |> Response.status(BadRequest)
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
          |> Response.status(Ok)
          |> Response.sendJson(isValid)
          |> resolve
        })
      |> catch(err => {
          // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
          Js.log(err);
          rep
          |> Response.status(BadRequest)
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
          ) {
          | exception e => reject(e)
          | (Some(email),Some(pseudo), Some(password), Some(name), Some(surname)) => 
              DataAccess.Users.create(email, pseudo, password, name, surname)
          | _ => reject(Failure("Manque de données"))
          }
        }
      )
      |> then_(() => {
           rep
           |> Response.status(Created)
           |> Response.sendJson(
                Json.Encode.(object_([("text", string("Created user"))])),
              )
           |> resolve
         })
      |> catch(err => {
           // Sadly no way to get Js.Promise.error is an abstract type, we have no way to get its message in a safe way
           Js.log(err);
           rep
           |> Response.status(BadRequest)
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
      object_([("text", string("Welcome to Yet Another Api"))])
    )|> Response.sendJson
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


let authenticate =
Middleware.from((next, req, res) => {
  switch (Request.get("authorization", req)) {
  | Some(token) => {
    let tokenWithoutBear = Js.String.split(" ", token)[1];
    let bool = JsonWebToken.verify(tokenWithoutBear,`string("issou")) |> Belt.Result.isOk
    if(bool){
      let payload = JsonWebToken.decode(tokenWithoutBear)
      Js.Dict.set(Request.asJsonObject(req), "user", payload)  
      res |> next(Next.middleware)
    } else{
      res |> Response.status(Forbidden) |> Response.sendJson(Json.Encode.(
        object_([
          (
            "error",
            string("INVALID JWT"),
          ),
        ])
      ),
      )
    }
  }
  | None => res |> Response.status(Unauthorized) |> Response.sendJson(Json.Encode.(
    object_([
      (
        "error",
        string("NO JWT IN HEADER"),
      ),
    ])
  ),
  )
  };
});

let badPrivilegeError = res =>  res |> Response.status(Unauthorized) |> Response.sendJson(Json.Encode.(
  object_([
    (
      "error",
      string("CHECK YOUR PRIVILEGE"),
    ),
  ])
),
)

let permit = (role : string) => {
  Middleware.from((next, req, res) => {

  switch (Js.Dict.get(Request.asJsonObject(req), "user")) {
  | None => res |> Response.status(Unauthorized) |> Response.sendJson(Json.Encode.(
    object_([
      (
        "error",
        string("BAD PAYLOAD IN JWT"),
      ),
    ])
  ),
  )
  | Some(user) =>
    switch (Js.Json.decodeObject(user)) {
    | Some(myUser) => {
      switch(Js_dict.get(myUser, "userRole")){
        | Some(userRole) => {
          switch (Js.Json.decodeString(userRole)) {
            | Some(userRole) => {
              if(role == userRole){
                res |> next(Next.middleware)
              } else {
                res |> badPrivilegeError
              }
            }
            |None => res |> badPrivilegeError
          }
        }
        | None =>  res |> badPrivilegeError
      }
    } 
    | _ => res |> badPrivilegeError
    }
  };
  });
};

