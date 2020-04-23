module User: {
  type t; // this is an abastraction to enforce creation with make functions
  let make: (string, string, string, string, string, string) => t;
  let modifyUserRole: (string, t) => t;
  let getEmail: t => string;
  let getPseudo: t => string;
  let getPassword: t => string;
  let getName: t => string;
  let getSurname: t => string;
  let getUserRole: t => string;
  let isAdmin: t => bool;//allow to know if assignment request could be accepted by this user
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toJsonWithoutPassword: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = {
    email: string,
    pseudo: string,
    password: string,
    name: string,
    surname: string,
    userRole: string,
  };

  // create a value of the type Users.t
  let make = (email, pseudo, password, name, surname, userRole) => {
    {email, pseudo, password, name, surname, userRole}
  }


  // Setters
  let modifyUserRole = (newUserRole, user) => {...user, userRole: newUserRole};

  // Getters are mandatory because of abstraction,
  // while it is a good practice for accessing records members, like for object members
  let getEmail = user => user.email;
  let getPseudo = user => user.pseudo;
  let getPassword = user => user.password;
  let getName = user => user.name;
  let getSurname = user => user.surname;
  let getUserRole = user => user.userRole;
  let isAdmin = user => user.userRole=="Administrateur";

  //let bool_of_int = value => value === 1;
  let fromJson = json =>
    Json.Decode.{
      email: json |> field("email", string),
      pseudo: json |> field("pseudo", string),
      password: json |> field("password", string),
      name: json |> field("name", string),
      surname: json |> field("surname", string),
      userRole: json |> field("userRole", string),
    };

  let fromString = jsonString =>
    switch (Json.parse(jsonString)) {
    | Some(validJson) => Some(fromJson(validJson))
    | None => None
    };

  let toJson = user =>
    // Json module comes from bs-json, it is not Js.Json module
    // Use of bs-json to encode json in expressive way while you can use Js.Json / Js.Dict in an imperative way
    Json.Encode.(
      object_([
        ("email", string(user.email)),
        ("pseudo", string(user.pseudo)),
        ("password", string(user.password)),
        ("name", string(user.name)),
        ("surname", string(user.surname)),
        ("userRole", string(user.userRole)),
      ])
    );

    let toJsonWithoutPassword = user =>
    // Json module comes from bs-json, it is not Js.Json module
    // Use of bs-json to encode json in expressive way while you can use Js.Json / Js.Dict in an imperative way
    Json.Encode.(
      object_([
        ("email", string(user.email)),
        ("pseudo", string(user.pseudo)),
        ("name", string(user.name)),
        ("surname", string(user.surname)),
        ("userRole", string(user.userRole)),
      ])
    );

  let toString = user => toJson(user) |> Js.Json.stringify;
};

module Users: {
  type t = list(User.t);
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = list(User.t);

  let fromJson: Js.Json.t => t =
    json => json |> Json.Decode.(list(User.fromJson));

  let fromString = jsonString =>
    switch (Json.parse(jsonString)) {
    | Some(validJson) => Some(fromJson(validJson))
    | None => None
    };

  let toJson = users =>
    Array.of_list(users)  // convert list(User.t) to array(User.t)
    |> Array.map(item => User.toJson(item))  // create an array(Js.Json.t)
    |> Json.Encode.jsonArray;
  let toString = users => toJson(users) |> Js.Json.stringify;
};


/* ASSIGNMENT REQUEST :
*/

module AssignmentRequest: {
  type t; // this is an abastraction to enforce creation with make functions
  let make: (int, string, string, bool, bool) => t;
  let getAssignmentRequestId: t => int;
  let getEmailUserForAssignment: t => string;
  let getRoleRequest: t => string;
  let getDecision: t => bool;
  let getProcessed: t => bool;
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = {
    assignmentRequestId: int,
    emailUserForAssignment: string,
    roleRequest: string,
    decision: bool,
    processed: bool,
  };

  // create a value of the type AssignmentRequests.t
  let make = (assignmentRequestId, emailUserForAssignment, roleRequest, decision, processed) => {
    {assignmentRequestId, emailUserForAssignment, roleRequest, decision, processed}
  }

  // Getters are mandatory because of abstraction,
  // while it is a good practice for accessing records members, like for object members
  let getAssignmentRequestId = assignmentrequest => assignmentrequest.assignmentRequestId;
  let getEmailUserForAssignment = assignmentrequest => assignmentrequest.emailUserForAssignment;
  let getRoleRequest = assignmentrequest => assignmentrequest.roleRequest;
  let getDecision = assignmentrequest => assignmentrequest.decision;
  let getProcessed = assignmentrequest => assignmentrequest.processed;
  
  let bool_of_int = value => value === 1;

  //let bool_of_int = value => value === 1;
  let fromJson = json =>
    Json.Decode.{
      assignmentRequestId: json |> field("assignmentRequestId", int),
      emailUserForAssignment: json |> field("emailUserForAssignment", string),
      roleRequest: json |> field("roleRequest", string),
      decision: json |> field("decision", int) |> bool_of_int,
      processed: json |> field("processed", int) |> bool_of_int,
    };

  let fromString = jsonString =>
    switch (Json.parse(jsonString)) {
    | Some(validJson) => Some(fromJson(validJson))
    | None => None
    };


  let toJson = assignmentrequest =>
    // Json module comes from bs-json, it is not Js.Json module
    // Use of bs-json to encode json in expressive way while you can use Js.Json / Js.Dict in an imperative way
    Json.Encode.(
      object_([
        ("assignmentRequestId", int(assignmentrequest.assignmentRequestId)),
        ("emailUserForAssignment", string(assignmentrequest.emailUserForAssignment)),
        ("roleRequest", string(assignmentrequest.roleRequest)),
        ("decision", bool(assignmentrequest.decision)),
        ("processed", bool(assignmentrequest.processed)),
      ])
    );

  let toString = user => toJson(user) |> Js.Json.stringify;
};

module AssignmentRequests: {
  type t = list(AssignmentRequest.t);
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = list(AssignmentRequest.t);

  let fromJson: Js.Json.t => t =
    json => json |> Json.Decode.(list(AssignmentRequest.fromJson));

  let fromString = jsonString =>
    switch (Json.parse(jsonString)) {
    | Some(validJson) => Some(fromJson(validJson))
    | None => None
  };

  let toJson = assignmentrequests =>
    Array.of_list(assignmentrequests)  
    |> Array.map(item => AssignmentRequest.toJson(item))  
    |> Json.Encode.jsonArray;
  let toString = assignmentrequests => toJson(assignmentrequests) |> Js.Json.stringify;
};