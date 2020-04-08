module User: {
  type t; // this is an abastraction to enforce creation with make functions
  let make: (string, string, string, string, string, string, string) => t;
  let modifyUserRole: (string, t) => t;
  let getEmail: t => string;
  let getPseudo: t => string;
  let getPassword: t => string;
  let getName: t => string;
  let getSurname: t => string;
  let getUserRole: t => string;
  let getToken: t => string;
  let isAdmin: t => bool;//allow to know if assignment request could be accepted by this user
  let fromJson: Js.Json.t => t;
  let fromString: string => option(t);
  let toJson: t => Js.Json.t;
  let toString: t => string;
} = {
  type t = {
    email: string,
    pseudo: string,
    password: string,
    name: string,
    surname: string,
    userRole: string,
    token: string,
  };

  // create a value of the type Users.t
  let make = (email, pseudo, password, name, surname, userRole, token) => {
    {email, pseudo, password, name, surname, userRole, token}
  }
  let makeWithEmailHash = (email,password) => make(email, "pseudo", password,"name","surname","userrole","token");


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
  let getToken = user => user.token;
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
      token: json |> field("token", string),
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
        ("token", string(user.token)),
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